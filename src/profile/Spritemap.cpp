#define TELKIN_REGISTERS
#include <cstring>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <system/ResMgr.h>
#include <actor/Profile.h>
#include <actor/MapActor.h>
#include <red/profile/ProfileEx.h>
#include <red/profile/Spritemap.h>
#include <telkin/Telkin.h>

u32 red::sSpritemapCount = 0;
red::SpritemapEntry* red::sSpritemapEntries = nullptr;

namespace {
    template <typename T>
    T read(const char*& ptr) {
        T val = *(T*)ptr;
        ptr += sizeof(T);
        return val;
    }
}

extern "C" void LoadSpritemap(sead::Heap* heap) {
    sead::CurrentHeapSetter scopedHeap = heap;
    
    // cleanup old data
    if (red::sSpritemapEntries != nullptr) {
        delete[] red::sSpritemapEntries;
        red::sSpritemapEntries = nullptr;
        red::sSpritemapCount = 0;
    }
    
    void* file = ResMgr::instance()->getFileFromCourseArchiveRes("course/spritemap.bin");
    if (file == nullptr) {
        // level has no custom sprites
        // do nothing?
        OSReport("Level has no spritemap.bin, skipping...\n");
        return;
    }
    
    const char* data = static_cast<const char*>(file);
    
    u32 spritemapVersion = read<u32>(data);
    if (spritemapVersion != red::cSpritemapVersion) {
        OSReport("ERROR: spritemap.bin version mismatch. Encountered 0x%08X, expected 0x%08X\n", spritemapVersion, red::cSpritemapVersion);
        return;
    }
    
    red::sSpritemapCount = read<u32>(data);
    if (red::sSpritemapCount == 0) {
        OSReport("Spritemap was empty, skipping...\n");
        return;
    }
    
    red::sSpritemapEntries = new red::SpritemapEntry[red::sSpritemapCount];
    for (s32 i = 0; i < red::sSpritemapCount; i++) {
        red::SpritemapEntry& entry = red::sSpritemapEntries[i];
        
        entry.key = read<s32>(data);
        entry.value = data;
        
        data += std::strlen(entry.value) + 1;
        
        OSReport("Loaded spritemap entry [%i]->[%s]\n", entry.key, entry.value);
    }
}

sead::SafeString ResolveLocalSprite(s16 spriteID) {
    for (s32 i = 0; i < red::sSpritemapCount; i++) {
        red::SpritemapEntry& entry = red::sSpritemapEntries[i];
        
        if (entry.key == spriteID) {
            OSReport("Mapped requested ID %i to identifier %s\n", spriteID, entry.value);
            
            return entry.value;
        }
    }
    
    OSReport("ERROR: Failed to find sprite: %i\n", int(spriteID));
    return "NULL";
}

extern "C" Profile* SpriteToProfileSmart(s16 spriteID) {
    if (spriteID < ProfileInfo::cProfileID_Max) {
        return Profile::get(MapActor::cProfileID[spriteID]);
    }

    sead::SafeString identifier = ResolveLocalSprite(spriteID);
    return red::ProfileEx::get(identifier);
}

extern "C" Profile* SpriteProfileR0Hook() tAssembly(
    mr r3, r0;
    b SpriteToProfileSmart;
);

extern "C" Profile* SpriteProfileR6Hook() tAssembly(
    mr r3, r6;
    b SpriteToProfileSmart;
);

extern "C" Profile* SpriteProfileR9Hook() tAssembly(
    mr r3, r9;
    b SpriteToProfileSmart;
);

// ActorCreateMgr::spawnSpriteActor
tHook(0x2004dbc, "SpriteProfileR6Hook", tk::BranchType::bl);
// ActorCreateMgr::spawnSprites
tHook(0x2005028, "SpriteProfileR6Hook", tk::BranchType::bl);
// ActorCreateMgr::update
tHook(0x2007c70, "SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::??
tHook(0x200845C, "SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::??
tHook(0x2008270, "SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::??
tHook(0x200807C, "SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::getNumCoinSpritesInLocation
tHook(0x2004588, "SpriteProfileR9Hook", tk::BranchType::bl);

// CourseCacheMgr::load
tHook(0x29cb3f8, "LoadSpritemapCacheHook", tk::BranchType::bl);
extern "C" void LoadSpritemapCacheHook() tAssembly(
    tSaveVolatileRegisters;
    lwz r3, 0x20(r29);
    bl LoadSpritemap;
    tRestoreVolatileRegisters;
    li r23, 0x0; // replaced instruction
    blr;
);

// CourseTask::prepare
tHook(0x24bdf50, "LoadSpritemapPrepareHook", tk::BranchType::bl);
extern "C" void LoadSpritemapPrepareHook() tAssembly(
    tSaveVolatileRegisters;
    li r3, 0x0;
    bl LoadSpritemap;
    tRestoreVolatileRegisters;
    b _ZN10CourseData14loadCourseDataEv; // replaced call
);
