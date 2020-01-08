#ifndef __MFO__SERVER__H__
#define __MFO__SERVER__H__

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>

class Server
{
public:
    Server(boost::asio::ip::address_v4 ip_address, int port);
    virtual ~Server();

private:
    int _port;
    boost::asio::ip::address_v4 _ip_address;

    template <class Body, class Allocator, class Send>
    void handle_request(
        boost::beast::string_view doc_root,
        boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req,
        Send &&send);
};

#endif // __MFO__SERVER__H__