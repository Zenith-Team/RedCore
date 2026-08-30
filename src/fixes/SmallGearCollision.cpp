#include <telkin/Telkin.h>

using namespace tk::ppc;

// Small Gear Collision Fix
tPatch32u(0x0277ECA4, 0xC00B91F4); // Change the value read in staticinit from 32.0f to the proper 72.0f
