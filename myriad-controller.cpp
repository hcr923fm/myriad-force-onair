#include "myriad-controller.h"

MyriadInstance::MyriadInstance(string ip_address = "127.0.0.1", MyriadConnectionTypes connection_type = TCP)
{
    connType = connection_type;
    _ip_address = ip_address;
    sock = INVALID_SOCKET;
};

bool MyriadInstance::connectInstance()
{
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0)
    {
        printf("%s:\tWSAStartup failed: %d\n", _ip_address.c_str(), ret);
        return false;
    }

    // Set up information about the address, and store it
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_family = SOCK_STREAM;
    hints.ai_protocol = connType == TCP ? IPPROTO_TCP : IPPROTO_UDP;

    // Try and parse the address and port
    char *port_str = connType == TCP ? "6950" : "6951";
    ret = getaddrinfo(_ip_address.c_str(), port_str, &hints, &result);
    if (ret != 0)
    {
        printf("%s:\tgetaddrinfo failed: %d\n", _ip_address.c_str(), ret);
        WSACleanup();
        return false;
    }

    // Create the socket with the parse address information
    ptr = result;
    sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (sock == INVALID_SOCKET)
    {
        printf("%s:\tError at socket(): %ld\n", _ip_address.c_str(), WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // And try and connect
    ret = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (ret = SOCKET_ERROR)
    {
        // Couldn't connect
        closesocket(sock);
        sock = INVALID_SOCKET;
    }

    freeaddrinfo(result);
    if (sock == INVALID_SOCKET)
    {
        printf("%s:\tUnable to connect to server!\n", _ip_address.c_str());
        WSACleanup();
        return false;
    }

    string resp = receiveLine();
    printf("%s:\tRecv:\t%s\n", _ip_address.c_str(), resp.c_str());

    return true;
};

string MyriadInstance::sendCommand(string command)
{
    constexpr int bufferLength = 1024;
    int ret = send(sock, command.c_str(), command.length(), 0);
    if (ret == SOCKET_ERROR)
    {
        printf("%s:\tCould not send command: %s (%d)\n", _ip_address.c_str(), command.c_str(), WSAGetLastError());
        return nullptr;
    }
    printf("%s:\tSent:\t%s\n", _ip_address.c_str(), command.c_str());
    return receiveLine();
}

string MyriadInstance::receiveLine()
{
    string response = "";
    char recv_buf[1];
    while (recv_buf != "\n")
    {
        recv(sock, recv_buf, 1, 0);
        response += recv_buf;
    }

    // Every so often, Myriad will send the instant cart updates - discard these
    if (response.find("SET IC CURRENTITEM") != string::npos)
    {
        printf("%s:\tDropping SET IC message...\n", _ip_address.c_str());
        response = receiveLine();
    }
    return response;
}

string MyriadInstance::getIPAddress()
{
    return _ip_address;
}

MyriadInstance::~MyriadInstance()
{
    if (sock != INVALID_SOCKET)
    {
        // TODO: Socket is connected, so disconnect
        int ret = shutdown(sock, SD_SEND);
        if (ret == SOCKET_ERROR)
        {
            printf("%s:\tFailed to disconnect! %d\n", WSAGetLastError());
            WSACleanup();
            return;
        }
        else
        {
            closesocket(sock);
            WSACleanup();
        }
    }
}