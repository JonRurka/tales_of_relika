#ifndef _BITONIC_SORT_INC_
#define _BITONIC_SORT_INC_

#include "../../libUSL/USL.inc"

/*struct Entry{
	uint originalIndex;
	uint hash;
	uint key;
	uint __placeholder;
};*/

readwrite_buffer(uvec4, 0, in_exec_settings)
read_buffer(uvec4, 1, in_settings)
readwrite_buffer(fvec4, 2, Entries)

shared fvec4 shared_entries [1024]; // shared implies coherent


#define SETTINGS_numEntries(i) 	(get_buffer_vector(in_settings, (i * 1) + 0).x)
#define SETTINGS_groupWidth(i)	(get_buffer_vector(in_settings, (i * 1) + 0).y)
#define SETTINGS_groupHeight(i)	(get_buffer_vector(in_settings, (i * 1) + 0).z)
#define SETTINGS_stepIndex(i)	(get_buffer_vector(in_settings, (i * 1) + 0).w)

#define SETTINGS_instance()	(get_buffer_vector(in_exec_settings, 0))

struct Settings{
	uint numEntries;
	uint groupWidth;
	uint groupHeight;
	uint stepIndex;
};

//#define GET_SETTINGS() get_buffer_object(in_settings, 0)
STRUCT Settings GET_SETTINGS(uint run)
{
	//uvec4 s_vec = get_buffer_vector(in_settings, 0);
	Settings s;
	s.numEntries = SETTINGS_numEntries(run);
	s.groupWidth = SETTINGS_groupWidth(run);
	s.groupHeight = SETTINGS_groupHeight(run);
	s.stepIndex = SETTINGS_stepIndex(run);
	return s;
}

#define ENTRY_originalIndex(e) 	(e.x)
#define ENTRY_value(e) 			(e.y)

//read_buffer_struct(Settings, 0, in_settings)
//readwrite_buffer_struct(Entry, 1, Entries)


//readwrite_buffer_uint(2, Offsets)

// Sort the given entries by their keys (smallest to largest)
// This is done using bitonic merge sort, and takes multiple iterations
void Sort_elem(int run, uvec3 id)
{	
	uint i = id.x;
	
	//uvec4 instance_settings = SETTINGS_instance();
	//uint inst = instance_settings.x;
	
	STRUCT Settings s = GET_SETTINGS(run);
	
	uint hIndex = i & (s.groupWidth - 1);
	uint indexLeft = hIndex + (s.groupHeight + 1) * (i / s.groupWidth);
	uint rightStepSize = (s.stepIndex == 0) ? s.groupHeight - 2 * hIndex : (s.groupHeight + 1) / 2;
	uint indexRight = indexLeft + rightStepSize;
	
	// Exit if out of bounds (for non-power of 2 input sizes)
	if (indexRight >= s.numEntries) return;
	
	//float valueLeft = ENTRY_value(get_buffer_vector(Entries, indexLeft));
	//float valueRight = ENTRY_value(get_buffer_vector(Entries, indexRight));
	float valueLeft = ENTRY_value(shared_entries[indexLeft]);
	float valueRight = ENTRY_value(shared_entries[indexRight]);
	
	// Swap entries if value is descending
	if (valueLeft > valueRight)
	{
		fvec4 temp = shared_entries[indexLeft];
		shared_entries[indexLeft] = shared_entries[indexRight];
		shared_entries[indexRight] = temp;
		
		//fvec4 temp = get_buffer_vector(Entries, indexLeft);
		//set_buffer_vector(Entries, indexLeft, get_buffer_vector(Entries, indexRight));
		//set_buffer_vector(Entries, indexRight, temp);
	}
	
	//memoryBarrierShared();
	//barrier();
	
	
	
	//SETTINGS_instance(i) = 
	//if (i == 0){
		//instance_settings.x = instance_settings.x + 1;
		//set_buffer_vector(in_exec_settings, 0, instance_settings);
	//}
}

void Sort_main(uvec3 id)
{
	shared_entries[id.x] = get_buffer_vector(Entries, id.x);
	
	memoryBarrierShared();
	barrier();
	
	uvec4 instance_settings = SETTINGS_instance();
	uint num_runs = instance_settings.y;
	
	int did_runs;
	
	for (int i = 0; i < num_runs; i++){
		Sort_elem(i, id);
		
		memoryBarrierShared();
		barrier();
		
		did_runs++;
	}
	
	if (id.x == 0){
		instance_settings.x = did_runs;
		set_buffer_vector(in_exec_settings, 0, instance_settings);
	}
	
	
	set_buffer_vector(Entries, id.x, shared_entries[id.x]);
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
	/*STRUCT Settings s = GET_SETTINGS();
	
	if (id.x >= s.numEntries) return;
	
	uint i = id.x;
	uint null_ = s.numEntries;
	
	uint key = get_buffer_object(Entries, i).key;
	uint keyPrev = (i == 0) ? null_ : get_buffer_object(Entries, i - 1).key;
	
	if (key != keyPrev)
	{
		set_buffer_scalar(Offsets, key, i);
	}*/
	
}


#endif // _BITONIC_SORT_INC_