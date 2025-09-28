#include "SignedMessagePacket.h"
#include "AckPacket.h"


// SignedMessagePacket
void SignedMessagePacket::serialize(uint8_t* buffer) const {
    Packet::serialize(buffer);
    size_t offset = Packet::getPayloadSize();
    memcpy(buffer + offset, &target_id, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(buffer + offset, payload, sizeof(payload));
    offset += sizeof(payload);
    memcpy(buffer + offset, &signature, sizeof(signature));
}

bool SignedMessagePacket::deserialize(const uint8_t* buffer, uint16_t length) {
    if (length < getPayloadSize()) return false;

    Packet::deserialize(buffer, length);
    size_t offset = Packet::getPayloadSize();
    memcpy(&target_id, buffer + offset, sizeof(target_id));
    offset += sizeof(target_id);
    memcpy(payload, buffer + offset, sizeof(payload));
    offset += sizeof(payload);
    memcpy(&signature, buffer + offset, sizeof(signature));
    return true;
}

void SignedMessagePacket::handle(Node& node_instance) {
    printf("[SignedMessagePacket] Received message from: %llx to: %llx\n", 
           sender_id, target_id);

    if (target_id == node_instance.getId()) {
        printf("[SignedMessagePacket] Message for me!");

        rsa_data* data = node_instance.getRFD()
                                .getCryptionService()
                                .getPrivateKey()
                                ->sign(
                                    (void*)payload,
                                    sizeof(payload) / sizeof(payload[0]),
                                    sizeof(rsa_data)
                                );

        printf("\nDecrypted data: ");
        uint8_t decryptedData[sizeof(payload) / sizeof(payload[0])] = {0};
        for (size_t i = 0; *((uint8_t*)(data + i)) ; i++) {
            decryptedData[i] = *((uint8_t*)(data + i)); 
            printf("%c", decryptedData[i]); // sifrelenen veri char tipindeydi
        }
        printf("\n");

        const RemoteNode* sender_node = node_instance.getNode(this->getSenderId());
        
        if(checkSignature(decryptedData,sender_node)){
            printf("[SignedMessagePacket] Signature verified successfully.\n");
        } else {
            printf("[SignedMessagePacket] Signature verification failed!\n");
        }

        // ACK gönder
        auto ack = std::make_unique<AckPacket>(node_instance.getId(), sender_id, PACKET_SIGNED_MESSAGE);
        ack->send(node_instance.getRFD().getUart());
    } else {
        printf("[SignedMessagePacket] Message not for me, ignoring\n");
    }
}

// 
void SignedMessagePacket::setPayload(const void* data, size_t len) {
    if(len > sizeof(payload)) {
        printf("[SignedMessagePacket] Warning: Payload length %zu exceeds buffer %zu, truncating.\n", len, sizeof(payload));
        len = sizeof(payload);
    }
    memset(payload, 0, sizeof(payload));
    memcpy(payload, data, len);
}

void SignedMessagePacket::setSignature(rsa_data sig) {
    signature = sig;
}

bool SignedMessagePacket::checkSignature(const uint8_t* decrypted,const RemoteNode* senderNode){
    if(!senderNode){
        printf("[SignedMessagePacket] ERROR can not find senderNode in network hashmap\n");
        return false;
    }
    
    // Find message length by looking for null terminator
    size_t message_length = 0;
    for(size_t i = 0; i < sizeof(payload)/sizeof(payload[0]) && decrypted[i] != 0; i++) {
        message_length++;
    }
    
    if(message_length == 0) {
        printf("[SignedMessagePacket] Empty message, signature verification failed\n");
        return false;
    }
    
    // Hash the entire decrypted message 
    uint32_t expectedHash;
    FastMath::myHash(decrypted, &expectedHash, message_length);
    
    // Reduce hash to fit within RSA modulus
    rsa_data expectedReducedHash = expectedHash % senderNode->getPublicKey().getBase();
    
    // The sender signs a single hash value
    const rsa_data messageSignature = signature;
    
    // Decrypt the signature using sender's public key to get the original hash
    rsa_data* recoveredHash = senderNode->getPublicKey().sign((void*)&messageSignature, 1, sizeof(messageSignature));



    bool verified = (*recoveredHash == expectedReducedHash);
    
    
    delete[] recoveredHash;
    
    if(!verified) {
        printf("[SignedMessagePacket] Signature verification failed: hash mismatch\n");
    }
    
    return verified;
}