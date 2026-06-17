# Creating a Basic Actor
To demonstrate the registration process, we will create a simple actor that will display a rotating model, and register it to the game.

First, define the class for the actor, inheriting from @ref Actor
```cpp
#include <actor/Actor.h>
#include <graphics/AnimModel.h>

class DemoActor : public Actor {
public:
    // Holds registry information
    static Profile* sProfile;

    DemoActor(const ActorCreateParam& param); // The constructor is only used for setting initial values
    ~DemoActor() override = default; // The heap is managed, unnecessary to implement beyond default
    
    // Basic lifecycle functions
    Result create() override;
    bool execute() override;
    bool draw() override;

private:
    // Add custom data
    AnimModel* mModel;
};
```
To learn more about the actor lifecycle, see @ref Actor.

Next, register the actor to the game using the `getRegistrar()` function we created [earlier](./registration.md):
```cpp
Profile* DemoActor::sProfile = getRegistrar()->newProfile<DemoActor>("demo_actor")
    .resources<"star_coin">(ProfileInfo::cResType_Course)
    .build();
```
The builder has additional methods for setting different properties of the profile, see @ref red::ProfileBuilder for more information.

Finally, create the actor's constructor and implement the lifecycle functions:
```cpp
DemoActor::DemoActor(const ActorCreateParam& param)
    : Actor(param)
    , mModel(nullptr)
{ }

ActorBase::Result DemoActor::create() {
    // Load a model from content/Common/actor/
    // SZS/BFRES name, then the model name inside
    mModel = AnimModel::create("star_coin", "star_coinA");
    
    return cResult_Success;
}

bool DemoActor::execute() {
    mAngle.z() += sead::Mathf::deg2idx(2.0f); // Spin 2 degrees per frame
    
    // Update the model's transformation/animation
    mModel->update(mPos, mAngle, mScale);
    
    return true;
}

bool DemoActor::draw() {
    mModel->draw();
    return true;
}
```

Finally, to test the actor, you must inform the level editor that it exists so that it can be placed in a level. See [here](https://github.com/Zenith-Team/Zap/tree/main/editor) for an example.
