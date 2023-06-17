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
		Bytes = SpatialData->GetRawData();
		Layout = MakeShared<FSpatialDataBufferLayout>(*SpatialData->GetLayout());
	}

	Ar << *Layout;
	Ar << Bytes;

	if (Ar.IsLoading())
	{
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
