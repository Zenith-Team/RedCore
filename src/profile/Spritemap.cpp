#include <cstring>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>
#define PROFILE_INFO_AS_NAMESPACE
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <system/ResMgr.h>
#include <actor/Profile.h>
#include <actor/MapActor.h>
#include <red/profile/ProfileEx.h>
#include <red/profile/Spritemap.h>
#include <telkin/Telkin.h>
#include <red/util/Log.h>

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

extern "C" void red_LoadSpritemap(sead::Heap* heap) {
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
        // do nothing
        red::print("Level has no spritemap.bin, skipping...\n");
        return;
    }
    
    const char* data = static_cast<const char*>(file);
    
    u32 spritemapVersion = read<u32>(data);
    if (spritemapVersion != red::cSpritemapVersion) {
        red::print("ERROR: spritemap.bin version mismatch. Encountered 0x%08X, expected 0x%08X\n", spritemapVersion, red::cSpritemapVersion);
        return;
    }
    
    red::sSpritemapCount = read<u32>(data);
    if (red::sSpritemapCount == 0) {
        red::print("Spritemap was empty, skipping...\n");
        return;
    }
    
    red::sSpritemapEntries = new red::SpritemapEntry[red::sSpritemapCount];
    for (s32 i = 0; i < red::sSpritemapCount; i++) {
        red::SpritemapEntry& entry = red::sSpritemapEntries[i];
        
        entry.key = read<s32>(data);
        entry.value = data;
        
        data += std::strlen(entry.value) + 1;
        
        red::print("Loaded spritemap entry [%i]->[%s]\n", entry.key, entry.value);
    }
}

sead::SafeString ResolveLocalSprite(s16 spriteID) {
    if (red::sSpritemapEntries == nullptr) {
        red::print("ERROR: Requesting OOB sprite ID %i without spritemap present\n", int(spriteID));
        return "NULL";
    }
    
    for (s32 i = 0; i < red::sSpritemapCount; i++) {
        red::SpritemapEntry& entry = red::sSpritemapEntries[i];
        
        if (entry.key == spriteID) {
            //red::print("Mapped requested ID %i to identifier %s\n", spriteID, entry.value);
            
            return entry.value;
        }
    }
    
    red::print("ERROR: Failed to find sprite: %i\n", int(spriteID));
    return "NULL";
}

extern "C" Profile* red_SpriteToProfileSmart(s16 spriteID) {
    if (spriteID < 0x2D4) { // TODO: dehardcode this
        return Profile::get(MapActor::cProfileID[spriteID]);
    }

    sead::SafeString identifier = ResolveLocalSprite(spriteID);
    return red::ProfileEx::get(identifier);
}

extern "C" s32 red_SpriteToProfileIDSmart(s16 spriteID) {
    return red_SpriteToProfileSmart(spriteID)->getID();
}

extern "C" Profile* red_SpriteProfileR0Hook() tAssembly(
    mr r3, r0;
    b red_SpriteToProfileSmart;
)

extern "C" Profile* red_SpriteProfileR6Hook() tAssembly(
    mr r3, r6;
    b red_SpriteToProfileSmart;
)

extern "C" Profile* red_SpriteProfileR9Hook() tAssembly(
    mr r3, r9;
    b red_SpriteToProfileSmart;
)

extern "C" Profile* red_SpriteProfileIDR0Hook() tAssembly(
    mr r3, r0;
    b red_SpriteToProfileIDSmart;
)

// ActorCreateMgr::spawnSpriteActor
tHook(0x2004dbc, "red_SpriteProfileR6Hook", tk::BranchType::bl);
// ActorCreateMgr::spawnSprites
tHook(0x2005028, "red_SpriteProfileR6Hook", tk::BranchType::bl);
// ActorCreateMgr::update
tHook(0x2007c70, "red_SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::??
tHook(0x200845C, "red_SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::??
tHook(0x2008270, "red_SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::??
tHook(0x200807C, "red_SpriteProfileR0Hook", tk::BranchType::bl);
// ActorCreateMgr::getNumCoinSpritesInLocation
tHook(0x2004588, "red_SpriteProfileR9Hook", tk::BranchType::bl);
// ActorResLoader::load
tHook(0x200a82c, "red_SpriteProfileIDR0Hook", tk::BranchType::bl);
tPatch32(0x200a830, 0x7C7D1B78); // mr r29, r3
tPatchNop(0x200a834);

// CourseCacheMgr::load
tHook(0x29cb3f8, "red_LoadSpritemapCacheHook", tk::BranchType::bl);
extern "C" void red_LoadSpritemapCacheHook() tAssembly(
    tSaveVolatileRegisters;
    lwz r3, 0x20(r29);
    bl red_LoadSpritemap;
    tRestoreVolatileRegisters;
    li r23, 0x0; // replaced instruction
    blr;
)

// CourseTask::prepare
tHook(0x24bdf50, "red_LoadSpritemapPrepareHook", tk::BranchType::bl);
extern "C" void red_LoadSpritemapPrepareHook() tAssembly(
    tSaveVolatileRegisters;
    li r3, 0x0;
    bl red_LoadSpritemap;
    tRestoreVolatileRegisters;
    b _ZN10CourseData14loadCourseDataEv; // replaced call
)

// Increase all instances of the 0x2D4 spriteToProfileTable limit to 0xFFFF
// TODO: Only keep the ones that are needed, else it may crash trying to use out of bounds sprite id to perform array lookup
// ActorCreateMgr::spawnSprites
tPatch16(0x200501A, 0xFFFF);
// ActorCreateMgr::update
tPatch16(0x2007C62, 0xFFFF);
// ActorCreateMgr::FUN_2007FC0
tPatch16(0x200806E, 0xFFFF);
// ActorCreateMgr::FUN_20081B8
tPatch16(0x2008262, 0xFFFF);
// ActorCreateMgr::FUN_20083A0
tPatch16(0x200844E, 0xFFFF);
// ActorCreateMgr::getNumCoinSpritesInLocation
tPatch16(0x200457A, 0xFFFF);
