#pragma once

/*
 * ============================================
 * C++ headers
 * ============================================
 * */
#include <string>

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>

#endif

/*
 * ============================================
 * Debug logging
 * ============================================
 * */
#ifdef _DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

void __cdecl dbgprintf(const char *format, ...);

#if DEBUG
#define DebugPrint dbgprintf
#else
#define DebugPrint(...) while(0)
#endif

/*
 * ============================================
 * Json config
 * ============================================
 * */
#include <json.hpp>

using json = nlohmann::json;

/*
 * ============================================
 * Websocketapp config
 * ============================================
 * */
#define ASIO_STANDALONE