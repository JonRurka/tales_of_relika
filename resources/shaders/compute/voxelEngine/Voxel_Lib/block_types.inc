#ifndef _BLOCK_TYPES_INC_
#define _BLOCK_TYPES_INC_

#ifdef RUNTIME_INSERTED_TYPES

// %block_types% replaced by type fuctions:
// example: #define grass() (1)
%block_types%

#else

int grass() {return 0;}
int dirt() {return 0;}
int stone() {return 0;}

#endif

#endif // _BLOCK_TYPES_INC_