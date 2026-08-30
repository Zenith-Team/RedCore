#include <telkin/Telkin.h>

using namespace tk::ppc;

// Small Gear Collision Fix
tPatch32u(0x0277ECA4, lfs(F::f0, R::r11, -0x6E0C)); // Change the value read in staticinit from 32.0f to the proper 72.0f
