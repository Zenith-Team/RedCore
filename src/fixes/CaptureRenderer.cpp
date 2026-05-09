#include <telkin/Telkin.h>

using namespace tk::ppc;

#ifdef __EMULATOR__

// Fixes the titlescreen crash
tPatch32u(0x024D71C4, b(0xE8)); // CaptureRenderer::drawMiiCaptureModel

#endif
