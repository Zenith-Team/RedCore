#include <telkin/Telkin.h>

namespace red {
    void handlePVC() {
        tk::fatal("A pure virtual function was called\n");
    }
    
    void handleDVC() {
        tk::fatal("A deleted virtual function was called\n");
    }
}

tBranch(0x2AFA554, red::handlePVC, tk::BranchType::b); // __pure_virtual_called
tBranch(0x2AFA5E8, red::handleDVC, tk::BranchType::b); // __deleted_virtual_called
