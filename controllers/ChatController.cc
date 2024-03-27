#include "ChatController.h"

/**
 * \code 该路径用于生成每个Conversation ID.
 */
void ChatController::GenerateId(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const {
  // 生成uuid
  auto res = Result::Ok(drogon::utils::getUuid());
  callback(HttpResponse::newHttpJsonResponse(res));
}

/**
 * \code 该路径用于生成文本.
 */
void ChatController::GenerateText(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, std::string &&cid,
    std::string &&prompt) const {
  using namespace drogon_model;

  auto &chat_service = ChatService::GetChatService();

  chat_service.SaveConversation(
      cid, prompt,
      [&chat_service, cid, callback = std::move(callback),
       prompt](qwen_chat::Conversation conv) {
        LOG_DEBUG << "Save Conversation. cid: " << cid << " title: " << prompt;
        // 保存conversation
        chat_service.GetHistotyMessage(
            // 获取历史记录
            cid, [&chat_service, callback = std::move(callback), cid,
                  prompt = std::move(prompt)](
                     std::vector<qwen_chat::History> history_res) {
              LOG_DEBUG << "Get History Message. cid: " << cid;
              // 请求qwen api
              Json::Value input;
              Json::Value human;
              Json::Value ai;
              if (!history_res.empty()) {
                auto end_record = history_res.back();
                ai["role"] = "system";
                ai["content"] = end_record.getValueOfAiMsg() + ".";
                input["messages"].append(ai);
              }
              human["role"] = "user";
              human["content"] = prompt;
              input["messages"].append(human);
              LOG_DEBUG << input.toStyledString();
              // 获取api响应并响应客户端
              DashScope::Generate(
                  "qwen-turbo", input,
                  [callback = std::move(callback), cid = std::move(cid),
                   prompt = std::move(prompt)](ReqResult,
                                               const HttpResponsePtr &resp) {
                    LOG_DEBUG << "Generate Message.";
                    auto msg = resp->getJsonObject();
                    std::string text;
                    if (msg) {
                      LOG_DEBUG << msg->get("output", Json::Value{})
                                       .get("text", Json::Value{})
                                       .asString();
                      text = (*msg)["output"]["text"].asString();
                      // save_message
                      ChatService::GetChatService().SaveMessage(
                          cid, text, prompt, [](qwen_chat::History) {
                            LOG_DEBUG << "History Message.";
                          });
                    }

                    std::string::size_type pos;
                    while ((pos = text.find("\n")) != text.npos) {
                      text.replace(pos, 1, "[ENTRY]");
                    }

                    auto async_resp = HttpResponse::newAsyncStreamResponse(
                        [text = std::move(text)](ResponseStreamPtr stream) {
                          LOG_DEBUG << "Async Response.";
                          stream->send("data: " + text + "\n\n");
                          stream->send("data: [DONE]\n\n");
                          stream->close();
                        });
                    async_resp->setContentTypeString("text/event-stream");
                    callback(async_resp);
                  });
            });
      });
}

/**
 * \code 该路径用于重新生成文本.
 */
void ChatController::RepeatGenerateText(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string &&cid) const {
  using namespace drogon_model;
  auto &chat_service = ChatService::GetChatService();
  chat_service.GetHistotyMessage(
      cid, [&chat_service, callback_ = std::move(callback),
            cid](std::vector<qwen_chat::History> history_res) {
        if (history_res.empty()) callback_({});
        Json::Value input;
        Json::Value human;
        Json::Value ai;
        auto &last_record = history_res.back();
        auto last_prompt = last_record.getValueOfHumanMsg();
        // 找到上一个问题
        auto pre_question =
            std::find_if(history_res.rbegin(), history_res.rend(),
                         [last_prompt](const qwen_chat::History &h) {
                           return last_prompt != h.getValueOfHumanMsg();
                         });
        // 构造api json参数
        if (pre_question != history_res.rend()) {
          ai["role"] = "system";
          ai["content"] = pre_question->getValueOfAiMsg() + ".";
          input["messages"].append(ai);
        }
        human["role"] = "user";
        human["content"] = last_prompt;
        input["messages"].append(human);
        LOG_DEBUG << input.toStyledString();

        auto async_generate =
            [callback_ = std::move(callback_), cid_ = std::move(cid),
             last_prompt_ = std::move(last_prompt)](
                ReqResult result, const HttpResponsePtr &resp) {
              if (result != ReqResult::Ok) callback_({});
              LOG_DEBUG << "Generate Message.";
              auto msg = resp->getJsonObject();
              std::string text;
              if (msg) {
                LOG_DEBUG << msg->get("output", Json::Value{})
                                 .get("text", Json::Value{})
                                 .asString();
                text = (*msg)["output"]["text"].asString();
                // save_message
                ChatService::GetChatService().SaveMessage(
                    cid_, text, last_prompt_, [](qwen_chat::History) {
                      LOG_DEBUG << "History Message.";
                    });
              }
              std::string::size_type pos;
              while ((pos = text.find("\n")) != text.npos) {
                text.replace(pos, 1, "[ENTRY]");
              }

              auto async_resp = HttpResponse::newAsyncStreamResponse(
                  [text = std::move(text)](ResponseStreamPtr stream) {
                    LOG_DEBUG << "Async Response.";
                    stream->send("data: " + text + "\n\n");
                    stream->send("data: [DONE]\n\n");
                    stream->close();
                  });
              async_resp->setContentTypeString("text/event-stream");
              callback_(async_resp);
            };

        DashScope::Generate("qwen-turbo", input, async_generate);
      });
}

/**
 * \code 该路径用于停止生成文本.
 */
void ChatController::StopGenerateText(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string &&id) const {
  callback(HttpResponse::newHttpJsonResponse(Result::Ok(std::string(""))));
}

/**
 * \code 该路径用于获取各个Conversation的历史记录.
 */
void ChatController::GetHistory(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string &&cid) const {
  using namespace drogon_model;

  auto &chat_service = ChatService::GetChatService();

  auto async_callback = [callback_ = std::move(callback),
                         cid](std::vector<qwen_chat::History> query_result) {
    if (query_result.empty()) callback_({});

    Json::Value data;
    // question  answer
    std::map<std::string, Json::Value> mapper;
    for (const auto &record : query_result) {
      auto &ref = mapper[record.getValueOfHumanMsg()]["speeches"];
      ref.append(record.getValueOfAiMsg());
    }
    for (const auto &pair : mapper) {
      Json::Value human_msg;
      Json::Value ai_msg;
      human_msg["speech"] = pair.first;
      human_msg["speaker"] = "human";
      human_msg["createTime"] = trantor::Date::now().toDbString();
      ai_msg["speaker"] = "ai";
      ai_msg["createTime"] = trantor::Date::now().toDbString();
      data["convs"].append(human_msg);
      const auto &ai_msgs = pair.second;

      for (const auto &msg : pair.second) {
        ai_msg["speeches"] = msg;
        int idx = 0;
        while (idx < msg.size()) ai_msg["suitable"].append(idx++);
      }
      data["convs"].append(ai_msg);
    }

    data["_id"] = 0;
    data["title"] = "test";
    LOG_DEBUG << data.toStyledString();
    callback_(HttpResponse::newHttpJsonResponse(Result::Ok(data)));
  };

  chat_service.GetHistotyMessage(cid, async_callback);
}

/**
 * \code 该路径用于返回标题.
 */
void ChatController::GetConversationTitle(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string &&cid) const {
  using namespace drogon_model;

  auto &chat_service = ChatService::GetChatService();

  auto async_get_conv = [callback_ = std::move(callback)](
                            std::vector<qwen_chat::Conversation> convs_res) {
    auto async_resp = HttpResponse::newAsyncStreamResponse(
        [convs_res_ = std::move(convs_res)](ResponseStreamPtr stream) {
          if (!convs_res_.empty()) {
            std::string_view title = convs_res_.at(0).getValueOfTitle();
            LOG_DEBUG << "---------------------";
            LOG_DEBUG << title.data();
            LOG_DEBUG << "---------------------";
            stream->send("data: ");
            stream->send(title.data());
            stream->send("\n\n");
            stream->send("data: [DONE]\n\n");
            stream->close();
          } else {
            stream->send("data: [DONE]\n\n");
            stream->close();
          }
        });
    async_resp->setContentTypeString("text/event-stream");
    callback_(async_resp);
  };

  chat_service.GetConversation(cid, async_get_conv);
}