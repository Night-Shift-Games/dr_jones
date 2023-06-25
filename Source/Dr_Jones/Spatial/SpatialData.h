#pragma once

#include "Dr_Jones.h"

struct FGeoLocation;

UENUM()
enum class ESpatialDataTexelAttributeType : uint8
{
	Raw = 0,
	Float,
	Int32,
	Byte,
	
	NUM UMETA(Hidden)
};

template<typename>
struct TGetSpatialTexelAttributeType { };

template<>
struct TGetSpatialTexelAttributeType<float>
{
	static constexpr ESpatialDataTexelAttributeType Type = ESpatialDataTexelAttributeType::Float;
};

template<>
struct TGetSpatialTexelAttributeType<int32>
{
	static constexpr ESpatialDataTexelAttributeType Type = ESpatialDataTexelAttributeType::Int32;
};

template<>
struct TGetSpatialTexelAttributeType<uint8>
{
	static constexpr ESpatialDataTexelAttributeType Type = ESpatialDataTexelAttributeType::Byte;
};

struct FSpatialDataTexelAttributeDescriptor
{
	FName Name;
	int32 Stride;
	uint8 Size;
	ESpatialDataTexelAttributeType Type;

	friend FArchive& operator<<(FArchive& Ar, FSpatialDataTexelAttributeDescriptor& Attribute);
};

class DR_JONES_API FSpatialDataBufferLayout
{
public:
	FSpatialDataBufferLayout();
	
	int32 GetTexelSize() const;
	const FSpatialDataTexelAttributeDescriptor* FindAttributeByName(const FName& Name) const;

	const TArray<FSpatialDataTexelAttributeDescriptor>& GetAttributes() const;

	friend FArchive& operator<<(FArchive& Ar, FSpatialDataBufferLayout& Layout);

private:
	void CacheTexelSize() const;
	
private:
	TArray<FSpatialDataTexelAttributeDescriptor> Attributes;
	mutable int32 CachedTexelSize;

	friend class FSpatialDataBufferBuilder;
};

class DR_JONES_API FSpatialDataTexelAccessor
{
public:
	template<typename DataT>
	const DataT* GetAttributeData(const FName& Name) const;
	
private:
	FSpatialDataTexelAccessor(const FSpatialDataBufferLayout& Layout, const uint8* Data);

	const FSpatialDataBufferLayout& Layout;
	const uint8* Data;

	friend class FSpatialDataBuffer;
};

class DR_JONES_API FSpatialDataBuffer
{
public:
	using ByteArray = TArray<uint8>;

	static TSharedRef<FSpatialDataBuffer> Default(const FIntVector4& Dimensions);

	FSpatialDataTexelAccessor Sample3D(int32 X, int32 Y, int32 Z) const;
	FSpatialDataTexelAccessor SampleNormalized3D(float X, float Y, float Z) const;
	
	bool WriteAttributeRaw(int32 Index, const FName& Attribute, const void* InData, int32 Size);
	bool ReadAttributeRaw(int32 Index, const FName& Attribute, void* OutData, int32 Size) const;
	bool TryCopyAttributeRaw(int32 Index, const FName& Attribute, FSpatialDataBuffer& OutOther) const;

	int32 GetBufferSize() const;
	int32 GetTexelCount() const;

	void SetRawData(const ByteArray& Bytes);
	const ByteArray& GetRawData() const;
	TSharedPtr<const FSpatialDataBufferLayout> GetLayout() const;
	
	friend FArchive& operator<<(FArchive& Ar, TSharedPtr<FSpatialDataBuffer>& InOutBufferPtr);

private:
	FSpatialDataBuffer();
	
	bool IsIndexValid(int32 Index) const;
	int32 GetOffsetForIndex(int32 Index) const;

private:
	TSharedPtr<FSpatialDataBufferLayout> Layout;
	ByteArray Data;
	int32 TexelCount;
	FIntVector4 Dimensions;

	friend class FSpatialDataBufferBuilder;
};

class DR_JONES_API FSpatialDataBufferBuilder
{
public:
	FSpatialDataBufferBuilder();
	
	template<typename T>
	void AddAttribute(const FName& Name);
	void AddAttributesFromExistingLayout(TSharedRef<FSpatialDataBufferLayout> Layout);
	
	void AddAttributeDynamic(const FSpatialDataTexelAttributeDescriptor& AttributeDescriptor);

	TSharedRef<FSpatialDataBuffer> Build(const FIntVector4& Dimensions) const;
	TSharedRef<FSpatialDataBuffer> Rebuild(const FSpatialDataBuffer& Other) const;

	void Reset();
	int32 GetAttributeCount() const;

private:
	void AddAttribute_Internal(const FName& Name, int32 AttributeSize, ESpatialDataTexelAttributeType AttributeType);
	
private:
	TArray<FSpatialDataTexelAttributeDescriptor> Attributes;
	int32 CurrentStride;
};

// Inline implementation:

// FSpatialDataBufferLayout impl

FORCEINLINE const TArray<FSpatialDataTexelAttributeDescriptor>& FSpatialDataBufferLayout::GetAttributes() const
{
	return Attributes;
}

// FSpatialDataTexelAccessor impl

template <typename DataT>
const DataT* FSpatialDataTexelAccessor::GetAttributeData(const FName& Name) const
{
	const FSpatialDataTexelAttributeDescriptor* AttributeDescriptor = Layout.FindAttributeByName(Name);
	if (!AttributeDescriptor)
	{
		UE_LOG(LogDrJones, Warning, TEXT("Attribute %s does not exist."), *Name.ToString());
		return nullptr;
	}

	if (!ensureAlwaysMsgf(AttributeDescriptor->Type == TGetSpatialTexelAttributeType<DataT>::Type,
		TEXT("Tried to get attribute data using a wrong type.")))
	{
		return nullptr;
	}

	return reinterpret_cast<const DataT*>(Data + AttributeDescriptor->Stride);
}

// FSpatialDataBuffer impl

FORCEINLINE int32 FSpatialDataBuffer::GetBufferSize() const
{
	check(Data.Num() == TexelCount * Layout->GetTexelSize());
	return Data.Num();
}

FORCEINLINE int32 FSpatialDataBuffer::GetTexelCount() const
{
	check(TexelCount == Data.Num() / Layout->GetTexelSize());
	return TexelCount;
}

FORCEINLINE const FSpatialDataBuffer::ByteArray& FSpatialDataBuffer::GetRawData() const
{
	return Data;
}

FORCEINLINE TSharedPtr<const FSpatialDataBufferLayout> FSpatialDataBuffer::GetLayout() const
{
	return Layout;
}

FORCEINLINE bool FSpatialDataBuffer::IsIndexValid(int32 Index) const
{
	return Index >= 0 && Index < TexelCount;
}

FORCEINLINE int32 FSpatialDataBuffer::GetOffsetForIndex(int32 Index) const
{
	return Index * Layout->GetTexelSize();
}

// FSpatialDataBufferBuilder impl

template <typename T>
FORCEINLINE void FSpatialDataBufferBuilder::AddAttribute(const FName& Name)
{
	static_assert(sizeof(T) <= sizeof(uint64),
		"Attributes should not be larger than 8 bytes. More attributes should be used to store that kind of data instead.");
	
	AddAttribute_Internal(Name, sizeof(T), TGetSpatialTexelAttributeType<T>::Type);
}

FORCEINLINE int32 FSpatialDataBufferBuilder::GetAttributeCount() const
{
	return Attributes.Num();
}
