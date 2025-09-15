#include "transmit_handler.hpp"

#include "network.hpp"
#include "log.hpp"
#include "sync_objects.hpp"

#include "release_notes.pb.h"

// ============ TRANSMIT LISTS TASK-CORE
TaskCore gTransmitListsCore = [](ThreadTask* task) {
    auto pmsg = TransmitTaskMQ.waitForMessage();

    auto pRN = static_cast<geo_release::ReleaseNotes*>(pmsg);
    std::unique_ptr<geo_release::ReleaseNotes> uRN(pRN);

    auto releaseNotes = std::move(*uRN);

    // ...
};
// ============
