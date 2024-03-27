#include <drogon/drogon.h>

#include "commons/Result.h"

int main() {
  // Set HTTP listener address and port
  drogon::app().addListener("0.0.0.0", 9999);
  drogon::app().loadConfigFile("config.json");

  auto exception_hander =
      [](const std::exception &e, const drogon::HttpRequestPtr &req,
         std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        LOG_ERROR << "====================================";
        LOG_ERROR << e.what();
        LOG_ERROR << "====================================";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(Result::Error("error")));
      };

  drogon::app().setExceptionHandler(exception_hander);

  drogon::app().run();
  return 0;
}