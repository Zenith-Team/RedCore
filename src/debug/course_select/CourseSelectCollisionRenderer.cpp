#include <layer/aglRenderInfo.h>
#include <course_select/CourseSelectTask.h>
#include <graphics/RenderObjLayer.h>
#include <red/event/RenderStepEvent.h>
#include <gfx/seadGraphicsContext.h>
#include <actor/ActorMgr.h>
#include <course_select/CourseSelectActor.h>
#include <imgui/imgui.h>
#include <gfx/seadPrimitiveRenderer.h>
#include <course_select/CourseSelectCollisionCheckMgr.h>
#include <course_select/CourseSelectLayerMgr.h>

static void drawLine3D(const sead::Vector3f& position, const u32 rotation, const sead::Color4f& color, const f32 lineLength, const f32 lineThickness) {
    // idk whats going on here (jh math)
    sead::Vector3f scale(lineLength, lineThickness, lineThickness);
    sead::Vector3u rot(0x80000000, (rotation + 0x40000000) * 0xFFFFFFFF, 0x00000000);
    f32 rotSin;
    f32 rotCos;
    sead::Mathf::sinCosIdx(&rotSin, &rotCos, rotation);
    sead::Vector3f pos(position.x + (lineLength * rotSin) / 2, position.y, position.z + (lineLength * rotCos) / 2);

    sead::Matrix34f mtx;
    mtx.makeSRzxyTIdx(scale, rot, pos);
    sead::PrimitiveRenderer::instance()->setModelMatrix(mtx);
    sead::PrimitiveRenderer::instance()->drawCube(color);
    sead::PrimitiveRenderer::instance()->setModelMatrix(sead::Matrix34f::ident);
}

static void renderID(const agl::lyr::RenderInfo& renderInfo, const CourseSelectActor* actor) {
    sead::Vector2f pos;
    renderInfo.getCamera()->projectByMatrix(&pos, actor->getPos(), *renderInfo.getProjection(), *renderInfo.getViewport());
    pos.x = pos.x + 1280.0f / 2.0f;
    pos.y = pos.y +  720.0f / 2.0f;
    pos.y = -pos.y + 720.0f;
    
    char buf[128] = { 0 };
    __os_snprintf(buf, 128, "0x%08X", actor->getActorUniqueID().getValue());
    ImGui::GetForegroundDrawList()->AddText(ImVec2(pos.x, pos.y), ImColor(1.0f, 0.0f, 0.0f, 1.0f), buf);
    //__os_snprintf(buf, 128, "%f, %f", actor->position.x, actor->position.y);
    //ImGui::GetForegroundDrawList()->AddText(ImVec2(pos.x, pos.y + 20.0f), ImColor(1.0f, 0.0f, 1.0f, 1.0f), buf);
}

static void renderRotation(const CourseSelectActor* actor) {
    drawLine3D(actor->getPos(), actor->getAngle().y(), sead::Color4f::cMagenta, 128.0f, 2.0f);
}

namespace red {
 
void renderCourseSelectCollisions(const agl::lyr::RenderInfo& renderInfo) {
    sead::GraphicsContext context;
    // context.setDepthEnable(true, true);                         // Automatically set by ctor
    // context.setDepthFunc(sead::Graphics::cDepthFunc_LessEqual); // ^^
    context.setCullingMode(sead::Graphics::cCullingMode_None);
    context.setBlendEnable(false);
    context.apply();
    
    sead::PrimitiveRenderer::instance()->setCamera(*renderInfo.getCamera());
    sead::PrimitiveRenderer::instance()->setProjection(*renderInfo.getProjection());
    sead::PrimitiveRenderer::instance()->setModelMatrix(sead::Matrix34f::ident);
    sead::PrimitiveRenderer::instance()->begin();
    
    const auto& array1 = CourseSelectCollisionCheckMgr::instance()->getArray1();
    const auto& array2 = CourseSelectCollisionCheckMgr::instance()->getArray2();
    
    for (const auto& cc : array1) {
        CourseSelectActor* actor = sead::DynamicCast<CourseSelectActor>(ActorMgr::instance()->getActorPtr(cc.getOwnerID()));
        if (!actor)
            continue;
    
        sead::PrimitiveRenderer::instance()->drawWireCube(
            sead::PrimitiveRenderer::CubeArg()
                .setCenter(actor->getPos() + sead::Vector3f(cc.getCenterOffset().x, cc.getCenterOffset().y, cc.getCenterOffset().z))
                .setSize(sead::Vector3f(cc.getSize(), cc.getSize(), cc.getSize()))
                .setColor(sead::Color4f::cYellow)
        );
    }
    
    for (const auto& cc : array2) {
        CourseSelectActor* actor = sead::DynamicCast<CourseSelectActor>(ActorMgr::instance()->getActorPtr(cc.getOwnerID()));
        if (!actor)
            continue;
    
        sead::PrimitiveRenderer::instance()->drawWireCube(
            sead::PrimitiveRenderer::CubeArg()
                .setCenter(actor->getPos() + sead::Vector3f(cc.getCenterOffset().x, cc.getCenterOffset().y, cc.getCenterOffset().z))
                .setSize(sead::Vector3f(cc.getSize(), cc.getSize(), cc.getSize()))
                .setColor(sead::Color4f::cRed)
        );
    }
    
    for (auto it = ActorMgr::instance()->getActorBegin(); it != ActorMgr::instance()->getActorEnd(); it++) {
        const CourseSelectActor* actor = sead::DynamicCast<CourseSelectActor>(*it);
        if (actor != nullptr) {
            renderID(renderInfo, actor);
            renderRotation(actor);
        }
    }
    
    sead::PrimitiveRenderer::instance()->end();
}

RenderStepEvent::Listener<RenderStepEvent::Stage::BeforePost> CourseSelectCollisionRenderer([](RenderStepEvent& e) {
    if (!CourseSelectTask::instance() || !CourseSelectCollisionCheckMgr::instance())
        return;

    if (!e.filterLayer(CourseSelectLayerMgr::cLayer3D)) // TODO: DRC breaks the text rendering
        return;

    if (!e.filterRenderStep(RenderObjLayer::cRenderStep_PostFx))
        return;

    renderCourseSelectCollisions(e.getRenderInfo());
});

}
