#pragma once

#include <framework/cafe/seadGameFrameworkCafe.h>

namespace red::pub {
    
    class GameFrameworkCafe : public sead::GameFrameworkCafe {
    public:
        using sead::GameFrameworkCafe::procDraw_;
        using sead::GameFrameworkCafe::procCalc_;
    };
    
}
