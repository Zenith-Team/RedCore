#pragma once

#include <layer/aglRenderer.h>

namespace red::pub {
    
    class Renderer : public ::agl::lyr::Renderer {
    public:
        using agl::lyr::Renderer::isDisplayList_;
    };
    
}
