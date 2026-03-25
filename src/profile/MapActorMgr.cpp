#include <cstring>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

#define PROFILE_INFO_AS_NAMESPACE
#include <actor/MapActor.h>
#include <actor/Profile.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <system/ResMgr.h>

#include <red/profile/MapActorMgr.h>
#include <red/profile/ProfileEx.h>
#include <red/util/RawRead.h>

namespace red {

SEAD_SINGLETON_DISPOSER_IMPL(MapActorMgr);

MapActorMgr::MapActorMgr()
    : mProfileID()
{ }

MapActorMgr::~MapActorMgr() {
    mProfileID.freeBuffer();
}

void MapActorMgr::init(sead::Heap* heap) {
    sead::CurrentHeapSetter chs(heap);

    const u8* file = static_cast<u8*>(ResMgr::instance()->getFileFromCourseArchiveRes("course/spritemap.bin"));
    if (file == nullptr) {
        //tk::print("Level has no spritemap.bin, skipping...\n");
        return;
    }

    const u8* data = static_cast<const u8*>(file);

    u32 spritemapVersion = rawRead<u32>(data);
    if (spritemapVersion != cSpritemapVersion) [[unlikely]] {
        tk::print("ERROR: spritemap.bin version mismatch. Encountered 0x%08X, expected 0x%08X\n", spritemapVersion, cSpritemapVersion);
        return;
    }

    u32 entryNum = rawRead<u32>(data);
    if (entryNum == 0) {
        //tk::print("Spritemap was empty, skipping...\n");
        return;
    }

    // red::print("entryNum: %u\n", entryNum);

    mProfileID.allocBuffer(static_cast<s32>(entryNum));
    for (u32 i = 0; i < entryNum; i++) {
        u32 strOffset = rawRead<u32>(data);
        // red::print("Entry %u str offset: 0x%08X\n", i, strOffset);

        const char* id = reinterpret_cast<const char*>(file + strOffset);

        Profile* prof = ProfileEx::get(id);
        if (!prof) [[unlikely]] {
            tk::print("ERROR: Failed to find profile for spritemap entry %u with identifier \"%s\"\n", i, id);
            mProfileID[i] = -1;
            continue;
        }

        mProfileID[i] = prof->getID();
        //tk::print("Loaded spritemap entry [%u]->[%s]\n", i, id);
    }
}

s32 MapActorMgr::mapToProf(u16 mapActor) {
    if (mapActor == 0xFFFF) [[unlikely]] {
        tk::print("ERROR: Requested mapActor -1\n");
        return -1;
    }

    if (mapActor < cMapActorNum)
        return MapActor::cProfileID[mapActor];

    if (!(mapActor & cMapMetaMask)) [[unlikely]] {
        tk::print("ERROR: Requested mapActor(0x%04X) is not a valid name map actor\n", int(mapActor));
        return -1;
    }

    mapActor &= ~cMapMetaMask; //? Remove metadata so it can used as a index

    if (!mProfileID.isBufferReady()) [[unlikely]] {
        tk::print("ERROR: Requesting mapActor(%i) without spritemap present\n", int(mapActor));
        return -1;
    }

    if (mapActor >= mProfileID.size()) [[unlikely]] {
        tk::print("ERROR: Requested mapActor(%i) is out of bounds(%i)\n", int(mapActor), mProfileID.size());
        return -1;
    }

    return mProfileID[mapActor];
}

}

extern "C" void red_CreateMapActorMgr() {
    red::MapActorMgr::createInstance(nullptr); //? Allocated on a CourseTask sub-heap, so it is freed when CourseTask dies
    red::MapActorMgr::instance()->init(nullptr);
}

namespace red {
    void createMapActorMgrHook() tAssembly(
        tSaveVolatileRegisters;
        bl red_CreateMapActorMgr;
        tRestoreVolatileRegisters;
        li r22, 0x0; // replaced instruction
        blr;
    )
}

tBranch(0x024BDF5C, red::createMapActorMgrHook, tk::BranchType::bl); // CourseTask::prepare

// Increase all instances of the cMapActorNum MapActor::cProfileID limit to 0xFFFF
// TODO: Only keep the ones that are needed, else it may crash trying to use out of bounds sprite id to perform array lookup
tPatch16u(0x0200457A, 0xFFFF); // ActorCreateMgr::getNumCoinSpritesInLocation
tPatch16u(0x0200501A, 0xFFFF); // ActorCreateMgr::spawnSprites
tPatch16u(0x02007C62, 0xFFFF); // ActorCreateMgr::update
tPatch16u(0x0200806E, 0xFFFF); // ActorCreateMgr::FUN_2007FC0
tPatch16u(0x02008262, 0xFFFF); // ActorCreateMgr::FUN_20081B8
tPatch16u(0x0200844E, 0xFFFF); // ActorCreateMgr::FUN_20083A0

namespace red {
    s32 mapToProfR3(u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
    
    s32 mapToProfR6(int, int, int, u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
    
    s32 mapToProfR8(int, int, int, int, int, u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
    
    s32 mapToProfR9(int, int, int, int, int, int, u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
}

tBranch(0x02004584, red::mapToProfR9, tk::BranchType::bl);
tBranch(0x020045B0, red::mapToProfR9, tk::BranchType::bl);
tPatch32u(0x020045B4, 0x7C661B78); // mr r6, r3

tBranch(0x02004948, red::mapToProfR6, tk::BranchType::bl);
tPatchNop(0x200C990); // we're not passing a pointer so skip the deref

tBranch(0x02004DA8, red::mapToProfR6, tk::BranchType::bl);
tBranch(0x02005024, red::mapToProfR6, tk::BranchType::bl);

tBranch(0x0200828C, red::mapToProfR8, tk::BranchType::bl);
tPatch32u(0x02008290, 0x7C601B78); // mr r0, r3

tBranch(0x0200869C, red::mapToProfR8, tk::BranchType::bl);
tPatch32u(0x020086A0, 0x7C691B78); // mr r9, r3

extern "C" s32 red_MapToProfR0R3Hook() tAssembly(
    mr r3, r0;
    b _ZN3red11mapToProfR3Et;
)

tBranch(0x02007C6C, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x02008078, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x020080A4, red_MapToProfR0R3Hook, tk::BranchType::bl);
tPatch32u(0x020080A8, 0x7C661B78); // mr r6, r3

tBranch(0x0200826C, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x02008458, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x0200A82C, red_MapToProfR0R3Hook, tk::BranchType::bl);
tPatch32u(0x0200A830, 0x7C7D1B78); // mr r29, r3
