find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

add_library(core src/ColumnUtils.cpp             include/ColumnUtils.h
                                                 include/VirtualDatabase.h
                                                 include/VirtualTable.h
                 src/CustomTable.cpp             include/CustomTable.h
                 src/Database.cpp                include/Database.h
                                                 include/AbstractTableFactory.h
                                                 include/AbstractCommand.h
                 src/DatabaseClient.cpp          include/DatabaseClient.h
                                                 include/VirtualValidators.h
                 src/Commands.cpp                include/Commands.h
                                                 include/AbstractSaveLoadStrategy.h
                 src/CustomSaveLoadStrategy.cpp  include/CustomSaveLoadStrategy.h
                 src/MongoDbSaveLoadStrategy.cpp include/MongoDbSaveLoadStrategy.h
                                                 include/SaveLoadUtils.h
                 src/StringUtils.cpp             include/StringUtils.h
                 src/SQLiteSaveLoadStrategy.cpp  include/SQLiteSaveLoadStrategy.h)

include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/libmongoc-1.0")
include_directories("/usr/local/include/libbson-1.0")
include_directories("/usr/local/lib")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../thirdparty/SQLiteCpp SQLiteCpp)

target_link_libraries(core PRIVATE ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(core PRIVATE ${LIBBSONCXX_LIBRARIES})
target_link_libraries(core PRIVATE SQLiteCpp)

target_include_directories(core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
