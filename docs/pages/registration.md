# Registration Setup

To register actors from your mod to the game, you must first create a global @ref red::Registrar object. The recommended method is to create a getter function with a static variable to hold the registrar:
```cpp
#include <red/registry/Registrar.h>

// Re-use this function globally
red::Registrar* getRegistrar() {
    static red::Registrar sRegistrar("mymod"); // This is your mod's namespace
    return &sRegistrar;
}
```
The namespace is used to create a unique ID for your mod, and is recommended to be the same as your mod's name.

> [!NOTE]
> The definition of the function should not be placed in a header file as the static variable should only be created once.

Once you have a registrar, you can register new profiles (registration information for an actor), replace vanilla profiles outright, or edit specific properties of an existing profile.

See @ref red::Registrar for more information.

You may now move on to [creating actors](./actor.md).
