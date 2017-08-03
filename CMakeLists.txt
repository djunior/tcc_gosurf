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
add_executable( thresholdDebugger "src/debug/thresholdDebugger" ${SRC_FILES} ${FILTER_FILES} )
add_executable( aus "src/debug/aus.cpp" )
add_executable( simpleTimestack "src/simpleTimestack.cpp" ${SRC_FILES} ${TIMESTACK_FILES} ${FILTER_FILES} )
add_executable( waveInspector "src/waveInspector.cpp" ${SRC_FILES} ${TIMESTACK_FILES} ${FILTER_FILES} )
add_executable( generateReport "src/generateReport.cpp")

target_link_libraries( timestack ${OpenCV_LIBS} )
target_link_libraries( filterImage ${OpenCV_LIBS} )
target_link_libraries( waveViewer ${OpenCV_LIBS} )
target_link_libraries( thresholdDebugger ${OpenCV_LIBS} )
target_link_libraries( aus ${OpenCV_LIBS} )
target_link_libraries( simpleTimestack ${OpenCV_LIBS} )
target_link_libraries( waveInspector ${OpenCV_LIBS} )