#pragma once

#include <red/event_sys/EventBase.h>
#include <red/event_sys/EventDelegator.h>
#include <actor/ActorBase.h>

namespace red {

    /**
     * @brief Event fired after static initializers have ran for the game. This is helpful when accessing .bss variables which are not initialized during mod init.
     */
    class StaticInitEvent : public EventBase<StaticInitEvent> {
    public:
        using Listener = EventBase<StaticInitEvent>::BasicListener;

    public:
        static void subscribe(EventDelegator<StaticInitEvent>::Listener& listener);
        static void emit();

    private:
        [[nodiscard]]
        static Delegator& getDelegator();
    };

}
