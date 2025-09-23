#pragma once
#include "Key.h"

class RemoteNode{
public:
    RemoteNode(uint64_t id, Key public_key) : nodeId(id), public_key(public_key) {}

    int getId() const { return nodeId; }
    Key getPublicKey() const { return public_key; }

private:
    const uint64_t nodeId;
    Key public_key;
};