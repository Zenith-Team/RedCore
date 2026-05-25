#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

namespace red {
    void caughtNullEntrance() {
        tk::fatal("Attempted to access an entrance which doesn't exist.\nDid you forget to add one?");
    }
    
    void checkNullEntrance() tAssembly (
        cmpwi r3, 0x0;
        beql _ZN3red18caughtNullEntranceEv;
        
        lbz r10, 0xD(r3); // replaced instruction
        blr;
    )
}
tBranch(0x024D2A30, red::checkNullEntrance, tk::BranchType::bl);
