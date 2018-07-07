#include "eventloop.h"

namespace tiny {

EventLoop::EventLoop(bool default_) {
    if (default_)
        evloop_ = ev_default_loop(0);
    else
        evloop_ = ev_loop_new(EVFLAG_AUTO);
}

void EventLoop::run() {
    if (!evloop_) return;

    ev_loop(evloop_, 0);
}

template<typename EventType>
static void s_ev_callback(struct ev_loop *loop, EventType *w, int revents) {
    auto holder = (EventHolder<EventType> *) w->data;
    if (holder) {
        holder->called();
    }
}

EventLoop &EventLoop::onIdle(const EventCallback &callback) {
    if (evloop_) {
        auto holder = std::make_shared<EventHolder<ev_idle>>(callback);
        if (holder) {
            holder->event.data = holder.get();
            ev_idle_init (&holder->event, s_ev_callback<ev_idle>);
            ev_idle_start(evloop_, &holder->event);

            holders_.push_back(holder);
        }
    }

    return *this;
}

EventLoop &EventLoop::onTimer(const EventCallback &callback, ev_tstamp repeat, ev_tstamp after) {
    if (evloop_) {
        auto holder = std::make_shared<EventHolder<ev_timer>>(callback);
        if (holder) {
            holder->event.data = holder.get();
            ev_timer_init (&holder->event, s_ev_callback<ev_timer>, after, repeat);
            ev_timer_start(evloop_, &holder->event);

            holders_.push_back(holder);
        }
    }

    return *this;
}

} // namespace tiny