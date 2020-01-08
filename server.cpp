#include "server.h"

Server::Server(boost::asio::ip::address_v4 ip_address, int port)
{
    _ip_address = ip_address;
    _port = port;
}

template <class Body, class Allocator, class Send>
void Server::handle_request(
    boost::beast::string_view doc_root,
    boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> &&req,
    Send &&send){};

Server::~Server()
{
}