#pragma once
#include <stdbool.h>
#include <assert.h>

void _expects(const bool expr_result, const char *expr, const char *format, ...);
void _ensures(const bool expr_result, const char *expr, const char *format, ...);

#ifndef EXPECTS
    #ifndef DEBUG
        #define EXPECTS(expr, msg, ...) assert(expr); _expects( (expr), #expr, msg, __VA_ARGS__ )
    #endif
#endif

#ifndef ENSURES
    #ifndef DEBUG
        #define ENSURES(expr, msg, ...) assert(expr); _ensures( (expr), #expr, msg, __VA_ARGS__ )
    #endif
#endif
