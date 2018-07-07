#ifndef __COMMON_EVENT_LOOP_H
#define __COMMON_EVENT_LOOP_H

#include <list>
#include <memory>
#include <functional>
#include <ev.h>

namespace tiny {

typedef std::function<void()> EventCallback;

struct EventHolderBase {
    virtual void called() = 0;
};

typedef std::shared_ptr<EventHolderBase> EventHolderPtr;

template<typename EventType>
struct EventHolder : public EventHolderBase {
    EventHolder(const EventCallback &callback)
            : callback(callback) {}

    void called() override {
        if (callback) callback();
    }

    EventType event;
    EventCallback callback;
};

class EventLoop {
public:
    EventLoop(bool default_ = true);

    static EventLoop *instance() {
        static EventLoop loop;
        return &loop;
    }

    EventLoop& onIdle(const EventCallback &callback);

    EventLoop& onTimer(const EventCallback &callback, ev_tstamp repeat, ev_tstamp after = 0.0);

    void run();

public:
    struct ev_loop *evLoop() { return evloop_; }

private:
    struct ev_loop *evloop_;

    std::list<EventHolderPtr> holders_;
};

} // namespace tiny

#endif // __COMMON_EVENT_LOOP_H