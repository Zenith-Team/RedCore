#pragma once

#include <event/EventBase.h>

namespace red {

    template <typename T>
    class FreezeFrameEvent : public EventBase {
    public:
        bool isJoin(const ActorBase* actor) const override {
            if (sead::DynamicCast<T>(actor)) {
                return true;
            }

            return !mFrozen;
        }

        Result execute() override {
            if (mFrozen)
                return cResult_Keep;

            return cResult_Exit;
        }

        void unfreeze() {
            mFrozen = false;
        }

    private:
        bool mFrozen = true;
    };

} // namespace red
