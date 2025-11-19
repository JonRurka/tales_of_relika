cmake -B build -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug -DSERVER_BUILD=1 -DNO_UI=1 -G "Ninja"
pause