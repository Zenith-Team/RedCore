#pragma once

#include <event/EventBase.h>

namespace red {

    /**
     * @brief Actor event that will pause execution of all actors except the ones specified in the whitelist.
     * @tparam T Actor types to be whitelisted. These will continue execution.
     * @details Push this to @ref EventMgr to freeze, then call @c unfreeze() to end the event.
     */
    template <typename... T>
    class FreezeFrameEvent : public EventBase {
    public:
        /**
         * @brief Ends the freeze event and resumes execution of all actors.
         */
        void unfreeze() {
            mFrozen = false;
        }
        
        /**
         * @brief Re-activates the freeze event.
         */
        void freeze() {
            mFrozen = true;
        }

    private:
        bool isJoin(const ActorBase* actor) const override {
            if ((... || (sead::DynamicCast<T>(actor) != nullptr))) { 
                return true;
            }

            return !mFrozen;
        }

        Result execute() override {
            if (mFrozen)
                return cResult_Keep;

            return cResult_Exit;
        }

    private:
        bool mFrozen = true;
    };

} // namespace red
