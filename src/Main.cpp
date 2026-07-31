#include <telkin/Print.h>
#include <telkin/Hooks.h>

void main() {
    tk::println("Welcome to REDCORE");
}

#include <layout_sys/TextBox.h>

namespace red {

void titleVer(TextBox* textBox)
{
    textBox->SetString(L"Red." MOD_VERSION);
    textBox->SetVtxColor(0, nw::ut::Color8(255, 255, 255, 255));
    textBox->SetVtxColor(2, nw::ut::Color8(255, 0, 0, 255));
}

}

tBranch(0x02668138, red::titleVer, tk::BranchType::bl); // Title::create
tBranch(0x026681A0, red::titleVer, tk::BranchType::bl); // Title::create

tBranch(0x02667FC4, red::titleVer, tk::BranchType::bl); // Title::create
tBranch(0x0266802C, red::titleVer, tk::BranchType::bl); // Title::create
