
#add_subdirectory(third_party)
add_executable(cb50_ctrl src/main.cpp src/PIRegulator.cpp src/Parameters.cpp src/TrendData.cpp src/Log.cpp)

target_compile_features(cb50_ctrl PRIVATE cxx_range_for)
include_directories(../third_party/crow/include/)
include_directories(../third_party/json/src/)
target_link_libraries(cb50_ctrl boost_system)
target_link_libraries(cb50_ctrl pthread)
message("Copying parameter file.")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cb50_ctrl_params.json 
               ${CMAKE_CURRENT_BINARY_DIR} 
               COPYONLY) 
