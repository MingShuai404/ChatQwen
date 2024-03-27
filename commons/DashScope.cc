#include "DashScope.h"

// 不开启SSE
void DashScope::Generate(
    const std::string& model, const Json::Value& input,
    std::function<void(ReqResult, const HttpResponsePtr&)>&& callback) {
  Json::Value data;
  data["input"] = input;
  data["model"] = model;
  LOG_DEBUG << data.toStyledString();

  auto req = HttpRequest::newHttpJsonRequest(data);
  req->setMethod(Post);
  req->addHeader("Authorization", kKey.data());
  req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
  req->setPath(kGenerationPath.data());

  kClient->sendRequest(req, callback);
  
}

/**
 * @brief 补充token信息.
 */
std::string_view DashScope::kKey = "sk-33b459f6132b45d98678e4f1a41a14d9"; // token已废弃
std::string_view DashScope::kHost = "https://dashscope.aliyuncs.com";
std::string_view DashScope::kGenerationPath =
    "/api/v1/services/aigc/text-generation/generation";

HttpClientPtr DashScope::kClient = HttpClient::newHttpClient(
    "https://dashscope.aliyuncs.com", drogon::app().getLoop());
