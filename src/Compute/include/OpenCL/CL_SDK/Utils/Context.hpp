#pragma once

// OpenCL SDK includes
#include "OpenCLUtilsCpp_Export.h"

#include "Error.hpp"

// OpenCL includes
#include "../opencl.hpp"

namespace cl {
namespace util {
    Context UTILSCPP_EXPORT get_context(cl_uint plat_id, cl_uint dev_id,
                                        cl_device_type type,
                                        cl_int* error = nullptr);
}
}
