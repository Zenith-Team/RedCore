#define TELKIN_REGISTERS
#include <layer/aglLayer.h>
#include <layer/aglRenderStep.h>
#include <layer/aglRenderInfo.h>
#include <prim/seadDelegateEventSlot.h>
#include <red/event/RenderStepEvent.h>
#include <telkin/Telkin.h>

template <red::RenderStepEvent::Stage S>
red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegator() {
    static Delegator sDelegator;
    return sDelegator;
}

template red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegator<red::RenderStepEvent::Stage::Pre>();
template red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegator<red::RenderStepEvent::Stage::AfterPre>();
template red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegator<red::RenderStepEvent::Stage::BeforePost>();
template red::RenderStepEvent::Delegator& red::RenderStepEvent::getDelegator<red::RenderStepEvent::Stage::Post>();

void red::RenderStepEvent::hookPre(agl::lyr::Layer* layer, agl::lyr::RenderInfo* renderInfo) {
    emit<Stage::Pre>(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
    layer->preDraw(*renderInfo);
    emit<Stage::AfterPre>(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
}
tBranch(0x02A39644, red::RenderStepEvent::hookPre, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395D0, red::RenderStepEvent::hookPre, tk::BranchType::bl); // agl::lyr::Layer::draw_()

void red::RenderStepEvent::hookPost(agl::lyr::Layer* layer, agl::lyr::RenderInfo* renderInfo) {
    emit<Stage::BeforePost>(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
    layer->postDraw(*renderInfo);
    emit<Stage::Post>(layer, agl::lyr::Renderer::instance()->isDisplayList__(), &layer->getRenderStep(renderInfo->getRenderStep()), renderInfo);
}
tBranch(0x02A39668, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
tBranch(0x02A395F4, red::RenderStepEvent::hookPost, tk::BranchType::bl); // agl::lyr::Layer::draw_()
