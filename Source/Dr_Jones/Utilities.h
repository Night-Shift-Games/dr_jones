#pragma once

namespace Utilities
{
	uint32 WrapIndexToSize(int64 Index, uint32 Size);

	template <typename T>
	uint32 WrapIndexToArray(int64 Index, TArray<T> Array)
	{
		return WrapIndexToSize(Index, Array.Num());
	}

	template <typename T>
	uint32 WrapIndexToArray(int64 Index, TSet<T> Array)
	{
		return WrapIndexToSize(Index, Array.Num());
	}
}	
