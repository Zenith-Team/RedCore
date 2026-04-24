#include <red/profile/ProfileEx.h>
#include <red/public/ProfileInfo.h>
#include <actor/ActorBase.h>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

s32 red_sProfileCount = ProfileInfo::cProfileID_Max;

static s32 getNext(const s32 id) {
    if (id == -1) { // string-profile
        return red_sProfileCount++;
    }
    
    return id;
}

// Custom impl of profile ctor, mods use this instead of defining the symbol for original.
Profile::Profile(ActorFactory factory, s32 id, const sead::SafeString& name, const ActorCreateInfo* createInfo, Flag flag)
    : mFactory(factory)
    , mID(getNext(id))
    , mActorCreateInfo(createInfo != nullptr ? createInfo : &ActorCreateInfo::cDefault)
    , mIsResLoaded(false)
    , mFlag(flag)
{
    red::ProfileEx::setName(mID, name.cstr());
    
    if (id != -1 && id < ProfileInfo::cProfileID_Max) { // vanilla replacements
        tk::println("Registering vanilla profile %i", id);
        sProfileList[id] = this;
    } else {
        tk::println("Registering custom profile [%i:%i:%s]", id, mID, name.cstr());
        red::ProfileEx::addIdentifierProfile(name, this);
    }
}

Profile* Profile::get(s32 id) {
    return sProfileList[id];
}

sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, Profile*, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfiles;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, red::ProfileEx::ResourceData, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileResources;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, s16, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileDrawPriorities;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, s16, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileExecutePriorities;
red::ProfileEx::VanillaProfileExecutePriorities red::ProfileEx::sVanillaProfileExecutePriorities;

const char* red::ProfileEx::sProfileNames[ProfileInfo::cProfileID_Max + cMaxCustomProfiles] = { nullptr };

Profile* red::ProfileEx::get(const sead::SafeString& identifier)  {
    Profile** it = sCustomProfiles.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found. sCustomProfiles: %d", identifier.cstr(), sCustomProfiles.size());
        return nullptr;
    }
    return *it;
}

Profile* red::ProfileEx::get(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("Profile ID %i was not found", id);
        return nullptr;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return Profile::get(id); // Not infinite loop because we define a custom impl above, not link to original
    }

    const sead::SafeString identifier = ProfileEx::getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        tk::fatal("Failed to get profile");
        return nullptr;
    }
    
    return ProfileEx::get(identifier);
}

const char* red::ProfileEx::getName(const s32 id) {
    if (id < 0 || id >= cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("Profile ID %i was not found", id);
        return "";
    }
    
    const char* name = sProfileNames[id];
    
    //tk::println("Mapped ID [%i:%u]", id, name);
    
    return name;
}

s16 red::ProfileEx::getDrawPriority(const sead::SafeString& identifier) {
    const s16* it = sCustomProfileDrawPriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return 0;
    }
    return *it;
}

s16 red::ProfileEx::getDrawPriority(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("Profile ID %i was not found", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return pub::ProfileInfo::cDrawPriority[id];
    }

    const sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        tk::fatal("Failed to get draw priority");
        return 0;
    }

    const s16* it = sCustomProfileDrawPriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return 0;
    }
    return *it;
}

s16 red::ProfileEx::getExecutePriority(const sead::SafeString& identifier) {
    const s16* it = sCustomProfileExecutePriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return 0;
    }
    return *it;
}

s16 red::ProfileEx::getExecutePriority(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("Profile ID %i was not found", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return sVanillaProfileExecutePriorities[id];
    }

    const sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        tk::fatal("Failed to get exec priority");
        return 0;
    }
    
    s16* it = sCustomProfileExecutePriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return 0;
    }
    return *it;
}

ProfileInfo::ResType red::ProfileEx::getResType(const sead::SafeString& identifier) {
    const ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return ProfileInfo::cResType_Num;
    }
    return it->resource_type;
}

ProfileInfo::ResType red::ProfileEx::getResType(const s32 id) {
    //tk::println("Requesting res type for ProfileID %i", id);
    
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("ProfileID %i (0x%x) was not found", id, id);
        return ProfileInfo::cResType_Boot;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        //tk::println("ID was less than %i, using cResType", ProfileInfo::cProfileID_Max);
        return static_cast<const ProfileInfo::ResType>(pub::ProfileInfo::cResType[id]);
    }
    
    //tk::println("ID was >= %i, querying name... ", ProfileInfo::cProfileID_Max);
    const sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        tk::fatal("Failed to get res type");
        return ProfileInfo::cResType_Boot;
    }
    
    //tk::println("Querying overload");
    return ProfileEx::getResType(identifier);
}

u32 red::ProfileEx::getResNum(const sead::SafeString& identifier) {
    const ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return 0;
    }
    return it->resource_count;
}

u32 red::ProfileEx::getResNum(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("Profile ID %i was not found", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return pub::ProfileInfo::cResNum[id];
    }

    const sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        tk::fatal("Failed to get resource count");
        return 0;
    }
    
    return ProfileEx::getResNum(identifier);
}

const sead::SafeString* red::ProfileEx::getResList(const sead::SafeString& identifier) {
    const ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        tk::fatal("Profile identifier \"%s\" was not found", identifier.cstr());
        return nullptr;
    }
    return it->resources;
}

const sead::SafeString* red::ProfileEx::getResList(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        tk::fatal("Profile ID %i was not found");
        return nullptr;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return pub::ProfileInfo::cResList[id];
    }

    const sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        tk::fatal("Failed to get resource list");
        return nullptr;
    }
    
    return ProfileEx::getResList(identifier);
}

void red::ProfileEx::addIdentifierProfile(const sead::SafeString& identifier, Profile* profile) {
    sCustomProfiles.insert(identifier, profile);
}

void red::ProfileEx::setResources(const sead::SafeString& identifier, const ProfileInfo::ResType resourceType, sead::SafeString* resources, const u8 resourceCount) {
    const ResourceData data = {
        .resource_count = resourceCount,
        .resources = resources,
        .resource_type = resourceType
    };
    sCustomProfileResources.insert(identifier, data);
}

void red::ProfileEx::setName(const s32 id, const char* name) {
    sProfileNames[id] = name;
}

void red::ProfileEx::setDrawPriority(const sead::SafeString& identifier, const s16 priority) {
    sCustomProfileDrawPriorities.insert(identifier, priority);
}

void red::ProfileEx::setExecutePriority(const sead::SafeString& identifier, const s16 priority) {
    sCustomProfileExecutePriorities.insert(identifier, priority);
}

void red::ProfileEx::setExecutePriority(const s32 id, const s16 priority) {
    sVanillaProfileExecutePriorities[id] = priority;
}

// Patch info

// Profile::get
tBranch(0x2019774, red::ProfileEx::get, Profile*(const s32), tk::BranchType::b);

// ProfileInfo::getDrawPriority
tBranch(0x2019988, red::ProfileEx::getDrawPriority, s16(const s32), tk::BranchType::b);

// ProfileInfo::getResType
tBranch(0x20199A8, red::ProfileEx::getResType, ProfileInfo::ResType(const s32), tk::BranchType::b);

// ProfileInfo::getResNum
tBranch(0x20199CC, red::ProfileEx::getResNum, u32(const s32), tk::BranchType::b);

// ProfileInfo::getResList
tBranch(0x20199EC, red::ProfileEx::getResList, const sead::SafeString*(const s32), tk::BranchType::b);

// Profile count patches

namespace red {
    s32 loadProfileCountR29() tAssembly(
        lis r29, red_sProfileCount@ha;
        addi r29, r29, red_sProfileCount@l;
        lwz r29, 0x0(r29);
        blr;
    )
}
// ActorResLoader::load
tBranch(0x200A89C, red::loadProfileCountR29, tk::BranchType::bl);

namespace red {
    s32 loadProfileCountR30() tAssembly(
        lis r30, red_sProfileCount@ha;
        lwz r30, red_sProfileCount@l(r30);
        blr;
    )
}

// ActorResLoader::unload
tBranch(0x200a950, red::loadProfileCountR30, tk::BranchType::bl);

// Execute Priority

namespace red {
    s16 getExecutePriority(const ActorBase* actor) {
        // Sort by custom value instead of by profile ID. Vanilla ones default to profile ID, but can be edited via ProfileEditBuilder.
        return red::ProfileEx::getExecutePriority(actor->getProfileID());
    }
}
tBranch(0x2009428, red::getExecutePriority, tk::BranchType::bl); // ActorMgr::pushExecuteAndDrawList_
tBranch(0x200945C, red::getExecutePriority, tk::BranchType::bl); // ActorMgr::pushExecuteAndDrawList_
