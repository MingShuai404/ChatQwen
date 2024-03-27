#pragma once
#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/HttpClient.h>
#include <drogon/WebSocketClient.h>

#include <string>

#include "DashScope.h"
#include "Result.h"

using namespace drogon;

class DashScope {
 public:
  static void Generate(
      const std::string& model, const Json::Value& input,
      std::function<void(ReqResult, const HttpResponsePtr&)>&&);

  static void GenerateAsync(
      const std::string& model, const std::string& prompt,
      std::function<void(ReqResult, const HttpResponsePtr&)>&&);

 private:
  static std::string_view kKey;
  static std::string_view kHost;
  static std::string_view kGenerationPath;
  static HttpClientPtr kClient;
  static WebSocketClientPtr kWebSockClient;
};
