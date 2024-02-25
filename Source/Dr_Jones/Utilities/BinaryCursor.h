#pragma once

struct FBinaryCursor
{
	const uint8* Buffer;
	const uint8* BufferEnd;
	const uint8* Pointer;

	FBinaryCursor(const uint8* Buffer, const uint8* BufferEnd)
		: Buffer(Buffer),
		  BufferEnd(BufferEnd),
		  Pointer(Buffer)
	{
	}

	template <size_t HeaderSize>
	FORCEINLINE bool VerifyHeader(const uint8 (&Header)[HeaderSize])
	{
		const bool bValid = GetBytesLeft() >= HeaderSize && FMemory::Memcmp(Pointer, Header, HeaderSize) == 0;
		if (bValid)
		{
			Advance(HeaderSize);
		}
		return bValid;
	}

	template <typename T>
	FORCEINLINE TOptional<T> Read()
	{
		if (!ensureAlwaysMsgf(GetBytesLeft() >= sizeof(T), TEXT("There are not enough bytes to read from.")))
		{
			return NullOpt;
		}

		const T* ValuePtr = reinterpret_cast<const T*>(Pointer);
		Advance(sizeof(T));
		return *ValuePtr;
	}

	FORCEINLINE bool ReadRaw(size_t ReadSize, void* OutBuffer, size_t BufferSize)
	{
		checkf(ReadSize <= BufferSize, TEXT("Cannot read %i bytes into a smaller buffer."), ReadSize);
		if (!ensureAlwaysMsgf(GetBytesLeft() >= ReadSize, TEXT("There are not enough bytes to read from.")))
		{
			return false;
		}

		FMemory::Memcpy(OutBuffer, Pointer, ReadSize);
		Advance(ReadSize);

		return true;
	}

	template <typename T>
	FORCEINLINE bool ReadRaw(size_t ReadSize, TArray<T>& Buffer)
	{
		return ReadRaw(ReadSize, Buffer.GetData(), Buffer.Num());
	}

	DR_JONES_API bool ReadCompressed(size_t ReadSize, void* OutBuffer, size_t BufferSize);

	template <typename T>
	FORCEINLINE bool ReadCompressed(size_t ReadSize, TArray<T>& Buffer)
	{
		return ReadCompressed(ReadSize, Buffer.GetData(), Buffer.Num());
	}

	FORCEINLINE TOptional<const char*> ReadZeroTerminatedAsciiString()
	{
		if (!ensureAlwaysMsgf(GetBytesLeft() > 0,
		                      TEXT("There are not enough bytes to read a zero terminated ascii string.")))
		{
			return NullOpt;
		}

		const char* Start = reinterpret_cast<const char*>(Pointer);
		while (*Pointer)
		{
			Advance(1);
			if (!ensureAlwaysMsgf(!Eof(), TEXT("Tried to read a zero terminated string that never terminated.")))
			{
				return nullptr;
			}
		}

		Advance(1);
		return Start;
	}

	FORCEINLINE size_t GetBytesLeft() const
	{
		return static_cast<size_t>(std::max(BufferEnd - Pointer, 0ll));
	}

	FORCEINLINE void Advance(size_t Count)
	{
		checkf(Count <= GetBytesLeft(), TEXT("The buffer is too small to advance the cursor by %i bytes."), Count);
		Pointer += Count;
	}

	FORCEINLINE bool Eof() const
	{
		return GetBytesLeft() <= 0;
	}
};
