#ifndef __MYRIAD_CONTROLLER_H__
#define __MYRIAD_CONTROLLER_H__

#include <string>
#include <sstream>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace std;

enum MyriadConnectionTypes
{
    TCP = 1,
    UDP = 2
};

class MyriadInstance
{
public:
    MyriadInstance(string ip_address, MyriadConnectionTypes connection_type); //,
                                                                              //boost::asio::io_context *ioc = nullptr, shared_ptr<boost::asio::ip::tcp::socket> sock = nullptr);
    virtual ~MyriadInstance();

    bool connectInstance();
    string sendCommand(string command);
    string MyriadInstance::receiveLine();
    string getIPAddress();
    void takeCommsLock();
    void releaseCommsLock();

private:
    string _ip_address;
    MyriadConnectionTypes connType;
    shared_ptr<boost::asio::ip::tcp::socket> socket;
    shared_ptr<boost::asio::io_context> io_context;
    mutex commsMutex;
    mutex ownershipMutex;
    unique_lock<mutex> ownershipLock;
    boost::system::error_code ec;

    inline void checkBoostError();
};

#endif