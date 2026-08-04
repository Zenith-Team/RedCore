#include <telkin/Hooks.h>

// Always initialize BonusTime even outside of NSLU
tPatchNop(0x025D4530); // Gamescene::create (this is for BoostRush)
tPatchNop(0x025D45A0); // Gamescene::create
