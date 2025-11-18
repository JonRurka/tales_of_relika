cmake -B build -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug -DSERVER_BUILD -DNO_UI -G "Ninja"
read -p "Press any key to resume ..."