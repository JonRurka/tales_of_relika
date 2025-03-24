#pragma once

#include "stdafx.h"
#include "ForwardDeclarations.h"

namespace VoxelEngine {

	class MarchingCubesArrays {
    public:

		static const int edgeTable[256];

        static const int triTable[4096];

        static const glm::ivec4 directionOffsets[8];
	};

}