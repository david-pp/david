#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <memory>
#include <cstdlib>
#include <unistd.h>

class FSM {
public:
    typedef std::function<void(uint32_t)> EventAction;
    typedef std::function<void(uint32_t)> StateAction;

    FSM(uint32_t initstate) : current_(initstate) {}

    uint32_t state() const { return current_; }

    bool is(uint32_t state) const {
        return current_ == state;
    }

    bool can(uint32_t event) {
        return transitions_[current_].find(event) != transitions_[current_].end();
    }

    bool transit(uint32_t event) {
        if (!can(event))
            return false;

        // on-before
        if (event_actions_[event].first)
            event_actions_[event].first(event);

        // on-leave
        if (state_actions_[current_].second)
            state_actions_[current_].second(current_);

        current_ = transitions_[current_][event];

        // on-enter
        if (state_actions_[current_].first)
            state_actions_[current_].first(current_);

        // on-after
        if (event_actions_[event].second)
            event_actions_[event].second(event);

        return true;
    }

public:
    bool add_transition(uint32_t from, uint32_t to, uint32_t event) {
        transitions_[from][event] = to;
    }

    void onBefore(uint32_t event, const EventAction &action) {
        event_actions_[event].first = action;
    }

    void onAfter(uint32_t event, const EventAction &action) {
        event_actions_[event].second = action;
    }


    void onEnter(uint32_t state, const StateAction &action) {
        state_actions_[state].first = action;
    }

    void onLeave(uint32_t state, const StateAction &action) {
        state_actions_[state].second = action;
    }

private:
    uint32_t current_ = 0;
    std::map<uint32_t, std::map<uint32_t, uint32_t >> transitions_;
    std::map<uint32_t, std::pair<StateAction, StateAction>> state_actions_;
    std::map<uint32_t, std::pair<EventAction, EventAction>> event_actions_;
};


class TimerEvent {
public:
    typedef std::function<void()> Callback;

    TimerEvent(uint32_t timepoint, const Callback &cb, uint32_t interval = 300)
            : timepoint_(timepoint), interval_(interval), callback_(cb) {}

    bool run(uint32_t now) {
        if (!runtime_) {
            if (now >= timepoint_ && now <= (timepoint_ + interval_)) {
                callback_();
                runtime_ = now;
                return true;
            }
        }
        return false;
    }

private:
    uint32_t timepoint_ = 0;
    uint32_t interval_ = 0;
    uint32_t runtime_ = 0;

    Callback callback_;
};

class TimerEvents {
public:
    void at(uint32_t timepoint, const TimerEvent::Callback &callback, uint32_t interval = 300) {
        auto event = std::make_shared<TimerEvent>(timepoint, callback, interval);
        if (event) {
            events_.push_back(event);
        }
    }

    void run(uint32_t now) {
        for (auto event : events_) {
            if (event)
                event->run(now);
        }
    }

private:
    std::vector<std::shared_ptr<TimerEvent>> events_;
};


class TimerFSM : public FSM {
public:
    using FSM::FSM;

    void run(uint32_t now) {
        for (auto timer : timers_) {
            timer.second->run(now);
        }
    }

    bool add_timer_transition(uint32_t from, uint32_t to, uint32_t event, uint32_t timepoint) {

        auto timer = std::make_shared<TimerEvent>(timepoint, [event, this]() {
            this->transit(event);
        }, 500);
        if (timer) {
            timers_[event] = timer;
        }

        add_transition(from, to, event);
    }

private:
    std::map<uint32_t, std::shared_ptr<TimerEvent>> timers_;
};


enum OjbectForm {
    solid,
    liquid,
    gas
};

enum ObjectEvent {
    melt,
    freeze,
    vaporize,
    condense,

    timer_1,
    timer_2,
};

void test_1() {

    uint32_t now = time(0);

    TimerFSM fsm(solid);
    fsm.add_transition(solid, liquid, melt);
    fsm.add_transition(liquid, gas, vaporize);
    fsm.add_timer_transition(gas, liquid, timer_1, now+5);

    fsm.onBefore(melt, [](uint32_t event) {
        std::cout << "Before Melt..." << std::endl;
    });

    fsm.onAfter(melt, [](uint32_t event) {
        std::cout << "After Melt..." << std::endl;
    });

    fsm.onLeave(solid, [](uint32_t state) {
        std::cout << "Leave Solid ..." << std::endl;
    });

    fsm.onEnter(liquid, [](uint32_t state) {
        std::cout << "Melt to liquid..." << std::endl;
    });


    fsm.onAfter(timer_1, [&fsm](uint32_t event){
        std::cout << "timer .... :" << fsm.state() << std::endl;
    });


    fsm.transit(melt);
    fsm.transit(vaporize);

    std::cout << "current_:" << fsm.state() << std::endl;

    while (true)
    {
        sleep(1);
        fsm.run(time(0));
    }
}

void test_2() {

    uint32_t now = time(0);

    TimerEvents timers;
    timers.at(now + 10, []() {
        std::cout << "fired!!!" << std::endl;
    });

    for (uint32_t i = 1; i <= 9; i++) {
        timers.at(now + i, [i]() {
            std::cout << i << std::endl;
        });
    }

    while (true) {
//        sleep(1);
        uint32_t now = time(0);
        timers.run(now);
    }
}

int main() {
    test_1();
//    test_2();
    return EXIT_SUCCESS;
}