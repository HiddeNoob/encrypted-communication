#include "Node.h"
#include "RemoteNode.h"

void Node::registerNode(uint64_t node_id, uint64_t public_key, uint64_t modulus) {
    if (network.find(node_id) == network.end()) {
        RemoteNode* newNode = new RemoteNode(node_id,Key(public_key, modulus));
        network[node_id] = newNode;
        printf("[Node %llx] Registered new node: %llx\n", this->nodeId, node_id);
    } else {
        printf("[Node %llx] Node %llx already registered\n", this->nodeId, node_id);
    }
}