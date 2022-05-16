cmake_minimum_required(VERSION 3.10)

# set the project name
project(FastOrFurious)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SFML_INCLUDE_DIR "./elo/SFML/include/")
set(SFML_LIBRARY_DIR "./SFML/build/lib")
link_directories(SFML_LIBRARY_DIR)
include_directories(SFML_INCLUDE_DIR)
# include_directories(SFML)
# include_directories(${SFML_INCLUDE_DIRS})
find_package(SFML 2.4 COMPONENTS system window graphics network audio)

# add the executable
add_executable(runClient run_client.cpp)
add_executable(FastOrFurious main.cpp)

target_link_libraries (runClient sfml-graphics sfml-window sfml-system sfml-network)
target_link_libraries (FastOrFurious sfml-graphics sfml-window sfml-system sfml-network)
# target_link_libraries(runClient sfml-graphics)
# target_link_libraries(runClient sfml-window)
# target_link_libraries(runClient sfml-system)
# target_link_libraries(runClient sfml-network)
# target_link_libraries(FastOrFurious sfml-graphics)
# target_link_libraries(FastOrFurious sfml-window)
# target_link_libraries(FastOrFurious sfml-system)
# target_link_libraries(FastOrFurious sfml-network)
