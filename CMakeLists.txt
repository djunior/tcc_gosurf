cmake_minimum_required(VERSION 2.8)
project( tcc_gosurf )
find_package( OpenCV REQUIRED )

set(EXECUTABLE_OUTPUT_PATH "deploy")

include_directories ( "src/include" )

file(GLOB_RECURSE SRC_FILES "src/lib/*.cpp")
file(GLOB_RECURSE FILTER_FILES "src/lib/filters/*.cpp")
file(GLOB_RECURSE TIMESTACK_FILES "src/lib/timestack/*.cpp")

add_executable( timestack "src/timestackFromVideo.cpp" ${SRC_FILES} ${TIMESTACK_FILES} )
add_executable( filterImage "src/filterImage.cpp" ${SRC_FILES} ${FILTER_FILES} )
add_executable( waveViewer "src/waveViewer.cpp" ${SRC_FILES} ${FILTER_FILES} )

target_link_libraries( timestack ${OpenCV_LIBS} )
target_link_libraries( filterImage ${OpenCV_LIBS} )
target_link_libraries( waveViewer ${OpenCV_LIBS} )