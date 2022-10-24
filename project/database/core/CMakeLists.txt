add_library(core src/ColumnUtils.cpp include/ColumnUtils.h
                                     include/VirtualDatabase.h
                                     include/VirtualTable.h
                 src/CustomTable.cpp include/CustomTable.h)

target_include_directories(core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
