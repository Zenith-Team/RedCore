#include <imgui/imgui.h>
#include <imgui_gx2/imgui_impl_gx2.h>
#include <imgui_gx2/imgui_impl_wiiu.h>

#include <controller/cafe/seadCafeVPadDeviceCafe.h>
#include <framework/cafe/seadGameFrameworkCafe.h>
#include <framework/seadTaskMgr.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <filedevice/seadFileDevice.h>
#include <heap/seadExpHeap.h>

#include <common/aglRenderBuffer.h>
#include <layer/aglRenderer.h>

#include <graphics/LayerMgr.h>
#include <system/ApplicationFramework.h>

#include <red/public/seadGameFrameworkCafe.h>
#include <red/heap/RedCoreHeap.h>
#include <red/event/TaskPrepareEvent.h>

#include <telkin/Hooks.h>

static ImGui_ImplWiiU_ControllerInput sImguiInput;
static sead::CafeVPadDevice::VPadInfo sVPadInfo;

static void* imguiAlloc(size_t size, void* userData) {
    sead::Heap* heap = static_cast<sead::Heap*>(userData);
    return heap->tryAlloc(size, 4);
}

static void imguiFree(void* ptr, void* userData) {
    sead::Heap* heap = static_cast<sead::Heap*>(userData);
    heap->free(ptr);
}

static void setupDarkTheme() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 0.70f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.74f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.74f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.74f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.74f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.74f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.74f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.74f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.74f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.74f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.74f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.72f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.56f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.72f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.72f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.72f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 15;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    style.WindowRounding                    = 7;
    style.ChildRounding                     = 4;
    style.FrameRounding                     = 3;
    style.PopupRounding                     = 4;
    style.ScrollbarRounding                 = 9;
    style.GrabRounding                      = 3;
    style.LogSliderDeadzone                 = 4;
    style.TabRounding                       = 4;
}

namespace red {
    TaskPrepareEvent::Listener<TaskPrepareEvent::Stage::After> InitImGui([](TaskPrepareEvent& e) {
        if (e.getTask()->getName() != "RootTask")
            return;
        
        sead::ExpHeap* imguiHeap = sead::ExpHeap::tryCreate(5 * 1024 * 1024, "ImGuiHeap", RedCoreHeap::instance()); // 5MiB
        ImGui::SetAllocatorFunctions(&imguiAlloc, &imguiFree, imguiHeap);
    
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
    
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.DisplaySize = ImVec2(1280.0f, 720.0f);
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
        ImGui_ImplGX2_Init();
        ImGui_ImplWiiU_Init();
    
        ImGui::StyleColorsDark();
        setupDarkTheme();
        
        sead::FileDevice::LoadArg loadArg;
        loadArg.path = "red/font/OpenSans.ttf";
        loadArg.heap = imguiHeap;
        
        u8* fontFile = sead::FileDeviceMgr::instance()->tryLoad(loadArg);
        
        if (fontFile) {
            tk::println("ImGui font file loaded");
            
            ImFontConfig fontConfig;
            fontConfig.FontDataOwnedByAtlas = loadArg.need_unload;
            ImFont* font = io.Fonts->AddFontFromMemoryTTF(fontFile, loadArg.read_size, 17.0f, &fontConfig);
            
            tk::println("ImGui font loaded");
        }
    
        sImguiInput.vpad = &sVPadInfo.status[0];
    
        // Set variables that are checked in sead::GameFrameworkCafe::swapBuffer_() to copy the FrameBuffers to the DisplayBuffers
        sead::GameFrameworkCafe* fw = static_cast<sead::GameFrameworkCafe*>(LayerMgr::instance()->getFramework());
        fw->setDeferredCopyCallback([](s32) -> void { });
        fw->set36c(true);
    });
} // namespace red

namespace red {
    void beginImGui(sead::GameFrameworkCafe* fw) {
        // Read input up front so we can update ImGui first, this is later copied from in sead::CafeVPadDevice::calc()
        sVPadInfo.last_read_length = VPADRead(VPAD_CHAN_0, sVPadInfo.status, VPAD_MAX_READ_BUFS, &sVPadInfo.last_read_error);
    
        ImGui_ImplGX2_NewFrame();
        ImGui_ImplWiiU_ProcessInput(&sImguiInput);
        ImGui::NewFrame();
        
        //ImGui::ShowDemoWindow();
    
        static_cast<red::pub::GameFrameworkCafe*>(fw)->procDraw_();
    }
} // namespace red
tBranch(0x02A031DC, red::beginImGui, tk::BranchType::bl); // sead::GameFrameworkCafe::procFrame_

namespace red {
    void endImGui(sead::GameFrameworkCafe* fw) {
        static_cast<red::pub::GameFrameworkCafe*>(fw)->procCalc_();
    
        ImGui::Render();
    
        { // TV
            agl::RenderBuffer& fb = *agl::lyr::Renderer::instance()->getRenderBuffer(agl::lyr::cDisplayType_Top_TV);
            fb.bind();
    
            ImGui_ImplGX2_RenderDrawData(ImGui::GetDrawData());
        }
        { // DRC (Gamepad)
            agl::RenderBuffer& fb = *agl::lyr::Renderer::instance()->getRenderBuffer(agl::lyr::cDisplayType_Bottom_DRC);
            fb.bind();
    
            ImDrawData* d = ImGui::GetDrawData();
            d->FramebufferScale = ImVec2(fb.getPhysicalArea().getSizeX() / fb.getVirtualSize().x, fb.getPhysicalArea().getSizeY() / fb.getVirtualSize().y);
    
            ImGui_ImplGX2_RenderDrawData(d);
        }
    }
} // namespace red
tBranch(0x02A031F0, red::endImGui, tk::BranchType::bl); // sead::GameFrameworkCafe::procFrame_

namespace red {
    void ApplicationFramework__calc_(ApplicationFramework* self) {
        // Skip copying the FrameBuffers to the DisplayBuffers here.
        // It's going to be copied later in sead::GameFrameworkCafe::swapBuffer_()
    
        self->getTaskMgr()->afterCalc();
    
        LayerMgr* layerMgr = LayerMgr::instance();
    
        sead::Graphics::instance()->lockDrawContext();
        layerMgr->calcParallelExec();
        sead::Graphics::instance()->unlockDrawContext();
    }
} // namespace red
tBranch(0x029C5104, red::ApplicationFramework__calc_, tk::BranchType::b); // ApplicationFramework::calc

namespace red {
    void sead__CafeVPadDevice__calc(sead::CafeVPadDevice* self) {
        self->getVPadInfo() = sVPadInfo;
    
        if (ImGui::GetIO().WantCaptureMouse) { // Don't let touch inputs be passed down to the game if ImGui is being interacted with
            for (u32 i = 0; i < VPAD_MAX_READ_BUFS; i++) {
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
} // namespace red
tBranch(0x029F4FB4, red::sead__CafeVPadDevice__calc, tk::BranchType::b); // sead::CafeVPadDevice::calc
