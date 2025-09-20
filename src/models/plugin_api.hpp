/*
 * This file is readonly! And used only for your help.
 * You can use it like interface for implementing
 * functions which used in main application.
 *
 * Implement all functions that included in PluginAPI structure.
 */

#pragma once

#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #ifdef PLUGIN_API_BUILD
        #define PLUGIN_API_EXPORT __declspec(dllexport)
    #else
        #define PLUGIN_API_EXPORT
    #endif
    #define PLUGIN_API_CALL __cdecl
#else
    #define PLUGIN_API_EXPORT __attribute__((visibility("default")))
    #define PLUGIN_API_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

using GetVersion = const char* (PLUGIN_API_CALL *)() noexcept;
using Work = void (PLUGIN_API_CALL *)() noexcept;
using ValidateAddr = int (PLUGIN_API_CALL *)(const char *, unsigned short);

struct PLUGIN_API_EXPORT PluginAPI {
    GetVersion getVersion;
    Work work;
    ValidateAddr validateAddr;
};

PLUGIN_API_EXPORT PluginAPI* PLUGIN_API_CALL get_plugin_api();

#ifdef __cplusplus
}
#endif

#endif // PLUGIN_API_H