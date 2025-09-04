#include "event_flag_grp.hpp"

#include <mutex>
#include <condition_variable>

using namespace EventFlags;

void EventFlagGroup::set(Flags flags) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        flags_ |= flags;
    }
    cv_.notify_all();
}

void EventFlagGroup::clear(Flags flags) {
    std::lock_guard<std::mutex> lock(mtx_);
    flags_ &= ~flags;
}

Flags EventFlagGroup::waitAny(Flags flag) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return (flags_ & flag) != 0; });
    return flags_ & flag;
}

Flags EventFlagGroup::waitAll(Flags flags) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] { return (flags_ & flags) == flags; });
    return flags_ & flags;
}
