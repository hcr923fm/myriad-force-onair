#include "myriad-controller.h"

using namespace std;

MyriadInstance::MyriadInstance(string ip_address, MyriadConnectionTypes connection_type) //,
                                                                                         //boost::asio::io_context *ioc, shared_ptr<boost::asio::ip::tcp::socket> sock)
//: io_context(ioc),
// socket(sock)
{
    connType = connection_type;
    _ip_address = ip_address;
    ownershipLock = unique_lock<mutex>(ownershipMutex, defer_lock);
    io_context = make_shared<boost::asio::io_context>();
    socket = make_shared<boost::asio::ip::tcp::socket>(*io_context);
};

bool MyriadInstance::connectInstance()
{
    printf("%s:\tWaiting for lock...\n", _ip_address.c_str());
    lock_guard<mutex> l(ownershipMutex);
    printf("%s:\tResolving host...\n", _ip_address.c_str());
    unsigned short port = connType == TCP ? 6950 : 6951;

    io_context->run(ec);
    checkBoostError();

    boost::asio::ip::tcp::resolver resolver(*io_context);
    char portStr[6];
    _itoa_s(port, portStr, 10);

    printf("%s:\tConnecting socket...\n", _ip_address.c_str());
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(_ip_address), port);

    socket->connect(ep, ec);
    checkBoostError();

    // boost::asio::connect(socket, ep);

    printf("Receiving connection string...\n");
    string resp = receiveLine();

    return true;
};

string MyriadInstance::sendCommand(string command)
{
    checkBoostError();
    printf("%s:\tWaiting for comms lock...\n", this->_ip_address.c_str());
    unique_lock<mutex> lk(commsMutex);

    if (command.rfind("\n") == string::npos)
    {
        command.append("\n");
    };
    printf("%s:\t--> %s\n", _ip_address.c_str(), command.c_str());

    auto buf = boost::asio::buffer(command);
    // socket.send(buf, 0, ec);
    boost::asio::write(*socket, buf, ec);
    checkBoostError();

    lk.unlock();
    string resp = receiveLine();
    return resp;
}

string MyriadInstance::receiveLine()
{
    printf("%s:\tWaiting for comms lock...\n", this->_ip_address.c_str());
    scoped_lock<mutex> lk(commsMutex);

    boost::asio::streambuf buf;

    boost::asio::read_until(*socket, buf, "\n");
    istream response_stream(&buf);
    string response;
    getline(response_stream, response);

    // Every so often, Myriad will send the instant cart updates - discard these
    if (response.find("SET IC CURRENTITEM") != string::npos || response.find("CURRENTSETINFO") != string::npos)
    {
        // lk.~scoped_lock();
        delete &lk;
        printf("%s:\tDropping SET IC message...\n", _ip_address.c_str());
        response = receiveLine();
    }
    printf("%s:\t<-- %s\n", _ip_address.c_str(), response.c_str());
    return response;
}

string MyriadInstance::getIPAddress()
{
    return _ip_address;
}

void MyriadInstance::takeCommsLock()
{
    ownershipLock.lock();
}

void MyriadInstance::releaseCommsLock()
{
    ownershipLock.unlock();
}

inline void MyriadInstance::checkBoostError()
{
    if (ec)
    {
        printf("%s:\tERROR - Category: %s; Value: %d; Message: %s\n", _ip_address.c_str(), ec.category().name(), ec.value(), ec.message().c_str());
    }
    ec.clear();
}

MyriadInstance::~MyriadInstance()
{
    socket->shutdown(boost::asio::socket_base::shutdown_both);
    socket = nullptr;
    io_context = nullptr;
}