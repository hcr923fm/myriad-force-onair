#ifndef __MYRIAD_CONTROLLER_H__
#define __MYRIAD_CONTROLLER_H__

#include <string>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
// #pragma comment(lib, "Ws2_32.lib")

using namespace std;

enum MyriadConnectionTypes
{
    TCP = 1,
    UDP = 2
};

class MyriadInstance
{
public:
    MyriadInstance(string ip_address, MyriadConnectionTypes connection_type);
    virtual ~MyriadInstance();

    bool connectInstance();
    string sendCommand(string command);
    string MyriadInstance::receiveLine();
    string getIPAddress();

private:
    WSADATA wsaData;
    string _ip_address;
    MyriadConnectionTypes connType;

    SOCKET sock;
};

#endif