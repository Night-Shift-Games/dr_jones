// Property of Night Shift Games, all rights reserved.

#include "WorldSpatialData.h"

UWorldSpatialData::UWorldSpatialData() :
	SpatialData(FSpatialDataBuffer::Default(DefaultBufferDimensions))
{
}

void UWorldSpatialData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	
	FSpatialDataBuffer::ByteArray Bytes;
	TSharedPtr<FSpatialDataBufferLayout> Layout;
	if (Ar.IsLoading())
	{
		Layout = MakeShared<FSpatialDataBufferLayout>();
	}
	else if (Ar.IsSaving())
	{
		Layout = MakeShared<FSpatialDataBufferLayout>(*SpatialData->GetLayout());
	}
	
	Ar << *Layout;
	
	if (Ar.IsSaving())
	{
		Bytes = SpatialData->GetRawData();

		// Save uncompressed size
		int32 BufferSize = Bytes.Num();
		Ar << BufferSize;

		Ar.SerializeCompressedNew(Bytes.GetData(), BufferSize);
	}
	else if (Ar.IsLoading())
	{
		// Load uncompressed size
		int32 BufferSize;
		Ar << BufferSize;
		
		Bytes.SetNumUninitialized(BufferSize);
		
		Ar.SerializeCompressedNew(Bytes.GetData(), BufferSize);

		FSpatialDataBufferBuilder Builder;
		Builder.AddAttributesFromExistingLayout(Layout.ToSharedRef());
		const TSharedRef<FSpatialDataBuffer> Buffer = Builder.Build(DefaultBufferDimensions);
		Buffer->SetRawData(Bytes);
		
		SetBuffer(Buffer);
	}
}

void UWorldSpatialData::SetBuffer(const TSharedRef<FSpatialDataBuffer>& Buffer)
{
	SpatialData = Buffer;
}
