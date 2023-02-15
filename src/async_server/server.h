#include <memory>
#include <boost/asio.hpp>

#include "session.h"

class server
{
public:
  server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    connections_count_(0)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
          if (!ec)
          { 
            ++connections_count_;
            std::make_shared<session>(std::move(socket), connections_count_)->start();
          }

          do_accept();
        });
  }

  boost::asio::ip::tcp::acceptor acceptor_;
  size_t connections_count_;
};