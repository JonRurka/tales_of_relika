set(module "UI_debugger")

add_library(${module}
	BeaconSource.h
	CommonSource.h
	Debugger.cpp
	DebuggerPlugin.cpp
	DebuggerPlugin.h
	DebuggerSystemInterface.cpp
	DebuggerSystemInterface.h
	ElementContextHook.cpp
	ElementContextHook.h
	ElementDebugDocument.cpp
	ElementDebugDocument.h
	ElementInfo.cpp
	ElementInfo.h
	ElementLog.cpp
	ElementLog.h
	FontSource.h
	Geometry.cpp
	Geometry.h
	InfoSource.h
	LogSource.h
	MenuSource.h
)



# Add public headers as files in the project (it's not necessary but convenient for IDE integration)
# Setting them as PRIVATE so that it's addition doesn't propagate, it won't affect availability since
# the entire include directory has already been declared as public
target_sources(${module} PRIVATE
	"../../include/RmlUi/Debugger/Debugger.h"
	"../../include/RmlUi/Debugger/Header.h"
	"../../include/RmlUi/Debugger.h"
)

#target_link_libraries(${module} PUBLIC UI)
target_link_libraries(${module} PUBLIC rmlui)

