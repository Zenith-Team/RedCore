#pragma once

#include <actor/Profile.h>

struct PublicProfile {
    ActorFactory            factory;
    s32                     id;
    const ActorCreateInfo*  actor_create_info;
    bool                    is_res_loaded;
    u32                     flag;
};
