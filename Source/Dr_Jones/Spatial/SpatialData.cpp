#include "SpatialData.h"

FSpatialDataBufferLayout::FSpatialDataBufferLayout()
	: CachedTexelSize(0)
{
}

int32 FSpatialDataBufferLayout::GetTexelSize() const
{
	if (Attributes.IsEmpty())
	{
		return 0;
	}
	if (CachedTexelSize == 0)
	{
		CacheTexelSize();
	}
	return CachedTexelSize;
}

const FSpatialDataTexelAttributeDescriptor* FSpatialDataBufferLayout::FindAttributeByName(const FName& Name) const
{
	return Attributes.FindByPredicate([&Name](const FSpatialDataTexelAttributeDescriptor& Segment)
	{
		return Segment.Name == Name;
	});
}

void FSpatialDataBufferLayout::CacheTexelSize() const
{
	CachedTexelSize = 0;
	for (const FSpatialDataTexelAttributeDescriptor& SegmentDescriptor : Attributes)
	{
		CachedTexelSize += SegmentDescriptor.Size;
	}
}

FSpatialDataTexelAccessor::FSpatialDataTexelAccessor(const FSpatialDataBufferLayout& Layout, const uint8* Data)
	: Layout(Layout),
	  Data(Data)
{
}

FSpatialDataBuffer::FSpatialDataBuffer()
	: TexelCount(0)
{
}

FSpatialDataTexelAccessor FSpatialDataBuffer::Sample3D(int32 X, int32 Y, int32 Z) const
{
	check(Layout.IsValid());
	checkf(X >= 0 && X < Dimensions.X, TEXT("Invalid X coordinate"));
	checkf(Y >= 0 && Y < Dimensions.Y, TEXT("Invalid Y coordinate"));
	checkf(Z >= 0 && Z < Dimensions.Z, TEXT("Invalid Z coordinate"));
	checkf(IsIndexValid(X * Y * Z), TEXT("Index out of bounds"));
	
	const int32 TexelSize = Layout->GetTexelSize();
	const int32 TexelIndex = X + Y * Dimensions.X + Z * Dimensions.X * Dimensions.Y;
	const int32 TexelIndexInByteArray = TexelIndex * TexelSize;
	return FSpatialDataTexelAccessor(*Layout.Get(), &Data[TexelIndexInByteArray]);
}

FSpatialDataTexelAccessor FSpatialDataBuffer::SampleNormalized3D(float X, float Y, float Z) const
{
	check(X >= 0.0f && X <= 1.0f);
	check(Y >= 0.0f && Y <= 1.0f);
	check(Z >= 0.0f && Z <= 1.0f);

	// TODO: Lerp between all dimensions
	
	return Sample3D(
		FMath::Min(Dimensions.X - 1, static_cast<int32>(X * Dimensions.X)),
		FMath::Min(Dimensions.Y - 1, static_cast<int32>(Y * Dimensions.Y)),
		FMath::Min(Dimensions.Z - 1, static_cast<int32>(Z * Dimensions.Z)));
}

bool FSpatialDataBuffer::WriteAttributeRaw(int32 Index, const FName& Attribute, const void* InData, int32 Size)
{
	check(IsIndexValid(Index));
	check(InData);
	check(Size > 0);
	
	const FSpatialDataTexelAttributeDescriptor* AttributeDescriptor = Layout->FindAttributeByName(Attribute);
	if (!AttributeDescriptor)
	{
		return false;
	}
	
	checkf(Size <= AttributeDescriptor->Size, TEXT("In memory buffer is too big to fit in the attribute's memory."));
	
	const int32 TexelOffset = GetOffsetForIndex(Index);
	const int32 AttributeOffset = TexelOffset + AttributeDescriptor->Stride;
	
	FMemory::Memcpy(&Data[AttributeOffset], InData, AttributeDescriptor->Size);  

	return true;
}

bool FSpatialDataBuffer::ReadAttributeRaw(int32 Index, const FName& Attribute, void* OutData, int32 Size) const
{
	check(IsIndexValid(Index));
	check(OutData);
	check(Size > 0);
	
	const FSpatialDataTexelAttributeDescriptor* AttributeDescriptor = Layout->FindAttributeByName(Attribute);
	if (!AttributeDescriptor)
	{
		return false;
	}

	checkf(Size >= AttributeDescriptor->Size, TEXT("Out memory buffer is too small to fit the attribute value."));
	
	const int32 TexelOffset = GetOffsetForIndex(Index);
	const int32 AttributeOffset = TexelOffset + AttributeDescriptor->Stride;

	FMemory::Memcpy(OutData, &Data[AttributeOffset], AttributeDescriptor->Size);  

	return true;
}

bool FSpatialDataBuffer::TryCopyAttributeRaw(int32 Index, const FName& Attribute, FSpatialDataBuffer& OutOther) const
{
	check(IsIndexValid(Index));
	
	const FSpatialDataTexelAttributeDescriptor* AttributeDescriptor = Layout->FindAttributeByName(Attribute);
	if (!AttributeDescriptor)
	{
		return false;
	}

	const FSpatialDataTexelAttributeDescriptor* OtherAttributeDescriptor = OutOther.Layout->FindAttributeByName(Attribute);
	if (!OtherAttributeDescriptor)
	{
		return false;
	}

	const int32 TexelOffset = GetOffsetForIndex(Index);
	const int32 AttributeOffset = TexelOffset + AttributeDescriptor->Stride;

	const int32 OtherTexelOffset = OutOther.GetOffsetForIndex(Index);
	const int32 OtherAttributeOffset = OtherTexelOffset + OtherAttributeDescriptor->Stride;

	if (!ensureAlwaysMsgf(AttributeDescriptor->Size == OtherAttributeDescriptor->Size, TEXT("Attribute sizes don't match")))
	{
		return false;
	}

	FMemory::Memcpy(&OutOther.Data[OtherAttributeOffset], &Data[AttributeOffset], AttributeDescriptor->Size);

	return true;
}

FSpatialDataBufferBuilder::FSpatialDataBufferBuilder()
	: CurrentStride(0)
{
}

TSharedRef<FSpatialDataBuffer> FSpatialDataBufferBuilder::Build(const FIntVector4& Dimensions) const
{
	check(Dimensions.X >= 0);
	check(Dimensions.Y >= 0);
	check(Dimensions.Z >= 0);
	check(Dimensions.W >= 0);
	check(Dimensions.X * Dimensions.Y * Dimensions.Z * Dimensions.W <= sizeof(int32));
	
	const TSharedRef<FSpatialDataBufferLayout> Layout = MakeShareable(new FSpatialDataBufferLayout);
	Layout->Attributes = Attributes;
	Layout->CacheTexelSize();
	
	TSharedRef<FSpatialDataBuffer> Buffer = MakeShareable(new FSpatialDataBuffer);
	Buffer->Layout = Layout;
	Buffer->Dimensions = Dimensions;
	Buffer->TexelCount = Dimensions.X * Dimensions.Y * Dimensions.Z * Dimensions.W;
	Buffer->Data.Reserve(Buffer->TexelCount * Layout->CachedTexelSize);

	return Buffer;
}

TSharedRef<FSpatialDataBuffer> FSpatialDataBufferBuilder::Rebuild(const FSpatialDataBuffer& Other) const
{
	TSharedRef<FSpatialDataBuffer> Buffer = Build(Other.Dimensions);

	// TODO: Totally unoptimized, but should work for now
	
	const int32 BufferLength = Other.GetBufferSize();
	for (int32 Index = 0; Index < BufferLength; ++Index)
	{
		for (const FSpatialDataTexelAttributeDescriptor& OtherAttribute : Other.Layout->Attributes)
		{
			Other.TryCopyAttributeRaw(Index, OtherAttribute.Name, Buffer.Get());
		}
	}

	return Buffer;
}

void FSpatialDataBufferBuilder::Reset()
{
	Attributes.Empty();
	CurrentStride = 0;
}

void FSpatialDataBufferBuilder::AddAttribute_Internal(const FName& Name, int32 AttributeSize,
	ESpatialDataTexelAttributeType AttributeType)
{
	FSpatialDataTexelAttributeDescriptor Descriptor;
	Descriptor.Name = Name;
	Descriptor.Size = AttributeSize;
	Descriptor.Type = AttributeType;
	Descriptor.Stride = CurrentStride;
	Attributes.Add(Descriptor);
	
	CurrentStride += AttributeSize; 
}
