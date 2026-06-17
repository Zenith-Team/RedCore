# Event System
RedCore provides a simple event system for hooking into various parts of the game's execution. See @ref include/red/event for a full list of events.

To subscribe to an event, create a listener object from the target class, pass the desired stage if applicable, and then pass a lambda to be called when the event is triggered. Example with @ref red::RenderStepEvent:
```cpp
#include <red/event/RenderStepEvent.h>
#include <telkin/Print.h>

using namespace red;

RenderStepEvent::Listener<RenderStepEvent::Stage::BeforePost> MyEvent([](RenderStepEvent& e) {
    tk::println("Rendering on layer %d", e.getRenderInfo().getLayerIndex());
});
```

An optional priority parameter can be passed to the listener after the lambda, where a higher number fires earlier and the default is 0.
