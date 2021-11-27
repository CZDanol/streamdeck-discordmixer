cmake_minimum_required(VERSION 3.20)

project(streamdock-discordmixer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(streamdeck-discordmixer)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" lib_platform_dir)

# ===========================================
# Sources
# ===========================================

# ===========================================
# Stream Deck SDK
# ===========================================
file(GLOB_RECURSE stream_deck_sdk_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/deps/streamdeck_sdk/*.cpp")
target_sources(streamdeck-discordmixer PRIVATE ${stream_deck_sdk_files})

# ===========================================
# Discord SDK
# ===========================================
file(GLOB_RECURSE discord_sdk_files CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/deps/discord_game_sdk/*.cpp")
target_sources(streamdeck-discordmixer PRIVATE ${discord_sdk_files})

message("${PROJECT_SOURCE_DIR}/lib/${lib_platform_dir}")
find_library(discord_sdk_lib NAMES discord_game_sdk.dll.lib PATHS "${PROJECT_SOURCE_DIR}/lib/${lib_platform_dir}" REQUIRED)