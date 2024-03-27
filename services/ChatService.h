#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Mapper.h>

#include "commons/ExceptionHandler.h"
#include "models/Conversation.h"
#include "models/History.h"

using namespace drogon;
using namespace drogon_model;

/**
 * @brief 由于持久层采用的均为异步接口 因此Service方法也均为异步方法.
 */
class ChatService {
 private:
  ChatService()
      : conversation_mapper(drogon::app().getDbClient()),
        history_mapper(drogon::app().getDbClient()) {}
  ChatService(const ChatService&) = delete;
  ChatService(ChatService&&) = delete;

 public:
  /**
   * @brief 几个服务都采用异步回调.
   * @brief 由于不太清楚框架设计 因此调用的orm方法均为findBy
   * 通过结果std::vector的非空判断保证异常安全
   */
  void SaveConversation(const std::string& cid, const std::string& title,
                        std::function<void(qwen_chat::Conversation)>&&);
  void SaveMessage(const std::string& cid, const std::string& ai_msg,
                   const std::string& human_msg,
                   std::function<void(qwen_chat::History)>&& callback);
  void GetHistotyMessage(
      const std::string& cid,
      std::function<void(std::vector<qwen_chat::History>)>&&);
  void GetConversation(const std::string& cid,
      std::function<void(std::vector<qwen_chat::Conversation>)>&& callback);

  static ChatService& GetChatService() {
    static ChatService obj;
    return obj;
  }

 private:
  orm::Mapper<qwen_chat::Conversation> conversation_mapper;
  orm::Mapper<qwen_chat::History> history_mapper;
};
