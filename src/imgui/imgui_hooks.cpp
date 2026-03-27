#include <imgui/imgui.h>
#include <imgui_gx2/imgui_impl_gx2.h>
#include <imgui_gx2/imgui_impl_wiiu.h>

#include <controller/cafe/seadCafeVPadDeviceCafe.h>
#include <framework/cafe/seadGameFrameworkCafe.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadExpHeap.h>

#include <common/aglRenderBuffer.h>
#include <layer/aglRenderer.h>

#include <graphics/LayerMgr.h>
#include <system/ApplicationFramework.h>

#include <red/public/seadGameFrameworkCafe.h>

#include <telkin/Hooks.h>

static ImGui_ImplWiiU_ControllerInput sImguiInput;
static sead::CafeVPadDevice::VPadInfo sVPadInfo;

static void* imguiAlloc(size_t size, void* userData)
{
    sead::Heap* heap = static_cast<sead::Heap*>(userData);
    return heap->tryAlloc(size, 4);
}

static void imguiFree(void* ptr, void* userData)
{
    sead::Heap* heap = static_cast<sead::Heap*>(userData);
    heap->free(ptr);
}

void initImGui()
{
    sead::ExpHeap* sImGuiHeap = sead::ExpHeap::tryCreate(5 * 1024 * 1024, "ImGuiHeap"); // 5MB
    ImGui::SetAllocatorFunctions(&imguiAlloc, &imguiFree, sImGuiHeap);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1280.0f, 720.0f);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGX2_Init();
    ImGui_ImplWiiU_Init();

    ImGui::StyleColorsDark();

    sImguiInput.vpad = &sVPadInfo.status[0];

    // Set variables that are checked in sead::GameFrameworkCafe::swapBuffer_() to copy the FrameBuffers to the DisplayBuffers
    sead::GameFrameworkCafe* fw = static_cast<sead::GameFrameworkCafe*>(LayerMgr::instance()->getFramework());
    fw->setCurrentDisplaybuffer([](u32) -> void { });
    fw->set36c(true);
}
tBranch(0x029D5B68, initImGui, tk::BranchType::b); // RootTask::prepare

void beginImGui(sead::GameFrameworkCafe* fw)
{
    // Read input up front so we can update ImGui first, this is later copied from in sead::CafeVPadDevice::calc()
    sVPadInfo.last_read_length = VPADRead(VPAD_CHAN_0, sVPadInfo.status, VPAD_MAX_READ_BUFS, &sVPadInfo.last_read_error);

    ImGui_ImplGX2_NewFrame();
    ImGui_ImplWiiU_ProcessInput(&sImguiInput);
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    static_cast<red::pub::GameFrameworkCafe*>(fw)->procDraw_();
}
tBranch(0x02A031DC, beginImGui, tk::BranchType::bl); // sead::GameFrameworkCafe::procFrame_

void endImGui(sead::GameFrameworkCafe* fw)
{
    static_cast<red::pub::GameFrameworkCafe*>(fw)->procCalc_();

    ImGui::Render();

    // TV
    {
        agl::RenderBuffer& fb = *agl::lyr::Renderer::instance()->getRenderBuffer(agl::lyr::cDisplayType_Top_TV);
        fb.bind();

        ImGui_ImplGX2_RenderDrawData(ImGui::GetDrawData());
    }
    // DRC
    {
        agl::RenderBuffer& fb = *agl::lyr::Renderer::instance()->getRenderBuffer(agl::lyr::cDisplayType_Bottom_DRC);
        fb.bind();

        ImDrawData* d = ImGui::GetDrawData();
        d->FramebufferScale = ImVec2(
            fb.getPhysicalArea().getSizeX() / fb.getVirtualSize().x,
            fb.getPhysicalArea().getSizeY() / fb.getVirtualSize().y
        );

        ImGui_ImplGX2_RenderDrawData(d);
    }
}
tBranch(0x02A031F0, endImGui, tk::BranchType::bl); // sead::GameFrameworkCafe::procFrame_

void ApplicationFramework__calc_(ApplicationFramework* self)
{
    // Skip copying the FrameBuffers to the DisplayBuffers here.
    // It's going to be copied later in sead::GameFrameworkCafe::swapBuffer_()

    self->getTaskMgr()->afterCalc();

    LayerMgr* layerMgr = LayerMgr::instance();

    sead::Graphics::instance()->lockDrawContext();
    {
        layerMgr->calcParallelExec();
    }
    sead::Graphics::instance()->unlockDrawContext();
}
tBranch(0x029C5104, ApplicationFramework__calc_, tk::BranchType::b); // ApplicationFramework::calc

void sead__CafeVPadDevice__calc(sead::CafeVPadDevice* self)
{
    self->getVPadInfo() = sVPadInfo;

    if (ImGui::GetIO().WantCaptureMouse) // Don't let touch inputs be passed down to the game if ImGui is being interated with
    {
        for (u32 i = 0; i < VPAD_MAX_READ_BUFS; i++)
        {
            VPADStatus* status = &self->getVPadInfo().status[i];
            status->tpdata.touched = VPAD_TP_TOUCH_OFF;
            status->tpdata.invalid = VPAD_TP_VALIDITY_INVALID_XY;
            status->tpdata1.touched = VPAD_TP_TOUCH_OFF;
            status->tpdata1.invalid = VPAD_TP_VALIDITY_INVALID_XY;
            status->tpdata2.touched = VPAD_TP_TOUCH_OFF;
            status->tpdata2.invalid = VPAD_TP_VALIDITY_INVALID_XY;
        }
    }
}
tBranch(0x029F4FB4, sead__CafeVPadDevice__calc, tk::BranchType::b); // sead::CafeVPadDevice::calc
