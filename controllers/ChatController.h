#pragma once

#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>

#include <map>
#include <string>

#include "commons/DashScope.h"
#include "commons/Result.h"
#include "models/Conversation.h"
#include "models/History.h"
#include "services/ChatService.h"

/*
  +--------------------------------------------------------------------+
  |   id   |   cid   | chat_msg | user_msg | create_time | update_time |
  | bigint | bigint  | varchar  | varchar  | datetime    | datetime    |
  +--------------------------------------------------------------------+

  +----------------------------+
  |   id   | conv_id |  title  |
  +----------------------------+
  */

using namespace drogon;

class ChatController : public drogon::HttpController<ChatController> {
 public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(ChatController::GenerateId, "/api/generate/id", Post);
  ADD_METHOD_TO(ChatController::GenerateText,
                "/api/chat/{1:id}?prompt={2:prompt}", Get);
  ADD_METHOD_TO(ChatController::RepeatGenerateText, "/api/chat/repeat/{1:id}",
                Get);
  ADD_METHOD_TO(ChatController::StopGenerateText, "/api/stop/chat/{1:id}", Put);
  ADD_METHOD_TO(ChatController::GetHistory, "/api/conv/{1:id}", Get);
  ADD_METHOD_TO(ChatController::GetConversationTitle, "/api/chat/title/{1:id}",
                Get);
  METHOD_LIST_END
  // 生成uuid作为标识
  void GenerateId(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) const;

  // 生成文本
  void GenerateText(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    std::string &&id, std::string &&prompt) const;

  // 生成文本
  void RepeatGenerateText(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback,
      std::string &&id) const;

  // 停止生成文本
  void StopGenerateText(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback,
                        std::string &&id) const;

  // 生成其他会话
  void GetHistory(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  std::string &&id) const;

  // 生成会话标题
  void GetConversationTitle(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback,
      std::string &&id) const;
};
