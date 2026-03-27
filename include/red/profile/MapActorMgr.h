#pragma once

#include <container/seadBuffer.h>
#include <heap/seadDisposer.h>

namespace red {

constexpr s32 cMapActorNum = 724; // TODO: Move to game headers ?

class MapActorMgr
{
    SEAD_SINGLETON_DISPOSER(MapActorMgr)

public:
    static constexpr u32 cSpritemapVersion = 2;
    static constexpr u16 cMapMetaMask = 0b1111000000000000;

public:
    MapActorMgr();
    ~MapActorMgr();

    void init();
    s32 mapToProf(u16 mapActor);

private:
    sead::Buffer<s32> mProfileID;
};

}
