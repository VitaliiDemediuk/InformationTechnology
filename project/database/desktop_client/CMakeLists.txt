set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/home/vitalii/Qt/6.3.0/gcc_64")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp             MainWindow.h             MainWindow.ui
        DesktopDatabaseClient.cpp  DesktopDatabaseClient.h
        Dialogs/NewDbDialog.cpp    Dialogs/NewDbDialog.h    Dialogs/NewDbDialog.ui
        Dialogs/NewTableDialog.cpp Dialogs/NewTableDialog.h Dialogs/NewTableDialog.ui
        Models/TableListModel.cpp  Models/TableListModel.h
                                   Models/ResetableModel.h
)

qt_add_executable(DatabaseDesktopClient
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(DatabaseDesktopClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets core)

target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../core/include)

target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Dialogs)
target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Models)

set_target_properties(DatabaseDesktopClient PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DatabaseDesktopClient)
endif()
