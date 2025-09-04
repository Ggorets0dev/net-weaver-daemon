#ifndef EVENT_FLAG_GRP_HPP
#define EVENT_FLAG_GRP_HPP

#include <mutex>
#include <condition_variable>

#define IS_BIT_SET(value, bit)   (((value) & (1U << (bit))) != 0)
#define IS_BIT_CLEAR(value, bit) (((value) & (1U << (bit))) == 0)

// How much events we need in one flag group
#define EVENT_FLAGS_COUNT_PER_GROUP     16

namespace EventFlags {
    #if (EVENT_FLAGS_COUNT_PER_GROUP > 0 && EVENT_FLAGS_COUNT_PER_GROUP <= 8)
    using Flags = uint8_t;
    #elif (EVENT_FLAGS_COUNT_PER_GROUP > 8 && EVENT_FLAGS_COUNT_PER_GROUP <= 16)
    using Flags = uint16_t;
    #elif (EVENT_FLAGS_COUNT_PER_GROUP > 16 && EVENT_FLAGS_COUNT_PER_GROUP <= 32)
    using Flags = uint32_t;
    #elif (EVENT_FLAGS_COUNT_PER_GROUP > 32 && EVENT_FLAGS_COUNT_PER_GROUP <= 64)
    using Flags = uint64_t;
    #else
    #error "Requested number of event flags in group can't be fitted"
    #endif

    class EventFlagGroup {
    public:
        void set(Flags flags);

        void clear(Flags flags) ;

        Flags waitAny(Flags flag);

        Flags waitAll(Flags flags);

    private:
        std::mutex mtx_;
        std::condition_variable cv_;
        Flags flags_ = 0;
    };
}

#endif // EVENT_FLAG_GRP_HPP
