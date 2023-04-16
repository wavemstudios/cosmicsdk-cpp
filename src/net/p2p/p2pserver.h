#ifndef P2PSERVER_H
#define P2PSERVER_H

#include <iostream>
#include <thread>
#include <future>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/lexical_cast.hpp>

#include "../../libs/BS_thread_pool_light.hpp"
#include "../../utils/utils.h"

#include "p2pbase.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace P2P {
  class ManagerBase;  // Forward declaration

  class ServerSession : public BaseSession, public std::enable_shared_from_this<ServerSession> {
    private:
      // TODO: change this to timed mutex, so that a thread doesn't lock forever.
      std::mutex writeLock_;
      beast::flat_buffer buffer_;
      beast::flat_buffer answerBuffer_;
      http::request<http::string_body> upgrade_request_;

    public:
      ServerSession(
        tcp::socket&& socket, ManagerBase& manager,
        const std::unique_ptr<BS::thread_pool_light>& threadPool
      ) : BaseSession(std::move(socket), manager, ConnectionType::SERVER, threadPool)
      {}

      void run() override;
      void stop() override;
      void on_run();
      void accept(beast::error_code ec, std::size_t bytes_transferred);
      void on_accept(beast::error_code ec);
      void read() override;
      void on_read(beast::error_code ec, std::size_t bytes_transferred) override;
      void write(const Message& message) override;
      void on_write(beast::error_code ec, std::size_t bytes_transferred) override;
      void close() override;
      void on_close(beast::error_code ec) override;

      void handleError(const std::string& func, const beast::error_code& ec);
  };

  class Server : public std::enable_shared_from_this<Server>  {
    class listener : public std::enable_shared_from_this<listener> {
      private:
        net::io_context& ioc_;
        tcp::acceptor acceptor_;
        ManagerBase& manager_;
        const std::unique_ptr<BS::thread_pool_light>& threadPool;
        void accept();
        void on_accept(beast::error_code ec, tcp::socket socket);

      public:
        listener(
          net::io_context& ioc, tcp::endpoint endpoint, ManagerBase& manager,
          const std::unique_ptr<BS::thread_pool_light>& threadPool
        ) : ioc_(ioc), acceptor_(ioc), manager_(manager), threadPool(threadPool) {
          beast::error_code ec;
          acceptor_.open(endpoint.protocol(), ec); // Open the acceptor
          if (ec) { Utils::logToDebug(Log::P2PServer, __func__, "Open Acceptor: " + ec.message()); return; }
          acceptor_.set_option(net::socket_base::reuse_address(true), ec); // Allow address reuse
          if (ec) { Utils::logToDebug(Log::P2PServer, __func__, "Set Option: " + ec.message()); return; }
          acceptor_.bind(endpoint, ec); // Bind to the server address
          if (ec) { Utils::logToDebug(Log::P2PServer, __func__, "Bind Acceptor: " + ec.message()); return; }
          acceptor_.listen(net::socket_base::max_listen_connections, ec); // Start listening
          if (ec) { Utils::logToDebug(Log::P2PServer, __func__, "Listen Acceptor: " + ec.message()); return; }
        }
        void run();
        void stop();
    };

    private:
      ManagerBase& manager_;
      const std::unique_ptr<BS::thread_pool_light>& threadPool;
      net::io_context ioc;
      std::shared_ptr<listener> listener_;
      const boost::asio::ip::address address;
      const unsigned short port;
      const unsigned int threads;
      std::future<bool> runFuture_;
      bool run();

    public:
      Server(
        boost::asio::ip::address address, unsigned short port,
        unsigned int threads, ManagerBase& manager,
        const std::unique_ptr<BS::thread_pool_light>& threadPool
      ) : address(address), port(port), threads(threads), manager_(manager), threadPool(threadPool)
      {};

      bool start();
      void stop();
      bool isRunning() const { return runFuture_.valid(); }
  };
};

#endif