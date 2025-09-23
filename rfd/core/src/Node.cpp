#include "Node.h"
#include "RemoteNode.h"

void Node::registerNode(uint64_t node_id, uint64_t public_key, uint64_t modulus) {
    if (network.find(node_id) == network.end()) {
        RemoteNode* newNode = new RemoteNode(node_id,Key(public_key, modulus));
        network[node_id] = newNode;
    }
}

RemoteNode* Node::getNode(uint64_t node_id) const {
    auto it = network.find(node_id);
    if (it != network.end()) {
        return it->second;
    }
    return nullptr;
}

size_t Node::getRegisteredNodeCount() const {
    return network.size();
}

const std::unordered_map<uint64_t, RemoteNode*>& Node::getNetwork() const {
    return network;
}