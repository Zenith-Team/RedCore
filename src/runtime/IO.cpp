#include <stdarg.h>
#include <limits.h>
#include <cafe/os.h>

#define TK_IMPL_PRINTF
#include <telkin/Runtime.h>

extern "C" int sprintf(char* buffer, const char* format, ...) {
    __va_list va;
    va_start(va, format);
    const int ret = vsnprintf(buffer, INT_MAX, format, va);
    va_end(va);
    return ret;
}
