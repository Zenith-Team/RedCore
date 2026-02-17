#pragma once

#include <container/seadBuffer.h>
#include <heap/seadDisposer.h>

namespace red {

constexpr u32 cSpritemapVersion = 1;
constexpr u32 cMapActorNum = 724; // TODO: Move to game headers ?

class MapActorMgr
{
    SEAD_SINGLETON_DISPOSER(MapActorMgr);

public:
    MapActorMgr();
    ~MapActorMgr();

    void init(sead::Heap* heap);
    s32 mapToProf(s16 mapActor);

private:
    sead::Buffer<s32> mProfileID;
};

}
