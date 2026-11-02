cmake_minimum_required(VERSION 3.19)
project(Gestorinventario LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Sql)

qt_standard_project_setup()

qt_add_executable(Gestorinventario
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    databasemanager.h
    databasemanager.cpp
    articulo.h
    articulo.cpp
)

qt_add_translations(
    TARGETS Gestorinventario
    TS_FILES Gestorinventario_es_CO.ts
)

target_link_libraries(Gestorinventario
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

install(TARGETS Gestorinventario
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Gestorinventario
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
