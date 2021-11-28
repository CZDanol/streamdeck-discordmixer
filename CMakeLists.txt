cmake_minimum_required(VERSION 3.20)

# Build tested on MSVC 2016 x64

project(streamdeck-discordmixer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===========================================
# Target setup
# ===========================================
set(target streamdeck-discordmixer)
set(bin_directory "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/cz.danol.discordmixer.sdPlugin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${bin_directory}/bin")

add_executable(${target})

if (MSVC)
    target_compile_options(${target} PRIVATE "/Zc:__cplusplus")
endif ()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" lib_platform_dir)

# ===========================================
# Sources
# ===========================================
file(GLOB_RECURSE source_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
target_sources(${target} PRIVATE ${source_files})
target_precompile_headers(${target} PRIVATE "${PROJECT_SOURCE_DIR}/src/pch.h")
target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/src")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/dist/" DESTINATION ${bin_directory})

# ===========================================
# Stream Deck SDK
# ===========================================
file(GLOB_RECURSE stream_deck_sdk_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/deps/streamdeck_sdk/Common/*.cpp")
if (NOT APPLE)
    list(FILTER stream_deck_sdk_files EXCLUDE REGEX "ESDUtilitiesMac.cpp")
endif ()
if (NOT WIN32)
    list(FILTER stream_deck_sdk_files EXCLUDE REGEX "ESDUtilitiesWindows.cpp")
endif ()
target_sources(${target} PRIVATE ${stream_deck_sdk_files})

target_include_directories(${target} PRIVATE SYSTEM
        "${PROJECT_SOURCE_DIR}/deps/streamdeck_sdk/Common"
        "${PROJECT_SOURCE_DIR}/deps/streamdeck_sdk/Vendor/json/src"
        "${PROJECT_SOURCE_DIR}/deps/streamdeck_sdk/Vendor/websocketpp"
        "${PROJECT_SOURCE_DIR}/deps/streamdeck_sdk/Vendor/asio/include"
        )

# ===========================================
# Discord SDK
# ===========================================
file(GLOB_RECURSE discord_sdk_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/deps/discord_game_sdk/*.cpp")
target_sources(${target} PRIVATE ${discord_sdk_files})
target_include_directories(${target} PRIVATE SYSTEM "${PROJECT_SOURCE_DIR}/deps/discord_game_sdk")

find_library(discord_sdk_lib NAMES discord_game_sdk.dll.lib PATHS "${PROJECT_SOURCE_DIR}/lib/${lib_platform_dir}" REQUIRED)
target_link_libraries(${target} PRIVATE "${discord_sdk_lib}")

find_file (discord_sdk_lib_so NAMES discord_game_sdk.dll PATHS "${PROJECT_SOURCE_DIR}/lib/${lib_platform_dir}" REQUIRED)
install(FILES ${discord_sdk_lib_so} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ===========================================
# Finally - install to appdata
# ===========================================
file(TO_CMAKE_PATH "$ENV{APPDATA}" appdata_path)
install(DIRECTORY ${bin_directory} DESTINATION "${appdata_path}/Elgato/StreamDeck/Plugins")