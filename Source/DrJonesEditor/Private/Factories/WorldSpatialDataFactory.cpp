// Property of Night Shift Games, all rights reserved.

#include "WorldSpatialDataFactory.h"

#include "World/WorldSpatialData.h"

UWorldSpatialDataFactory::UWorldSpatialDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWorldSpatialData::StaticClass();
	
	bCreateNew = false;
	// bEditAfterNew = true;
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

struct FBinaryCursor
{
	const uint8* Buffer;
	const uint8* BufferEnd;
	const uint8* Pointer;

	FBinaryCursor(const uint8* Buffer, const uint8* BufferEnd)
		: Buffer(Buffer)
		, BufferEnd(BufferEnd)
		, Pointer(Buffer)
	{}

	FORCEINLINE size_t GetBytesLeft() const
	{
		return static_cast<size_t>(std::max(BufferEnd - Pointer, 0ll));
	};

	FORCEINLINE bool Advance(size_t Count)
	{
		if (Count > GetBytesLeft())
		{
			Pointer += GetBytesLeft();
			return false;
		}
		
		Pointer += Count;
		return true;
	}

	FORCEINLINE bool Eof() const
	{
		return GetBytesLeft() <= 0;
	}

	template<size_t HeaderSize>
	FORCEINLINE bool VerifyHeader(const uint8(&Header)[HeaderSize])
	{
		const bool bValid = GetBytesLeft() >= HeaderSize && FMemory::Memcmp(Pointer, Header, HeaderSize) == 0;
		if (bValid)
		{
			Advance(HeaderSize);
		}
		return bValid;
	}

	template<typename T>
	FORCEINLINE T Read()
	{
		check(GetBytesLeft() >= sizeof(T));
		const T* ValuePtr = reinterpret_cast<const T*>(Pointer);
		Pointer += sizeof(T);
		return *ValuePtr;
	}
	
	FORCEINLINE const char* ReadZeroTerminatedAsciiString()
	{
		check(GetBytesLeft() > 0);
		const char* Start = reinterpret_cast<const char*>(Pointer);
		while (*Pointer)
		{
			Advance(1);
			if (Eof())
			{
				return nullptr;
			}
		}
		Advance(1);
		return Start;
	}
};

UObject* UWorldSpatialDataFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	static constexpr uint8 NSGSpatialData_Header[16] = { 0x4E, 0x53, 0x47, 0xFF, 0x53, 0x70, 0x61, 0x74, 0x69, 0x61, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static constexpr uint8 NSGSpatialData_DimHeader[4] = { 0x44, 0x49, 0x4D, 0xFA };
	static constexpr uint8 NSGSpatialData_AttrHeader[4] = { 0x41, 0x54, 0x52, 0xFA };
	static constexpr uint8 NSGSpatialData_DataHeader[4] = { 0x44, 0x41, 0x54, 0xFA };
	
	FBinaryCursor Cursor(Buffer, BufferEnd);
	
	if (!Cursor.VerifyHeader(NSGSpatialData_Header))
		return nullptr;

	if (!Cursor.VerifyHeader(NSGSpatialData_DimHeader))
		return nullptr;

	FIntVector4 Dimensions;
	Dimensions.X = Cursor.Read<int32>();
	Dimensions.Y = Cursor.Read<int32>();
	Dimensions.Z = Cursor.Read<int32>();
	Dimensions.W = Cursor.Read<int32>();

	FSpatialDataBufferBuilder Builder;
	bool bHasAttributes = false;
	while (Cursor.VerifyHeader(NSGSpatialData_AttrHeader))
	{
		FSpatialDataTexelAttributeDescriptor Attribute;
		Attribute.Name = Cursor.ReadZeroTerminatedAsciiString();
		Attribute.Size = Cursor.Read<uint8>();
		Attribute.Type = Cursor.Read<ESpatialDataTexelAttributeType>();
		Builder.AddAttribute(Attribute);
		bHasAttributes = true;
	}
	if (!bHasAttributes)
		return nullptr;
	
	const TSharedRef<FSpatialDataBuffer> SpatialDataBuffer = Builder.Build(Dimensions);

	if (!Cursor.VerifyHeader(NSGSpatialData_DataHeader))
		return nullptr;

	const int32 DataSize = Cursor.Read<int32>();
	TArray<uint8> Bytes;
	Bytes.AddUninitialized(DataSize);
	FMemory::Memcpy(Bytes.GetData(), Cursor.Pointer, DataSize);

	SpatialDataBuffer->SetRawData(Bytes);
	
	UWorldSpatialData* WorldSpatialData = NewObject<UWorldSpatialData>(InParent, InClass, InName, Flags);
	WorldSpatialData->SetBuffer(SpatialDataBuffer);

	return WorldSpatialData;
}
