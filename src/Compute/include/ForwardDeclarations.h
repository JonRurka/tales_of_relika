#pragma once

//#include "stdafx.h"

struct C_Vec2;
struct C_Vec3;
struct C_Vec4;
struct C_Quat;

namespace DynamicCompute {


	namespace Compute {
		
		struct Platform;
		struct Device;

		class IComputeBuffer_private;
		class IComputeProgram_private;
		class IComputeController_private;
		class ComputeInterface_private;

		class IComputeBuffer;
		class IComputeProgram;
		class IComputeController;
		class ComputeInterface;


	}
	
	


	namespace Util {
		typedef float Real;

		class Radian;
		class Degree;
		class Angle;
		class Math;

		class Ray;
		class Plane;
		class Sphere;
		class AxisAlignedBox;
	}
}
