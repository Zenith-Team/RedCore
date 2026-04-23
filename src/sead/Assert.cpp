#include <basis/seadAssert.h>
#include <telkin/Print.h>
#include <cstdarg>
#include <cstdio>

void sead::system::HaltWithDetail(const char* pos, const s32 line, const char* format, ...) {
    char buffer[2048];

    int offset = __os_snprintf(buffer, sizeof(buffer), "[%s:%d] ", tk::internal::basename(pos), line);
    if (offset >= 0 && offset < static_cast<int>(sizeof(buffer))) {
        va_list args;
        va_start(args, format);
        std::vsnprintf(buffer + offset, sizeof(buffer) - offset, format, args);
        va_end(args);
    } else {
        OSReport("HaltWithDetail buffer overflow attempting to print prefix.\n");
    }

    OSReport("%s\n", buffer);
    OSFatal(buffer);
}

void sead::system::HaltWithDetailNoFormat(const char* pos, const s32 line, const char* str) {
    char buffer[2048];
    
    __os_snprintf(buffer, sizeof(buffer), "[%s:%d] %s", tk::internal::basename(pos), line, str);
    
    OSReport("%s\n", buffer);
    OSFatal(buffer);
}
