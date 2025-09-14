
#pragma once
#include <unordered_map>
#include "RFD.h"
#include "RemoteNode.h"

class Node{
public:
    Node(RFD rfd) : connectedRFD(rfd){
        rfd.connectedNode = this;
    }
    ~Node() = default;

    uint64_t getId() const { return connectedRFD.getId(); }
    RFD& getRFD() { return connectedRFD; }

    void registerNode(uint64_t node_id, uint64_t public_key, uint64_t modulus);

private:
    std::unordered_map<uint64_t,RemoteNode*> network;
    RFD connectedRFD;

};