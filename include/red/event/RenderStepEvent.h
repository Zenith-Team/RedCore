#pragma once

#include <layer/aglLayer.h>
#include <layer/aglRenderStep.h>
#include <layer/aglRenderInfo.h>
#include <layer/aglRenderer.h>
#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <red/event_sys/EventStage.h>
#include <telkin/Assembly.h>
#include <concepts>

namespace red {
    
    class RenderStepEvent : public EventBase<RenderStepEvent> {
    public:
        // Supported stages: Pre, AfterPre, BeforePost, Post
        static void subscribe(Listener& listener, EventStage stage);
    
        [[nodiscard]]
        agl::lyr::Layer& getLayer() const { return *mLayer; }
        
        [[nodiscard]]
        agl::lyr::RenderStep& getRenderStep() const { return *mRenderStep; }
        
        [[nodiscard]]
        agl::lyr::RenderInfo& getRenderInfo() const { return *mRenderInfo; }
        
        [[nodiscard]]
        bool isDisplayList() const { return mRenderAsDL; }
        
    public:
        // filter helpers
        
        bool filterLayer(std::convertible_to<s32> auto... args) {
            return ((agl::lyr::Renderer::instance()->getLayer(args) == &getLayer()) || ...);
        }
        
        bool filterRenderStep(std::convertible_to<s32> auto... args) {
            return ((args == getRenderInfo().getRenderStep()) || ...);
        }
        
    private:
        RenderStepEvent(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo)
            : mLayer(layer)
            , mRenderStep(renderStep)
            , mRenderInfo(renderInfo)
            , mRenderAsDL(renderAsDL)
        { }
    
        [[nodiscard]]
        static Delegator& getDelegatorPre();
        
        [[nodiscard]]
        static Delegator& getDelegatorAfterPre();
        
        [[nodiscard]]
        static Delegator& getDelegatorBeforePost();
        
        [[nodiscard]]
        static Delegator& getDelegatorPost();
        
        // 0x02A39644, 0x02A395D0
        static void emitPre(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave;
        // 0x02A39644, 0x02A395D0
        static void emitAfterPre(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave;
        // 0x02A39668, 0x02A395F4
        static void emitBeforePost(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave;
        // 0x02A39668, 0x02A395F4
        static void emitPost(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave;
        
    public: //! TODO: Make private
        static void hookPre();
        static void hookPost();
        
    private:
        agl::lyr::Layer* mLayer;
        agl::lyr::RenderStep* mRenderStep;
        agl::lyr::RenderInfo* mRenderInfo;
        bool mRenderAsDL;
    };
    
}
