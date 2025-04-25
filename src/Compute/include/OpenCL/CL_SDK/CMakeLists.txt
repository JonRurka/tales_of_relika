set(module "Compute_OCL_SDK")

set(${module}_headers
	Utils/Context.h
	Utils/Context.hpp
	Utils/Detail.hpp
	Utils/Device.hpp
	Utils/Error.h
	Utils/Error.hpp
	Utils/ErrorCodes.h
	Utils/Event.h
	Utils/Event.hpp
	Utils/File.h
	Utils/File.hpp
	Utils/InteropContext.hpp
	Utils/OpenCLUtilsCpp_Export.h
	Utils/OpenCLUtils_Export.h
	Utils/Platform.hpp
	Utils/Utils.h
	Utils/Utils.hpp
	
	cl.h
	cl2.hpp
	cl_d3d10.h
	cl_d3d11.h
	cl_dx9_media_sharing.h
	cl_dx9_media_sharing_intel.h
	cl_egl.h
	cl_ext.h
	cl_ext_intel.h
	cl_gl.h
	cl_gl_ext.h
	cl_half.h
	cl_icd.h
	cl_layer.h
	cl_platform.h
	cl_va_api_media_sharing_intel.h
	cl_version.h
	opencl.h
	opencl.hpp
)

set(${module}_sources
	cl_main.cpp
)


add_library(${module} STATIC ${${module}_headers} ${${module}_sources})
target_link_libraries(Compute_OCL_SDK OpenCL-SDK OpenCLUtils OpenCLUtilsCpp)

