# ImGui
The ImGui library is available as part of RedCore, useful for debugging and development to visualize and manipulate states/logic at runtime. Refer to [the ImGui documentation](https://github.com/ocornut/imgui/blob/master/docs/README.md) for more information.

To use ImGui in your mod, simply include the header and call into the library:
```cpp
#include <imgui/imgui.h>

bool DemoActor::execute() {
    ImGui::Begin("Demo Actor Debug Panel");
    ImGui::Text("Position: %f, %f, %f", mPos.x(), mPos.y(), mPos.z());
    ImGui::End();
    
    //...
    return true;
}
```
