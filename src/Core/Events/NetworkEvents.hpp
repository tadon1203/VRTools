#pragma once
#include "IEvent.hpp"
#include "SDK/Game/Photon/EventData.hpp"
#include "SDK/Game/Photon/SendOptions.hpp"
#include "SDK/Il2Cpp/Structs.hpp"

// Fired when receiving an event
struct PhotonEventEvent : public ICancellableEvent {
    Photon::EventData* eventData;

    explicit PhotonEventEvent(Photon::EventData* data)
        : eventData(data) {}
};

// Fired when sending an event
struct RaiseEventEvent : public ICancellableEvent {
    uint8_t code;
    Il2CppObject* content;
    void* raiseEventOptions;
    Photon::SendOptions sendOptions;

    RaiseEventEvent(uint8_t c, Il2CppObject* o, void* r, Photon::SendOptions s)
        : code(c)
        , content(o)
        , raiseEventOptions(r)
        , sendOptions(s) {}
};
