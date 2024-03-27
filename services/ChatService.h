#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Mapper.h>

#include "commons/ExceptionHandler.h"
#include "models/Conversation.h"
#include "models/History.h"

using namespace drogon;
using namespace drogon_model;

class ChatService {
 private:
  ChatService()
      : conversation_mapper(drogon::app().getDbClient()),
        history_mapper(drogon::app().getDbClient()) {}
  ChatService(const ChatService&) = delete;
  ChatService(ChatService&&) = delete;

 public:
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
