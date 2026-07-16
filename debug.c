#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vadefs.h>

#define MAX_MESSAGE_LENGTH 512

void _expects(const bool expr_result, const char *expr, const char *format, ...) {
    if(expr_result) {
        return;
    }

    char *msg_buf = (char*) calloc(MAX_MESSAGE_LENGTH, sizeof(char));

    va_list args;
    va_start(args, format);
    vsprintf_s(msg_buf, MAX_MESSAGE_LENGTH * sizeof(char), format, args);
    va_end(args);

    fprintf(stderr, "Expected precondition failed:\nExpression: %s\nMessage: %s", expr, msg_buf);
    exit(EXIT_FAILURE);
}

void _ensures(const bool expr_result, const char *expr, const char *format, ...) {
    if(expr_result) {
        return;
    }

    char *msg_buf = (char*) calloc(MAX_MESSAGE_LENGTH, sizeof(char));

    va_list args;
    va_start(args, format);
    vsprintf_s(msg_buf, MAX_MESSAGE_LENGTH * sizeof(char), format, args);
    va_end(args);

    fprintf(stderr, "Ensured postcondition failed:\nExpression: %s\nMessage: %s", expr, msg_buf);
    exit(EXIT_FAILURE);
}
