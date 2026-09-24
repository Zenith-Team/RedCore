#pragma once

#include <types.h>

namespace red {
    
    template <typename T>
    inline T rawRead(const u8*& ptr) {
        T val = *(T*)ptr;
        ptr += sizeof(T);
        return val;
    }
    
}
