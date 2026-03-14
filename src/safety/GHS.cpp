#include <telkin/Telkin.h>

namespace red {
    void handlePVC() {
        tk::print("ERROR: A pure virtual function was called\n");
        return;
    }
    
    void handleDVC() {
        tk::print("ERROR: A deleted virtual function was called\n");
        return;
    }
}

tBranch(0x2AFA554, red::handlePVC, tk::BranchType::b);
tBranch(0x2AFA5E8, red::handleDVC, tk::BranchType::b);
