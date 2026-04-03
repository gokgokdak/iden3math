#pragma once

#if defined(IDEN3MATH_BUILD_PY) || defined(IDEN3MATH_BUILD_TEST)
    #define API
#elif defined(_WIN32) || defined(__CYGWIN__)
    #ifdef IDEN3MATH_BUILD_DYNAMIC
        #define API __declspec(dllexport)
    #else
        #define API __declspec(dllimport)
    #endif
#else
    #define API __attribute__((visibility("default")))
#endif
