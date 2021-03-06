#include "HostConnected.h"
#include <iostream>

HostConnected::HostConnected(const uint8_t * const publickey, const uint64_t &local_sequence_number_validated,
                             const uint64_t &remote_sequence_number, const sockaddr_in6 &si_other) :
    P2PPeer(publickey,local_sequence_number_validated,remote_sequence_number,si_other)
{
}

HostConnected::~HostConnected()
{
    std::cout << "\e[1m\e[91mHostConnected::~HostConnected()\e[0m" << std::endl;
}

bool HostConnected::parseData(const uint8_t * const data, const uint16_t &size)
{
    std::cout << "From: " << toString() << ": " << std::string(reinterpret_cast<const char *>(data),size) << std::endl;
    return true;
}
