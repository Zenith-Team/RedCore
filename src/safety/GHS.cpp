#include <telkin/Telkin.h>
#include <red/util/Log.h>

tBranch(0x2AFA554, "red_HandlePVC", tk::BranchType::b);
extern "C" void red_HandlePVC() {
    red::print("ERROR: A pure virtual function was called\n");
    return;
}

tBranch(0x2AFA5E8, "red_HandleDVC", tk::BranchType::b);
extern "C" void red_HandleDVC() {
    red::print("ERROR: A deleted virtual function was called\n");
    return;
}
