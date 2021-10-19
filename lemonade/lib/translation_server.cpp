#include "translation_server.h"
#include "data.h"
#include "json_interop.h"
#include "logging.h"
#include "utils.h"

namespace lemonade {

TranslationServer::TranslationServer(size_t port, size_t maxModels,
                                     size_t numWorkers)
    : translator_(maxModels, numWorkers), logger_("server") {
  server_.config.port = port;

  auto &endpoint = server_.endpoint["^/?$"];

  endpoint.on_message = [this](std::shared_ptr<WSServer::Connection> connection,
                               std::shared_ptr<WSServer::InMessage> message) {
    // Get input text
    std::string payloadAsString = message->string();
    // std::cerr << "Message: " << payloadAsString << std::endl;
    Payload payload;
    fromJSON<Payload>(payloadAsString, payload);

    logger_.log(fmt::format("[{} -> {}] {}", payload.source, payload.target,
                            payload.query));

    auto callback = [connection, this](marian::bergamot::Response &&response) {
      std::string outputText = response.target.text;

      auto sendStream = std::make_shared<WSServer::OutMessage>();
      // Translate
      // timer::Timer timer;
      *sendStream << outputText;

      // Send translation back
      connection->send(sendStream, [this](const SimpleWeb::error_code &error) {
        if (error)
          logger_.log(fmt::format("Error sending message: ({}) {}\n",
                                  error.value(), error.message()));
      });
    };

    translator_.translate(payload.query, payload.source, payload.target,
                          callback);
  };

  // Error Codes for error code meanings
  // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html
  endpoint.on_error =
      [this](std::shared_ptr<WSServer::Connection> /*connection*/,
             const SimpleWeb::error_code &error) {
        logger_.log(fmt::format("Connection error: ({}) {}\n", error.value(),
                                error.message()));
      };
}

// Start server thread
void TranslationServer::run() {
  server_.start([this](unsigned short port) {
    logger_.log(fmt::format("TranslationServer is listening on port {}", port));
  });
}

} // namespace lemonade
