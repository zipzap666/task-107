#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
//#include <chrono>

using boost::asio::ip::tcp;

enum
{
  max_length = 1024
};

int main(int argc, char *argv[])
{
  if (argc != 5)
  {
    std::cerr << "Usage: client <host> <port> <thread count> <loop write+read count>\n";
    return 1;
  }

  std::vector<std::thread> threads;
  size_t thread_count = std::stoi(argv[3]);
  threads.reserve(thread_count);

  for (int i = 0; i < thread_count; i++)
  {
    threads.push_back(std::thread([&argv, i]()
    {
      //std::this_thread::sleep_for(std::chrono::seconds(i));
      try{
        boost::asio::io_context io_context;

        int loop_count = std::stoi(argv[4]);
        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

        std::cout << "Client id - " << i+1 << " connected." << std::endl;

        //std::this_thread::sleep_for(std::chrono::seconds(10 + std::rand() % 10));
        
        while(loop_count--)
        {
          std::string request("any msg");
          boost::asio::write(s, boost::asio::buffer(request.c_str(), request.size()));

          char reply[1024];
          boost::asio::read(s,boost::asio::buffer(reply, request.size()));
        }

        std::cout << "Client id - " << i+1 << " disconnected." << std::endl;
      }
      catch(std::exception &e)
      {
        std::cerr << "Exception: " << e.what() << "\n";
      }
    }));
  }

  for (int i = 0; i < thread_count; i++)
  {
    threads[i].join();
  }

  return 0;
}