#ifndef MSG_QUEUE_HPP
#define MSG_QUEUE_HPP

#include <condition_variable>
#include <queue>
#include <chrono>

namespace PostMethods {
    class MessageQueue {
    public:
        MessageQueue(uint16_t maxSize) :
            maxSize_(maxSize) {}

        using MsgPtr = void*;
        using MsgTimeout = std::chrono::seconds;

        bool addMessage(MsgPtr msg);
        MsgPtr waitForMessage(MsgTimeout timeout = MsgTimeout::zero());
        MsgPtr tryGetMessage();

    private:
        const uint16_t maxSize_;

        std::mutex mtx_;
        std::condition_variable cv_;
        std::queue<MsgPtr> queue_;
    };
}

#endif // MSG_QUEUE_HPP
