#define PROFILE_INFO_AS_NAMESPACE
#include <red/profile/ProfileEx.h>

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

#include <red/util/Log.h>

s32 red_sProfileCount = ProfileInfo::cProfileID_Max;

static s32 getNext(s32 id) {
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
        red::print("Registering vanilla profile %i\n", id);
        sProfileList[id] = this;
    } else {
        red::print("Registering custom profile [%i:%i:%s]\n", id, mID, name.cstr());
        red::ProfileEx::addIdentifierProfile(name, this);
    }
}

Profile* Profile::get(s32 id) {
    return sProfileList[id];
}

sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, Profile*, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfiles;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, red::ProfileEx::ResourceData, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileResources;
sead::FixedStrTreeMap<red::ProfileEx::cNameMaxLen, s16, red::ProfileEx::cMaxCustomProfiles> red::ProfileEx::sCustomProfileDrawPriorities;

const char* red::ProfileEx::sProfileNames[ProfileInfo::cProfileID_Max + cMaxCustomProfiles] = { nullptr };

Profile* red::ProfileEx::get(const sead::SafeString& identifier)  {
    Profile** it = sCustomProfiles.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return nullptr;
    }
    return *it;
}

Profile* red::ProfileEx::get(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return nullptr;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return Profile::get(id); // Not infinite loop because we define a custom impl above, not link to original
    }
    
    sead::SafeString identifier = ProfileEx::getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get profile\n");
        return nullptr;
    }
    
    return ProfileEx::get(identifier);
}

const char* red::ProfileEx::getName(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return "";
    }
    
    const char* name = sProfileNames[id];
    
    //red::print("Mapped ID [%i:%u]\n", id, name);
    
    return name;
}

s16 red::ProfileEx::getDrawPriority(const sead::SafeString& identifier) {
    s16* it = sCustomProfileDrawPriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return 0;
    }
    return *it;
}

s16 red::ProfileEx::getDrawPriority(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return ProfileInfo::cDrawPriority[id];
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get draw priority\n");
        return 0;
    }
    
    s16* it = sCustomProfileDrawPriorities.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return 0;
    }
    return *it;
}

ProfileInfo::ResType red::ProfileEx::getResType(const sead::SafeString& identifier) {
    ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return ProfileInfo::cResType_Num;
    }
    return it->resource_type;
}

ProfileInfo::ResType red::ProfileEx::getResType(const s32 id) {
    //red::print("Requesting res type for ProfileID %i\n", id);
    
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: ProfileID %i (0x%x) was not found\n", id, id);
        return ProfileInfo::cResType_Boot;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        //red::print("ID was less than %i, using cResType\n", ProfileInfo::cProfileID_Max);
        return static_cast<const ProfileInfo::ResType>(ProfileInfo::cResType[id]);
    }
    
    //red::print("ID was >= %i, querying name... ", ProfileInfo::cProfileID_Max);
    sead::SafeString identifier = getName(id);
    OSReport("%s.\n", identifier.cstr());
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get res type\n");
        return ProfileInfo::cResType_Boot;
    }
    
    //red::print("Querying overload\n");
    return ProfileEx::getResType(identifier);
}

u32 red::ProfileEx::getResNum(const sead::SafeString& identifier) {
    ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return 0;
    }
    return it->resource_count;
}

u32 red::ProfileEx::getResNum(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n", id);
        return 0;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return ProfileInfo::cResNum[id];
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get resource count\n");
        return 0;
    }
    
    return ProfileEx::getResNum(identifier);
}

const sead::SafeString* red::ProfileEx::getResList(const sead::SafeString& identifier) {
    ResourceData* it = sCustomProfileResources.find(identifier);
    if (it == nullptr) [[unlikely]] {
        red::print("ERROR: Profile identifier \"%s\" was not found\n", identifier.cstr());
        return nullptr;
    }
    return it->resources;
}

const sead::SafeString* red::ProfileEx::getResList(const s32 id) {
    if (id < 0 || id > cMaxCustomProfiles + ProfileInfo::cProfileID_Max) [[unlikely]] {
        red::print("ERROR: Profile ID %i was not found\n");
        return nullptr;
    }
    
    if (id < ProfileInfo::cProfileID_Max) {
        return ProfileInfo::cResList[id];
    }
    
    sead::SafeString identifier = getName(id);
    
    if (identifier.isEmpty()) [[unlikely]] {
        red::print("ERROR: Failed to get resource list\n");
        return nullptr;
    }
    
    return ProfileEx::getResList(identifier);
}

void red::ProfileEx::addIdentifierProfile(const sead::SafeString& identifier, Profile* profile) {
    sCustomProfiles.insert(identifier, profile);
}

void red::ProfileEx::setResources(const sead::SafeString& identifier, const ProfileInfo::ResType resourceType, sead::SafeString* resources, const u8 resourceCount) {
    ResourceData data = {
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

// Patch info

extern "C" Profile* red_GetProfileHook(s32 id) {
    return red::ProfileEx::get(id);
}

// Profile::get
tHook(0x2019774, "red_GetProfileHook", tk::BranchType::b);

extern "C" s16 red_DrawPriorityHook(s32 id) {
    return red::ProfileEx::getDrawPriority(id);
}

// ProfileInfo::getDrawPriority
tHook(0x2019988, "red_DrawPriorityHook", tk::BranchType::b);

extern "C" ProfileInfo::ResType red_GetResTypeHook(const s32 id) {
    return red::ProfileEx::getResType(id);
}

// ProfileInfo::getResType
tHook(0x20199A8, "red_GetResTypeHook", tk::BranchType::b);

extern "C" u32 red_GetResNumHook(const s32 id) {
    return red::ProfileEx::getResNum(id);
}

// ProfileInfo::getResNum
tHook(0x20199CC, "red_GetResNumHook", tk::BranchType::b);

extern "C" const sead::SafeString* red_GetResListHook(const s32 id) {
    return red::ProfileEx::getResList(id);
}

// ProfileInfo::getResList
tHook(0x20199EC, "red_GetResListHook", tk::BranchType::b);

// Profile count patches

extern "C" s32 red_LoadProfileCountR29() tAssembly(
    lis r29, red_sProfileCount@ha;
    addi r29, r29, red_sProfileCount@l;
    lwz r29, 0x0(r29);
    blr;
)

// ActorResLoader::load
tHook(0x200A89C, "red_LoadProfileCountR29", tk::BranchType::bl);

extern "C" s32 red_LoadProfileCountR30() tAssembly(
    lis r30, red_sProfileCount@ha;
    lwz r30, red_sProfileCount@l(r30);
    blr;
)

// ActorResLoader::unload
tHook(0x200a950, "red_LoadProfileCountR30", tk::BranchType::bl);
