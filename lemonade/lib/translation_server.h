#pragma once

#include "3rd_party/Simple-WebSocket-Server/server_ws.hpp"
#include "logging.h"
#include "translator.h"

namespace lemonade {

class TranslationServer {
public:
  using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
  TranslationServer(size_t port, size_t maxModels, size_t numWorkers);

  // Start server thread
  void run();

private:
  WSServer server_;
  Translator translator_;
  Logger logger_;
};

} // namespace lemonade
