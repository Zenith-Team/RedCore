#include <cstring>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

#include <actor/MapActor.h>
#include <actor/Profile.h>
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

void MapActorMgr::init() {
    const u8* file = static_cast<u8*>(ResMgr::instance()->getFileFromCourseArchiveRes("course/spritemap.bin"));
    if (file == nullptr) [[unlikely]] {
        //tk::println("Level has no spritemap.bin, skipping...");
        return;
    }

    const u8* data = file;

    if (u32 spritemapVersion = rawRead<u32>(data); spritemapVersion != cSpritemapVersion) [[unlikely]] {
        tk::fatal("spritemap.bin version mismatch. Encountered 0x%08X, expected 0x%08X", spritemapVersion, cSpritemapVersion);
        return;
    }

    const u32 entryNum = rawRead<u32>(data);
    if (entryNum == 0) {
        //tk::println("Spritemap was empty, skipping...");
        return;
    }

    // tk::println("entryNum: %u", entryNum);

    mProfileID.allocBuffer(static_cast<s32>(entryNum));
    for (u32 i = 0; i < entryNum; i++) {
        const u32 strOffset = rawRead<u32>(data);
        // tk::println("Entry %u str offset: 0x%08X", i, strOffset);

        const char* id = reinterpret_cast<const char*>(file + strOffset);

        const Profile* prof = ProfileEx::get(id);
        if (!prof) [[unlikely]] {
            tk::fatal("Failed to find profile for spritemap entry %u with identifier \"%s\"", i, id);
            mProfileID[i] = -1;
            continue;
        }

        mProfileID[i] = prof->getID();
        //tk::println("Loaded spritemap entry [%u]->[%s]", i, id);
    }
}

s32 MapActorMgr::mapToProf(u16 mapActor) {
    if (mapActor == 0xFFFF) [[unlikely]] {
        tk::fatal("Requested mapActor -1");
        return -1;
    }

    if (mapActor < cMapActorNum)
        return MapActor::cProfileID[mapActor];

    if (!(mapActor & cMapMetaMask)) [[unlikely]] {
        tk::fatal("Requested mapActor(0x%04X) is not a valid named map actor", mapActor);
        return -1;
    }

    mapActor &= ~cMapMetaMask; //? Remove metadata so it can be used as an index

    if (!mProfileID.isBufferReady()) [[unlikely]] {
        tk::fatal("Requesting mapActor(%i) without spritemap present", mapActor);
        return -1;
    }

    if (mapActor >= mProfileID.size()) [[unlikely]] {
        tk::fatal("Requested mapActor(%i) is out of bounds(%i)", mapActor, mProfileID.size());
        return -1;
    }

    return mProfileID[mapActor];
}

u16 MapActorMgr::profToMap(s32 profileID) {
    // TODO: Optimize this
    
    for (s32 i = 0; i < mProfileID.size(); i++) {
        if (mProfileID[i] == profileID) {
            return i | cMapMetaMask;
        }
    }
    
    return 0xFFFF;
}

}

extern "C" void red_CreateMapActorMgr() {
    red::MapActorMgr::createInstance(nullptr); //? Allocated on a CourseTask sub-heap, so it is freed when CourseTask dies
    red::MapActorMgr::instance()->init();
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

extern "C" s32 red_MapToProfR0R3Hook() tAssembly(
    mr r3, r0;
    b _ZN3red11mapToProfR3Et;
)

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
    s32 mapToProfR3(const u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
    
    s32 mapToProfR6(int, int, int, const u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
    
    s32* mapToProfR6Ptr(int, int, int, const u16 mapActor) tRegSave {
        static s32 sProf = -1;
        sProf = red::MapActorMgr::instance()->mapToProf(mapActor);
        return &sProf;
    }
    
    s32 mapToProfR8(int, int, int, int, int, const u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
    
    s32 mapToProfR9(int, int, int, int, int, int, const u16 mapActor) tRegSave {
        return red::MapActorMgr::instance()->mapToProf(mapActor);
    }
}

using namespace tk::ppc;
#include <telkin/UndefineRegisters.h>

tBranch(0x02004584, red::mapToProfR9, tk::BranchType::bl);
tBranch(0x020045B0, red::mapToProfR9, tk::BranchType::bl);
tPatch32u(0x020045B4, mr(R::r6, R::r3));

tBranch(0x02004948, red::mapToProfR6Ptr, tk::BranchType::bl);

tBranch(0x02004DA8, red::mapToProfR6, tk::BranchType::bl);
tBranch(0x02005024, red::mapToProfR6, tk::BranchType::bl);

tBranch(0x0200828C, red::mapToProfR8, tk::BranchType::bl);
tPatch32u(0x02008290, mr(R::r0, R::r3));

tBranch(0x0200869C, red::mapToProfR8, tk::BranchType::bl);
tPatch32u(0x020086A0, mr(R::r9, R::r3));

tBranch(0x02007C6C, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x02008078, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x020080A4, red_MapToProfR0R3Hook, tk::BranchType::bl);
tPatch32u(0x020080A8, mr(R::r6, R::r3));

tBranch(0x0200826C, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x02008458, red_MapToProfR0R3Hook, tk::BranchType::bl);

tBranch(0x0200A82C, red_MapToProfR0R3Hook, tk::BranchType::bl);
tPatch32u(0x0200A830, mr(R::r29, R::r3));
