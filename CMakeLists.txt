cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(cb50_ctrl)

#add_subdirectory(third_party)

add_executable(cb50_ctrl src/main.cpp src/PIRegulator.cpp src/Parameters.cpp)
file(COPY third_party/Chart.js/src/ DESTINATION www/assets/js/chart/)

target_compile_features(cb50_ctrl PRIVATE cxx_range_for)
include_directories(third_party/crow/include/)
include_directories(third_party/json/src/)
target_link_libraries(cb50_ctrl boost_system)
target_link_libraries(cb50_ctrl pthread)
