#include <cstring>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

#define PROFILE_INFO_AS_NAMESPACE
#include <actor/MapActor.h>
#include <actor/Profile.h>
#include <system/ResMgr.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

#include <red/profile/MapActorMgr.h>
#include <red/profile/ProfileEx.h>
#include <red/util/Log.h>

namespace {
    template <typename T>
    T read(const u8*& ptr) {
        T val = *(T*)ptr;
        ptr += sizeof(T);
        return val;
    }
}

namespace red {

SEAD_SINGLETON_DISPOSER_IMPL(MapActorMgr);

MapActorMgr::MapActorMgr()
    : mProfileID()
{
}

MapActorMgr::~MapActorMgr()
{
    red::print("Unloading spritemap...\n");
    mProfileID.freeBuffer();
}

void MapActorMgr::init(sead::Heap* heap)
{
    red::print("Loading spritemap...\n");

    sead::CurrentHeapSetter chs(heap);

    const u8* file = static_cast<u8*>(ResMgr::instance()->getFileFromCourseArchiveRes("course/spritemap.bin"));
    if (file == nullptr)
    {
        red::print("Level has no spritemap.bin, skipping...\n");
        return;
    }

    const u8* data = static_cast<const u8*>(file);

    u32 spritemapVersion = read<u32>(data);
    if (spritemapVersion != red::cSpritemapVersion)
    {
        red::print("ERROR: spritemap.bin version mismatch. Encountered 0x%08X, expected 0x%08X\n", spritemapVersion, red::cSpritemapVersion);
        return;
    }

    u32 entryNum = read<u32>(data);
    if (entryNum == 0)
    {
        red::print("Spritemap was empty, skipping...\n");
        return;
    }

    // red::print("entryNum: %u\n", entryNum);

    mProfileID.allocBuffer(entryNum);
    for (u32 i = 0; i < entryNum; i++)
    {
        u32 strOffset = read<u32>(data);
        // red::print("Entry %u str offset: 0x%08X\n", i, strOffset);

        const char* id = reinterpret_cast<const char*>(file + strOffset);

        Profile* prof = ProfileEx::get(id);
        if (!prof)
        {
            red::print("ERROR: Failed to find profile for spritemap entry %u with identifier \"%s\"\n", i, id);
            mProfileID[i] = -1;
            continue;
        }

        mProfileID[i] = prof->getID();
        red::print("Loaded spritemap entry [%u]->[%s]\n", i, id);
    }
}

s32 MapActorMgr::mapToProf(s16 mapActor)
{
    if (mapActor < cMapActorNum)
    {
        return MapActor::cProfileID[mapActor];
    }

    if (mapActor == 1000) // TODO: REMOVE TEMP CUZ I DIDNT EDIT LEVELS
    {
        mapActor = 0x1000;
    }

    if ((mapActor & 0xF000) != 0x1000)
    {
        red::print("ERROR: Requested mapActor(0x%04X) is not a valid name map actor\n", int(mapActor));
        return -1;
    }

    mapActor &= 0x0FFF; //? Remove metadata so it can used as a index

    if (!mProfileID.isBufferReady())
    {
        red::print("ERROR: Requesting mapActor(%i) without spritemap present\n", int(mapActor));
        return -1;
    }

    if (mapActor >= mProfileID.size())
    {
        red::print("ERROR: Requested mapActor(%i) is out of bounds(%i)\n", int(mapActor), mProfileID.size());
        return -1;
    }

    return mProfileID[mapActor];
}

// s32* MapActorMgr::mapToProf(s16 mapActor)
// {
//     if (mapActor < cMapActorNum)
//     {
//         return &MapActor::cProfileID[mapActor];
//     }

//     if (mapActor == 1000)
//     {
//         mapActor = 0x1000;
//     }

//     if ((mapActor & 0xF000) != 0x1000)
//     {
//         red::print("ERROR: Requested mapActor(0x%04X) is not a valid name map actor\n", int(mapActor));
//         return -1;
//     }

//     mapActor &= 0x0FFF; //? Remove metadata so it can used as a index

//     if (!mProfileID.isBufferReady())
//     {
//         red::print("ERROR: Requesting mapActor(%i) without spritemap present\n", int(mapActor));
//         return -1;
//     }

//     if (mapActor >= mProfileID.size())
//     {
//         red::print("ERROR: Requested mapActor(%i) is out of bounds(%i)\n", int(mapActor), mProfileID.size());
//         return -1;
//     }

//     return mProfileID[mapActor];
// }

}

extern "C" void red_CreateMapActorMgr()
{
    red::MapActorMgr::createInstance(nullptr); //? Allocated on a CourseTask sub-heap, so it is freed when CourseTask dies
    red::MapActorMgr::instance()->init(nullptr);
}

extern "C" void red_CreateMapActorMgrHook() tAssembly
(
    tSaveVolatileRegisters;
    bl red_CreateMapActorMgr;
    tRestoreVolatileRegisters;

    li r22, 0x0; // replaced instruction
    blr;
)
tBranchEx(0x024BDF5C, "red_CreateMapActorMgrHook", tk::BranchType::bl); // CourseTask::prepare

// Increase all instances of the cMapActorNum MapActor::cProfileID limit to 0xFFFF
// TODO: Only keep the ones that are needed, else it may crash trying to use out of bounds sprite id to perform array lookup
// ActorCreateMgr::getNumCoinSpritesInLocation
tPatch16(0x0200457A, 0xFFFF);
// ActorCreateMgr::spawnSprites
tPatch16(0x0200501A, 0xFFFF);
// ActorCreateMgr::update
tPatch16(0x02007C62, 0xFFFF);
// ActorCreateMgr::FUN_2007FC0
tPatch16(0x0200806E, 0xFFFF);
// ActorCreateMgr::FUN_20081B8
tPatch16(0x02008262, 0xFFFF);
// ActorCreateMgr::FUN_20083A0
tPatch16(0x0200844E, 0xFFFF);

tPatch32(0x02004950, 0x38600000); // TODO: REMOVE TEMP (li r3, 0)

extern "C" s32 red_MapToProf(s16 mapActor)
{
    return red::MapActorMgr::instance()->mapToProf(mapActor);
}

// extern "C" s32* red_MapToProf(s16 mapActor)
// {
//     return red::MapActorMgr::instance()->mapToProf(mapActor);
// }

extern "C" s32 red_MapToProfGeneric() tAssembly
(
    tSaveVolatileRegisters;
    bl red_MapToProf;
    mr r2, r3;
    tRestoreVolatileRegisters;

    mr r3, r2;
    blr;
)

extern "C" s32 red_MapToProfR9R3Hook() tAssembly
(
    mr r3, r9;
    b red_MapToProfGeneric;
)
tBranchEx(0x02004584, "red_MapToProfR9R3Hook", tk::BranchType::bl);

tBranchEx(0x020045B0, "red_MapToProfR9R3Hook", tk::BranchType::bl);
tPatch32(0x020045B4, 0x7C661B78); // mr r6, r3

extern "C" s32 red_MapToProfR6R3Hook() tAssembly
(
    mr r3, r6;
    b red_MapToProfGeneric;
)

extern "C" s32 red_MapToProfR6R3Hookerr() tAssembly
(
    mr r3, r6;
    b red_MapToProfGeneric;
)
tBranchEx(0x02004948, "red_MapToProfR6R3Hookerr", tk::BranchType::bl);

tBranchEx(0x02004DA8, "red_MapToProfR6R3Hook", tk::BranchType::bl);

tBranchEx(0x02005024, "red_MapToProfR6R3Hook", tk::BranchType::bl);

extern "C" s32 red_MapToProfR0R3Hook() tAssembly
(
    mr r3, r0;
    b red_MapToProfGeneric;
)
tBranchEx(0x02007C6C, "red_MapToProfR0R3Hook", tk::BranchType::bl);

tBranchEx(0x02008078, "red_MapToProfR0R3Hook", tk::BranchType::bl);

tBranchEx(0x020080A4, "red_MapToProfR0R3Hook", tk::BranchType::bl);
tPatch32(0x020080A8, 0x7C661B78); // mr r6, r3

tBranchEx(0x0200826C, "red_MapToProfR0R3Hook", tk::BranchType::bl);

extern "C" s32 red_MapToProfR8R3Hook() tAssembly
(
    mr r3, r8;
    b red_MapToProfGeneric;
)
tBranchEx(0x0200828C, "red_MapToProfR8R3Hook", tk::BranchType::bl);
tPatch32(0x02008290, 0x7C601B78); // mr r0, r3

tBranchEx(0x02008458, "red_MapToProfR0R3Hook", tk::BranchType::bl);

tBranchEx(0x0200869C, "red_MapToProfR8R3Hook", tk::BranchType::bl);
tPatch32(0x020086A0, 0x7C691B78); // mr r9, r3

tBranchEx(0x0200A82C, "red_MapToProfR0R3Hook", tk::BranchType::bl);
tPatch32(0x0200A830, 0x7C7D1B78); // mr r29, r3
