// Property of Night Shift Games, all rights reserved.

#include "WorldSpatialDataFactory.h"

#include "Serialization/BinaryCursor.h"
#include "World/WorldSpatialData.h"

UWorldSpatialDataFactory::UWorldSpatialDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWorldSpatialData::StaticClass();
	
	bCreateNew = false;
	bEditorImport = true;
	bText = false;

	Formats.Add(TEXT("nsgsd;NSG Spatial Data"));
}

UObject* UWorldSpatialDataFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UWorldSpatialData* WorldSpatialData = NewObject<UWorldSpatialData>(InParent, InClass, InName, Flags);
	check(WorldSpatialData);
	
	WorldSpatialData->SetBuffer(FSpatialDataBuffer::Default(UWorldSpatialData::DefaultBufferDimensions));
	
	return WorldSpatialData;
}

bool UWorldSpatialDataFactory::FactoryCanImport(const FString& Filename)
{
	FString Extension = FPaths::GetExtension(Filename);

	return Formats.ContainsByPredicate(
		[&Extension](const FString& Format)
		{
			return Format.StartsWith(Extension);
		});
}

#define GUARD(Expr) if (!(Expr)) return nullptr
#define GUARDED_READ(OutValue, ReadExpr) { auto Opt = ReadExpr; if (Opt.IsSet()) OutValue = Opt.GetValue(); else return nullptr; }

UObject* UWorldSpatialDataFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	static constexpr uint8 NSGSpatialData_Header[16] = {
		0x4E, 0x53, 0x47, 0xFF, 0x53, 0x70, 0x61, 0x74, 0x69, 0x61, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	static constexpr uint8 NSGSpatialData_DimHeader[4] = { 0x44, 0x49, 0x4D, 0xFA };
	static constexpr uint8 NSGSpatialData_AttrHeader[4] = { 0x41, 0x54, 0x52, 0xFA };
	static constexpr uint8 NSGSpatialData_DataHeader[4] = { 0x44, 0x41, 0x54, 0xFA };
	
	FBinaryCursor Cursor(Buffer, BufferEnd);
	
	GUARD(Cursor.VerifyHeader(NSGSpatialData_Header));

	// Read dimensions
	
	GUARD(Cursor.VerifyHeader(NSGSpatialData_DimHeader));

	FIntVector4 Dimensions;
	GUARDED_READ(Dimensions.X, Cursor.Read<int32>());
	GUARDED_READ(Dimensions.Y, Cursor.Read<int32>());
	GUARDED_READ(Dimensions.Z, Cursor.Read<int32>());
	GUARDED_READ(Dimensions.W, Cursor.Read<int32>());

	// Read attributes
	
	FSpatialDataBufferBuilder Builder;
	while (Cursor.VerifyHeader(NSGSpatialData_AttrHeader))
	{
		FSpatialDataTexelAttributeDescriptor Attribute;
		GUARDED_READ(Attribute.Name, Cursor.ReadZeroTerminatedAsciiString())
		GUARDED_READ(Attribute.Size, Cursor.Read<uint8>())
		GUARDED_READ(Attribute.Type, Cursor.Read<ESpatialDataTexelAttributeType>())
		Builder.AddAttributeDynamic(Attribute);
	}
	GUARD(Builder.GetAttributeCount() > 0);
	
	const TSharedRef<FSpatialDataBuffer> SpatialDataBuffer = Builder.Build(Dimensions);

	// Read data bytes
	
	TArray<uint8> Bytes;
	while (Cursor.VerifyHeader(NSGSpatialData_DataHeader))
	{
		int32 DataSize;
		GUARDED_READ(DataSize, Cursor.Read<int32>());

		Bytes.AddUninitialized(DataSize);
		GUARD(Cursor.ReadRaw(DataSize, Bytes));
	}
	
	SpatialDataBuffer->SetRawData(Bytes);

	// Create the object
	
	UWorldSpatialData* WorldSpatialData = NewObject<UWorldSpatialData>(InParent, InClass, InName, Flags);
	WorldSpatialData->SetBuffer(SpatialDataBuffer);

	return WorldSpatialData;
}
