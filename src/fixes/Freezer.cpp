#include <telkin/Telkin.h>
#include <bullet/FireBallBase.h>

// Add a dynamic cast to FireBallBase in the Freezer's melt function, allowing custom fireballs to melt it

#include <telkin/DefineRegisters.h>

namespace red {

    bool FreezerIsActorFireBall(Actor* actor) tRegSave{
        FireBallBase* fireball = sead::DynamicCast<FireBallBase>(actor);
        return fireball != nullptr;
    }

    void FreezerAddFireBallCastCheck() tAssembly(
        mr r3, r31;

        tSaveLR;

        bl _ZN3red22FreezerIsActorFireBallEP5Actor;
        mr r11, r3;

        tRestoreLR;

        mr r3, r31; // Replaced instruction
        blr;
    );

} // namespace red

#include <telkin/UndefineRegisters.h>

using namespace tk::ppc;

tBranch(0x02779FCC, red::FreezerAddFireBallCastCheck, tk::BranchType::bl);
tPatchNop(0x02779FD4);
tPatchNop(0x02779FD8);
tPatch32u(0x02779FDC, cmpwi(r11, 1));
