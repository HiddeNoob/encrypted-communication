
#pragma once
#include <unordered_map>
#include "RFD.h"
#include "RemoteNode.h"

class Node{
public:
    Node(RFD rfd) : connectedRFD(rfd), nodeId(rfd.getId()){
        rfd.connectedNode = this;
    }
    ~Node() = default;

    int getId() const { return nodeId; }
    RFD& getRFD() { return connectedRFD; }

    void registerNode(uint64_t node_id, uint64_t public_key, uint64_t modulus);

private:
    const uint64_t nodeId;
    std::unordered_map<uint64_t,RemoteNode*> network;
    RFD connectedRFD;

};