#define TELKIN_REGISTERS
#include <telkin/Assembly.h>
#include <telkin/Hooks.h>
#include <telkin/Print.h>

namespace red {
    void caughtNullState() {
        tk::fatal("Attempted to change from null state! Ensure your actor is currently within a state.");
    }

    void checkNullState() tAssembly (
        cmpwi r3, 0x0;
        beql _ZN3red15caughtNullStateEv;
        
        lwz r11, 0x0(r3); // replaced instruction
        blr;
    )
}
tBranch(0x029C4BEC, red::checkNullState, tk::BranchType::bl); // StateStateMethod::changeToSubState
