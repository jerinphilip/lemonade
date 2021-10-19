#include "translation_client.h"
#include "json_interop.h"
#include "utils.h"
#include <iostream>

namespace lemonade {

void TranslationClient::run() {
  client_.on_message = [](std::shared_ptr<WsClient::Connection> connection,
                          std::shared_ptr<WsClient::InMessage> in_message) {
    std::cout << fmt::format("[{}] {}", currentTime(), in_message->string())
              << std::endl;

    // std::cout << "Client: Sending close connection" << std::endl;
    // connection->send_close(1000);
  };

  client_.on_open = [](std::shared_ptr<WsClient::Connection> connection) {
    std::cout << "Client: Opened connection" << std::endl;
  };

  client_.on_close = [](std::shared_ptr<WsClient::Connection> /*connection*/,
                        int status, const std::string & /*reason*/) {
    std::cout << "Client: Closed connection with status code " << status
              << std::endl;
  };

  // See
  // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
  // Error Codes for error code meanings
  client_.on_error = [](std::shared_ptr<WsClient::Connection> /*connection*/,
                        const SimpleWeb::error_code &ec) {
    std::cout << "Client: Error: " << ec << ", error message: " << ec.message()
              << std::endl;
  };

  // Leave start somewhere.
  std::thread listeningThread([this]() { client_.start(); });

  // Mainloop
  std::string line;
  while (std::getline(std::cin, line)) {
    client_.translate(source_, target_, line);
  }

  listeningThread.join();
}

void TranslationClient::WsClient::translate(const std::string &source,
                                            const std::string &target,
                                            const std::string &query) {
  Payload payload{source, target, query};
  std::string payloadAsString = toJSON<Payload>(payload);
  SimpleWeb::LockGuard lock(connection_mutex);
  connection->send(payloadAsString);
}

} // namespace lemonade
