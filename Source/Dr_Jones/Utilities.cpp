#include "Utilities.h"

uint32 Utilities::WrapIndexToSize(int64 Index, const uint32 Size)
{
	if (Size == 0)
	{
		return 0;
	}
	if (Index < 0)
	{
		return Size - 1;
	}
	if (Index >= Size)
	{
		return 0;
	}
	return Index;
}
