cmake_minimum_required(VERSION 2.8)
project( tcc_gosurf )
find_package( OpenCV REQUIRED )

set(EXECUTABLE_OUTPUT_PATH "deploy")

include_directories ( "src/include" )

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable( main "src/main.cpp" ${SRC_FILES} )

target_link_libraries( main ${OpenCV_LIBS} )