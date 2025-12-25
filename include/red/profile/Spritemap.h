#pragma once

#include <types.h>

namespace red {

constexpr u32 cSpritemapVersion = 1;

struct SpritemapEntry {
    s32 key;
    const char* value;
};

extern SpritemapEntry* sSpritemapEntries;
extern u32 sSpritemapCount;

}
