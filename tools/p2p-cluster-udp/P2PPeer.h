#ifndef P2PPEER_H
#define P2PPEER_H

#include <nettle/eddsa.h>
#include <netinet/in.h>
#include <stdint.h>
#include <vector>
#include <string>

class P2PPeer
{
public:
    P2PPeer(const uint8_t * const publickey, const uint64_t &local_sequence_number_validated,
            const uint64_t &remote_sequence_number,const sockaddr_in6 &si_other);
    virtual ~P2PPeer();
public:
    //P2PPeer();
    void emitAck();
    bool sendData(const uint8_t * const data, const uint16_t &size);
    bool sendDataWithMessageType(const uint8_t * const data, const uint16_t &size);
    bool sendRawDataWithoutPutInQueue(const uint8_t * const data, const uint16_t &size);
    static void sign(uint8_t *msg, const size_t &length);
    bool discardBuffer(const uint64_t &ackNumber);
    const uint8_t *getPublickey() const;
    const uint64_t &get_remote_sequence_number() const;
    void incremente_remote_sequence_number();
    static void incremented_sequence_number(uint64_t &number);
    std::string toString() const;
    static std::string toString(const sockaddr_in6 &si_other, const std::string &separator);
    virtual bool parseData(const uint8_t * const data, const uint16_t &size) = 0;
private:
    uint8_t publickey[ED25519_KEY_SIZE];
    uint64_t local_sequence_number_validated;
    uint64_t remote_sequence_number;
    const sockaddr_in6 si_other;

    std::vector<std::string> dataToSend;

    //[8(current sequence number)+8(acknowledgement number)+1(request type)+ED25519_SIGNATURE_SIZE(node)]
    static char buffer[1496];
};

#endif // P2PPEER_H
