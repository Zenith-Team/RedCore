#define TELKIN_REGISTERS
#include <layer/aglLayer.h>
#include <layer/aglRenderStep.h>
#include <layer/aglRenderInfo.h>
#include <prim/seadDelegateEventSlot.h>
#include <red/event/RenderStepEvent.h>
#include <telkin/Telkin.h>

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorPre() {
    static Delegator sDelegator;
    return sDelegator;
}

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorAfterPre() {
    static Delegator sDelegator;
    return sDelegator;
}

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorBeforePost() {
    static Delegator sDelegator;
    return sDelegator;
}

red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegatorPost() {
    static Delegator sDelegator;
    return sDelegator;
}

void red::RenderStepEvent::subscribePre(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorPre().connect(listener);
}

void red::RenderStepEvent::subscribeAfterPre(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorAfterPre().connect(listener);
}

void red::RenderStepEvent::subscribeBeforePost(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorBeforePost().connect(listener);
}

void red::RenderStepEvent::subscribePost(EventDelegator<RenderStepEvent>::Listener& listener) {
    getDelegatorPost().connect(listener);
}

void red::RenderStepEvent::emitPre(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorPre().fire(event);
}

void red::RenderStepEvent::emitAfterPre(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorAfterPre().fire(event);
}

void red::RenderStepEvent::emitBeforePost(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorBeforePost().fire(event);
}

void red::RenderStepEvent::emitPost(agl::lyr::Layer* layer, bool renderAsDL, agl::lyr::RenderStep* renderStep, agl::lyr::RenderInfo* renderInfo) tRegSave {
    RenderStepEvent event(layer, renderAsDL, renderStep, renderInfo);
    getDelegatorPost().fire(event);
}

void red::RenderStepEvent::hookPre(agl::lyr::Layer* layer, agl::lyr::RenderInfo* renderInfo)
{
    emitPre(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
    layer->preDraw(*renderInfo);
    emitAfterPre(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
}
tBranch(0x02A39644, red::RenderStepEvent::hookPre, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395D0, red::RenderStepEvent::hookPre, tk::BranchType::bl); // agl::lyr::Layer::draw_()

void red::RenderStepEvent::hookPost(agl::lyr::Layer* layer, agl::lyr::RenderInfo* renderInfo)
{
    emitBeforePost(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
    layer->postDraw(*renderInfo);
    emitPost(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
}
tBranch(0x02A39668, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395F4, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
