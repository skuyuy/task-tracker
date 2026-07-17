#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vadefs.h>

void _expects(const bool expr_result, const char *expr, const char *format, ...) {
    if(expr_result) {
        return;
    }

    va_list args;
    va_start(args, format);

    int buffer_size = vsnprintf(NULL, 0, format, args);
    char *buffer = calloc(buffer_size + 1, sizeof(char));
    vsprintf_s(buffer, buffer_size * sizeof(char), format, args);

    va_end(args);

    fprintf(stderr, "Expected precondition failed:\nExpression: %s\nMessage: %s", expr, buffer);
    exit(EXIT_FAILURE);
}

void _ensures(const bool expr_result, const char *expr, const char *format, ...) {
    if(expr_result) {
        return;
    }

    va_list args;
    va_start(args, format);

    int buffer_size = vsnprintf(NULL, 0, format, args);
    char *buffer = calloc(buffer_size + 1, sizeof(char));
    vsprintf_s(buffer, buffer_size * sizeof(char), format, args);

    va_end(args);

    fprintf(stderr, "Ensured postcondition failed:\nExpression: %s\nMessage: %s", expr, buffer);
    exit(EXIT_FAILURE);
}
