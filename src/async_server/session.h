#include <memory>
#include <iostream>
#include <boost/asio.hpp>

class session
    : public std::enable_shared_from_this<session>
{
public:
  session(boost::asio::ip::tcp::socket socket, int connection_id)
      : socket_(std::move(socket)), connection_id_(connection_id)
  {
    std::cout << "connection established: " << connection_id_ << std::endl;
  }

  ~session()
  {
    std::cout << "connection lost: " << connection_id_ << std::endl;
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            [this, self](boost::system::error_code ec, std::size_t length)
                            {
                              if (!ec)
                              {
                                do_write(length);
                              }
                            });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
                             {
                               if (!ec)
                               {
                                 do_read();
                               }
                             });
  }

  boost::asio::ip::tcp::socket socket_;
  enum
  {
    max_length = 1024
  };
  char data_[max_length];
  int connection_id_;
};