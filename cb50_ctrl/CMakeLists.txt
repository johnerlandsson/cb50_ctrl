cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

#add_subdirectory(third_party)
set(SRCS src/main.cpp
	src/PIRegulator.cpp
	src/TrendData.cpp
	src/Database.cpp
    src/ProcessData.cpp
    src/Recipe.cpp
    src/ProcessValue.cpp)

add_executable(cb50_ctrl ${SRCS})

target_compile_features(cb50_ctrl PRIVATE cxx_lambda_init_captures)
include_directories(../third_party/crow/include/)
include_directories(../third_party/SQLiteCpp/include)
target_link_libraries(cb50_ctrl boost_system)
target_link_libraries(cb50_ctrl pthread)
target_link_libraries(cb50_ctrl sqlite3)
target_link_libraries(cb50_ctrl ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SQLiteCpp/libSQLiteCpp.a)
