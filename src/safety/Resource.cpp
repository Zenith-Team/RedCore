#define TELKIN_REGISTERS
#include <telkin/Assembly.h>
#include <telkin/Hooks.h>
#include <telkin/Print.h>

namespace red {
    void caughtNullResource() {
        tk::fatal("Attempted to get an unloaded resource!\nForgot to register a resource for your profile?");
    }
    
    void checkNullResource() tAssembly (
        cmpwi r21, 0x0;
        beql _ZN3red18caughtNullResourceEv;
        
        lwz r20, 0x10(r21); // replaced instruction
        blr;
    )
}
tBranch(0x024EE384, red::checkNullResource, tk::BranchType::bl); // Model::createG3dImpl
