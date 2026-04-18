#pragma once

#include <actor/ProfileInfo.h>

namespace red::pub {
    
    class ProfileInfo : public ::ProfileInfo {
    public:
        using ::ProfileInfo::DrawPriority;
        using ::ProfileInfo::cDrawPriority;
        using ::ProfileInfo::cResType;
        using ::ProfileInfo::cResNum;
        using ::ProfileInfo::cResList;
    };
    
}
