#include <telkin/Hooks.h>

// Fixes the titlescreen crash
tPatch32u(0x024D71C4, 0x480000E8); // CaptureRenderer::drawMiiCaptureModel, b +0xE8
