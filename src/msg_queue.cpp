#include "msg_queue.hpp"

using namespace PostMethods;

bool MessageQueue::addMessage(MsgPtr msg) {
    if (queue_.size() == maxSize_) {
        // Queue is already full
        return false;
    }

    queue_.push(msg);

    return true;
}

MessageQueue::MsgPtr MessageQueue::waitForMessage(MsgTimeout timeout) {
    std::unique_lock<std::mutex> lock(mtx_);

    bool status;

    if (timeout != MsgTimeout::zero()) {
        status = cv_.wait_for(lock, timeout , [&] { return !queue_.empty(); });
    } else {
        cv_.wait(lock, [&] { return !queue_.empty(); });
        status = true;
    }

    if (!status) {
        return nullptr;
    }

    auto data = queue_.front();
    queue_.pop();

    return data;
}

MessageQueue::MsgPtr MessageQueue::tryGetMessage() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (!queue_.empty()) {
        auto data = queue_.front();
        queue_.pop();
        return data;
    } else {
        return nullptr;
    }
}
