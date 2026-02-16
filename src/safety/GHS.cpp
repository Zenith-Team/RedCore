#include <telkin/Telkin.h>
#include <red/util/Log.h>

namespace red {
    void handlePVC() {
        red::print("ERROR: A pure virtual function was called\n");
        return;
    }
    
    void handleDVC() {
        red::print("ERROR: A deleted virtual function was called\n");
        return;
    }
}

tBranch(0x2AFA554, red::handlePVC, tk::BranchType::b);
tBranch(0x2AFA5E8, red::handleDVC, tk::BranchType::b);
