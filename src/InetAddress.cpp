#include "InetAddress.hpp"

InetAddress::InetAddress(const std::string& _ip, uint16_t _port)
{
    _M_addr.sin_family = AF_INET;
    _M_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    _M_addr.sin_port = htons(_port);
}

InetAddress::InetAddress(const sockaddr_in _addr):_M_addr(_addr) {

}

const char* InetAddress::ip() const {
    return inet_ntoa(_M_addr.sin_addr);
}

uint16_t InetAddress::port() const {
    return ntohs(_M_addr.sin_port);
}

const sockaddr* InetAddress::addr() const {
    return (sockaddr*)&_M_addr;
}

InetAddress::~InetAddress()
{

}