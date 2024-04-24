#include "BinaryCursor.h"

#include "zlib.h"

bool FBinaryCursor::ReadCompressed(size_t ReadSize, void* OutBuffer, size_t BufferSize)
{
	checkf(ReadSize <= BufferSize, TEXT("Cannot read %i bytes into a smaller buffer."), ReadSize);
	if (!ensureAlwaysMsgf(GetBytesLeft() >= ReadSize, TEXT("There are not enough bytes to read from.")))
	{
		return false;
	}

	TArray<uint8> CompressedBytes;
	CompressedBytes.SetNumUninitialized(ReadSize);
	if (!ReadRaw(ReadSize, CompressedBytes))
	{
		return false;
	}

	z_stream ZLibStream;
	ZLibStream.zalloc = nullptr;
	ZLibStream.zfree = nullptr;
	ZLibStream.opaque = nullptr;

	ZLibStream.avail_in = CompressedBytes.Num();
	ZLibStream.next_in = CompressedBytes.GetData();
	ZLibStream.avail_out = BufferSize;
	ZLibStream.next_out = static_cast<Bytef*>(OutBuffer);
	
	verify(inflateInit(&ZLibStream) >= 0);
	if (inflate(&ZLibStream, Z_FINISH) < 0)
	{
		return false;
	}
	if (inflateEnd(&ZLibStream) < 0)
	{
		return false;
	}

	return true;
}
