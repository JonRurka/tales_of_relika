set(module "LuaCPP")

include_directories(Lua/include)

set(${module}_headers
	Lua.hpp
	LuaCpp.hpp
	LuaVersion.hpp
	Engine/LuaState.hpp
	Engine/LuaType.hpp
	Engine/LuaTNil.hpp
	Engine/LuaTString.hpp
	Engine/LuaTNumber.hpp
	Engine/LuaTBoolean.hpp
	Engine/LuaTTable.hpp
	Engine/LuaTUserData.hpp
	Registry/LuaRegistry.hpp
	Registry/LuaCodeSnippet.hpp
	Registry/LuaCompiler.hpp
	Registry/LuaCFunction.hpp
	Registry/LuaLibrary.hpp
	LuaContext.hpp
    LuaMetaObject.hpp
)

set(${module}_sources
	LuaVersion.cpp 				
	Engine/LuaState.cpp 		
	Engine/LuaType.cpp 			
	Engine/LuaTNil.cpp 			
	Engine/LuaTString.cpp 		
	Engine/LuaTNumber.cpp 		
	Engine/LuaTBoolean.cpp 		
	Engine/LuaTTable.cpp 		
	Engine/LuaTUserData.cpp 	
	Registry/LuaRegistry.cpp 	
	Registry/LuaCodeSnippet.cpp 
	Registry/LuaCompiler.cpp 	
	Registry/LuaCFunction.cpp 	
	Registry/LuaLibrary.cpp 	
	LuaContext.cpp 				
	LuaMetaObject.cpp 		
)

add_library(${module} STATIC ${${module}_headers} ${${module}_sources})
target_link_libraries(${module} lua::lua)




