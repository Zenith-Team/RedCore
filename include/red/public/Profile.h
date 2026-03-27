#pragma once

#include <actor/Profile.h>

namespace red::pub {
    
    class Profile : public ::Profile {
    public:
        using ::Profile::mFactory;
        using ::Profile::mID;
        using ::Profile::mActorCreateInfo;
        using ::Profile::mIsResLoaded;
        using ::Profile::mFlag;
    };
    
}
