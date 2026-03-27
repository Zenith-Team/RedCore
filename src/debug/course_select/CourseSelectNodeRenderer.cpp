#include "gfx/seadColor.h"
#include "math/seadVector.h"
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
#include <course_select/CourseSelectMap.h>

namespace red {

void renderCourseSelectNodes(const agl::lyr::RenderInfo& renderInfo) {
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
    
    CourseSelectMap* map = CourseSelectMap::instance();
    
    for (s32 world = 0; world < 9; world++) { // iterate over every world
        const sead::OffsetList<MapModel>& list = map->getMapModel(world);
        
        s32 subworld = 0;
        for (const MapModel& map_it : list) { // iterate over every sub-area
            const Model* model = map_it.getModel()->getModel();
            
            s32 boneNum = model->getBoneNum();
            
            for (s32 bone = 0; bone < boneNum; bone++) { // iterate over every bone
                const char* name = model->getBoneName(bone);
                
                sead::Vector3f pos;
                
                map->getBoneWorldPos(&pos, world, subworld, name);
                
                sead::PrimitiveRenderer::instance()->drawCube(
                    sead::PrimitiveRenderer::CubeArg()
                        .setCenter(pos)
                        .setSize(sead::Vector3f(16.0f, 16.0f, 16.0f))
                        .setColor(sead::Color4f::cCyan)
                );
                
                sead::PrimitiveRenderer::instance()->drawWireCube(
                    sead::PrimitiveRenderer::CubeArg()
                        .setCenter(pos)
                        .setSize(sead::Vector3f(16.0f, 16.0f, 16.0f))
                        .setColor(sead::Color4f::cBlack)
                );
                
                // TODO: Optimize?
                sead::Vector2f projectedPos;
                renderInfo.getCamera()->projectByMatrix(&projectedPos, pos, *renderInfo.getProjection(), *renderInfo.getViewport());
                projectedPos.x = projectedPos.x + 1280.0f / 2.0f;
                projectedPos.y = projectedPos.y +  720.0f / 2.0f;
                projectedPos.y = -projectedPos.y + 720.0f;
                
                ImGui::GetForegroundDrawList()->AddText(ImVec2(projectedPos.x, projectedPos.y), ImColor(1.0f, 0.0f, 1.0f, 1.0f), name);
            }
            
            subworld++;
        }
    }
    
    sead::PrimitiveRenderer::instance()->end();
}

RenderStepEvent::Listener<RenderStepEvent::Stage::BeforePost> CourseSelectNodeRenderer([](RenderStepEvent& e) {
    if (!CourseSelectTask::instance() || !CourseSelectMap::instance())
        return;

    if (!e.filterLayer(CourseSelectLayerMgr::cLayer3D)) // TODO: DRC breaks the text rendering
        return;

    if (!e.filterRenderStep(RenderObjLayer::cRenderStep_PostFx))
        return;

    renderCourseSelectNodes(e.getRenderInfo());
});

}
