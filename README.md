<img alt="banner" src="https://github.com/user-attachments/assets/4060bc9b-185c-4944-9b66-d18025d08a15" />
<div align="center">
  <img alt="wiiu" height="56" src="https://github.com/user-attachments/assets/fc1f38b1-46a1-44ee-acfb-ac436d900dc6">
  <a href="https://go.nsmbu.net/discord">
    <img alt="discord" height="56" src="https://github.com/user-attachments/assets/495433d7-1ddc-47f1-8c8e-f4ef137754c2">
  </a>
  <a href="https://zenith.nsmbu.net/wiki/RedCore">
    <img alt="docs" height="56" src="https://github.com/user-attachments/assets/dc583842-a4d3-4635-b036-33d5f9bfc4d5">
  </a>
</div>

## Overview
A modern C++ API and modding framework for **New Super Mario Bros. U** codemods.

## Features
- **Dynamic**: Powered by [Telkin](https://github.com/Zenith-Team/Telkin), multiple codemods using RedCore can be loaded simultaneously!
- **Headers**: All libraries and game interfaces are accessible and rapidly updated.
- **Target Support**: Pre-configured for US, EU, and JP titles on both Cemu and Console.
- **Named Registries**: Register custom actors in a quick and compatible manner using string names.
- **Event System**: Delegate-based event handling with listeners for simple hooks without ASM/RE or conflicts.
- **ImGui**: Easily produce visual interfaces for fast debugging and development.

## Quick Start
Fork the [Example Mod](https://github.com/Zenith-Team/RedCore-Example-Mod), and clone the repository
```sh
git clone https://github.com/Zenith-Team/RedCore-Example-Mod MyMod
cd MyMod
```
Install the [Tachyon SDK](https://github.com/Zenith-Team/Tachyon) (requires [Node.js](https://nodejs.org/) v24+)
```yml
npm i -g https://github.com/Zenith-Team/Tachyon/releases/latest/download/Tachyon.tgz
```
Build and run the project for your region (example with `US`)
```rb
tachyon pm install
tachyon compile US
tachyon launch US
```

## Usage (Actor)
Create a registrar for your mod:
```cpp
// Re-use this function globally
red::Registrar* getRegistrar() {
    static red::Registrar sRegistrar("example"); // This is your mod's namespace
    return &sRegistrar;
}
```

Register a simple actor:
```cpp
#include <actor/Actor.h>
#include <graphics/AnimModel.h>

class DemoActor : public Actor {
public:
    // Holds registry information
    static Profile* sProfile;

    // Constructor
    DemoActor(const ActorCreateParam& param);
    // Destructor (the heap is managed, unnecessary to implement beyond default)
    ~DemoActor() override = default;
    
    // Basic lifecycle functions
    Result create() override;
    bool execute() override;
    bool draw() override;

    // Add custom data
    AnimModel* mModel;
};

// Register our actor
Profile* DemoActor::sProfile = getRegistrar()->newProfile<DemoActor>("demo_actor")
    .resources<"star_coin">(ProfileInfo::cResType_Course)
    .build();

// Main code
DemoActor::DemoActor(const ActorCreateParam& param)
    : Actor(param)
    , mModel(nullptr)
{ }

ActorBase::Result DemoActor::create() {
    // Load our model
    // SZS name, then the model name inside
    mModel = AnimModel::create("star_coin", "star_coinA");
    
    return cResult_Success;
}

bool DemoActor::execute() {
    // Called every tick, any behavioural logic goes here
    
    // Update our model's transformation
    sead::Matrix34f mtx;
    mtx.makeRTIdx(mAngle, mPos);
    mModel->setMtxRT(mtx);
    mModel->setScale(mScale);
    mModel->calcMdl();
    
    return true;
}

bool DemoActor::draw() {
    // Called every frame
    mModel->draw();
    return true;
}
```
See the [documentation](https://zenith.nsmbu.net/wiki/RedCore) for comprehensive information on advanced usage.

## Usage (Event)
Hooking into the rendering loop using the event system:
```cpp
#include <red/event/RenderStepEvent.h>
#include <game/AreaLayerMgr.h>
#include <graphics/RenderObjLayer.h>
#include <imgui/imgui.h>

using namespace red;

// Register a listener
//    Event: RenderStepEvent
//    Stage: BeforePost
// Then pass a lambda to be called
RenderStepEvent::Listener<RenderStepEvent::Stage::BeforePost> MyEvent([](RenderStepEvent& e) {
    // Filter for the specific layer and step we want
    if (!e.filterLayer(AreaLayerMgr::cLayer_3D, AreaLayerMgr::cLayer_3D_DRC))
        return;
    if (!e.filterRenderStep(RenderObjLayer::cRenderStep_PostFx))
        return;
    
    // Display a demo popup
    ImGui::Begin("HelloWorld");
    ImGui::Text("Hello from RedCore!");
    ImGui::End();
});
```
See the [documentation](https://zenith.nsmbu.net/wiki/RedCore) for a list of all available events.

## Credits
- [Luminyx](https://github.com/Luminyx1) - API, Infrastructure, Headers
- [stupidestmodder](https://github.com/stupidestmodder) - API, Headers
- [jhmaster](https://github.com/jhmaster2000) - Infrastructure

## Special Thanks
- [AboodXD](https://github.com/aboood40091) - Headers
- [Omar](https://github.com/ocornut) - ImGui Library
- [GaryOderNichts](https://github.com/GaryOderNichts) - ImGui GX2 Backend
