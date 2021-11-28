cmake_minimum_required(VERSION 3.20)

# Build tested on MSVC 2016 x64

project(streamdeck-discordmixer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_IN_STREAMDECK_PLUGINS_DIR ON)

# ===========================================
# Target setup
# ===========================================
set(target streamdeck-discordmixer)

# Qt setup
find_package(Qt6 COMPONENTS Core WebSockets REQUIRED CONFIG)
set(CMAKE_AUTOMOC ON)

if (BUILD_IN_STREAMDECK_PLUGINS_DIR)
    file(TO_CMAKE_PATH "$ENV{APPDATA}" appdata_path)
    set(target_directory "${appdata_path}/Elgato/StreamDeck/Plugins/cz.danol.discordmixer.sdPlugin")
else ()
    set(target_directory "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/cz.danol.discordmixer.sdPlugin")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${target_directory}/bin")
qt_add_executable(${target})
target_link_libraries(${target} PRIVATE Qt6::WebSockets)
target_compile_definitions(${target} PRIVATE QT_DISABLE_DEPRECATED_BEFORE=0x060200)

# ===========================================
# Sources
# ===========================================
file(GLOB_RECURSE streamdeckplugin_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/deps/streamdeck_qtpluginbase/ElgatoStreamDeck/*.cpp")
target_sources(${target} PRIVATE ${streamdeckplugin_files})
target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/deps/streamdeck_qtpluginbase")

# ===========================================
# StreamDeck plugin
# ===========================================
file(GLOB_RECURSE source_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
target_sources(${target} PRIVATE ${source_files})
target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/src")


# ===========================================
# Deployment
# ===========================================

# Copy files from dist
install(DIRECTORY "${PROJECT_SOURCE_DIR}/dist/" DESTINATION ${target_directory})

# Run windeployqt
get_target_property(QMAKE_FILEPATH Qt6::qmake IMPORTED_LOCATION)

install(CODE "set(target_file \"$<TARGET_FILE:${target}>\")")
install(CODE "set(QMAKE_FILEPATH \"${QMAKE_FILEPATH}\")")
install(CODE "set(PROJECT_SOURCE_DIR \"${PROJECT_BASE_DIR}\")")
install(CODE "set(CMAKE_BINARY_DIR \"${CMAKE_BASE_DIR}\")")
install(SCRIPT "${PROJECT_SOURCE_DIR}/deploy.cmake")