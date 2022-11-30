cmake_minimum_required(VERSION 3.20)

project(core_unit_tests LANGUAGES CXX)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)

add_executable(core_unit_tests main.cpp)
add_test(NAME CreateEmptyDatabase COMMAND CreateEmptyDatabase)
add_test(NAME RenameDatabase COMMAND RenameDatabase)
add_test(NAME CreateTable COMMAND CreateTable)
add_test(NAME DeleteTable COMMAND DeleteTable)
add_test(NAME CreateCartesianProduct COMMAND CreateCartesianProduct)

if (DEFINED ENV{BOOST_INCLUDE_DIR})
    set(BOOST_INCLUDE_DIR $ENV{BOOST_INCLUDE_DIR})
else ()
    set(BOOST_INCLUDE_DIR "")
endif ()
if (BOOST_INCLUDE_DIR STREQUAL "")
    message("BOOST_INCLUDE_DIR is not set, assuming Boost can be found automatically in your system")
elseif (EXISTS ${BOOST_INCLUDE_DIR})
    include_directories(${BOOST_INCLUDE_DIR})
endif ()

target_link_libraries(core_unit_tests core)

target_include_directories(core_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../core/include)
