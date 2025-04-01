#ifndef _BITONIC_SORT_INC_
#define _BITONIC_SORT_INC_

#include "../../libUSL/USL.inc"

struct Entry{
	uint originalIndex;
	uint hash;
	uint key;
	uint __placeholder;
};

struct Settings{
	uint numEntries;
	uint groupWidth;
	uint groupHeight;
	uint stepIndex;
};

#define GET_SETTINGS() get_buffer_object(in_settings, 0)

read_buffer_struct(Settings, 0, in_settings)
readwrite_buffer_struct(Entry, 1, Entries)
readwrite_buffer_uint(2, Offsets)

// Sort the given entries by their keys (smallest to largest)
// This is done using bitonic merge sort, and takes multiple iterations
void Sort_main(uvec3 id)
{
	STRUCT Settings s = GET_SETTINGS();
	
	uint i = id.x;
	
	uint hIndex = i & (s.groupWidth - 1);
	uint indexLeft = hIndex + (s.groupHeight + 1) * (i / s.groupWidth);
	uint rightStepSize = (s.stepIndex == 0) ? s.groupHeight - 2 * hIndex : (s.groupHeight + 1) / 2;
	uint indexRight = indexLeft + rightStepSize;
	
	// Exit if out of bounds (for non-power of 2 input sizes)
	if (indexRight >= s.numEntries) return;
	
	uint valueLeft = get_buffer_object(Entries, indexLeft).key;
	uint valueRight = get_buffer_object(Entries, indexRight).key;
	
	// Swap entries if value is descending
	if (valueLeft > valueRight)
	{
		Entry temp = get_buffer_object(Entries, indexLeft);
		set_buffer_object(Entries, indexLeft, get_buffer_object(Entries, indexRight));
		set_buffer_object(Entries, indexRight, temp);
	}
}


// Calculate offsets into the sorted Entries buffer (used for spatial hashing).
// For example, given an Entries buffer sorted by key like so: {2, 2, 2, 3, 6, 6, 9, 9, 9, 9}
// The resulting Offsets calculated here should be:            {-, -, 0, 3, -, -, 4, -, -, 6}
// (where '-' represents elements that won't be read/written)
// 
// Usage example:
// Say we have a particular particle P, and we want to know which particles are in the same grid cell as it.
// First we would calculate the Key of P based on its position. Let's say in this example that Key = 9.
// Next we can look up Offsets[Key] to get: Offsets[9] = 6
// This tells us that SortedEntries[6] is the first particle that's in the same cell as P.
// We can then loop until we reach a particle with a different cell key in order to iterate over all the particles in the cell.
// 
// NOTE: offsets buffer must filled with values equal to (or greater than) its length to ensure that this works correctly
void CalculateOffsets_main(uvec3 id)
{
	STRUCT Settings s = GET_SETTINGS();
	
	if (id.x >= s.numEntries) return;
	
	uint i = id.x;
	uint null_ = s.numEntries;
	
	uint key = get_buffer_object(Entries, i).key;
	uint keyPrev = (i == 0) ? null_ : get_buffer_object(Entries, i - 1).key;
	
	if (key != keyPrev)
	{
		set_buffer_scalar(Offsets, key, i);
	}
	
}


#endif // _BITONIC_SORT_INC_