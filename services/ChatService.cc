#include "ChatService.h"

/**
 * \param.
 */
void ChatService::SaveConversation(
    const std::string& cid, const std::string& title,
    std::function<void(qwen_chat::Conversation)>&& callback) {
  conversation_mapper.findBy(
      orm::Criteria("cid", orm::CompareOperator::EQ, cid),
      [this, callback_ = std::move(callback), cid_ = std::move(cid),
       title_ = std::move(title)](std::vector<qwen_chat::Conversation> result) {
        if (!result.empty()) {
          callback_(result.at(0));
        } else {
          LOG_DEBUG << "---------------------";
          LOG_DEBUG << "insert : " << title_;
          LOG_DEBUG << "---------------------";
          // 插入conversation后返回
          qwen_chat::Conversation conv;
          conv.setCid(cid_), conv.setTitle(title_);
          conversation_mapper.insert(conv, callback_,
                                     handler_impl::DbExceptionHandler);
        }
      },
      handler_impl::DbExceptionHandler);
}

/**
 * \param cid.
 * \param ai_msg.
 * \param human_msg.
 * \param callback   异步回调
 */
void ChatService::SaveMessage(
    const std::string& cid, const std::string& ai_msg,
    const std::string& human_msg,
    std::function<void(qwen_chat::History)>&& callback) {
  auto find_by = [ai_msg_ = std::move(ai_msg),
                  human_msg_ = std::move(human_msg),
                  callback_ = std::move(callback),
                  this](const std::vector<qwen_chat::Conversation> conv) {
    qwen_chat::History history;
    if (!conv.empty()) {
      history.setConvId(conv.at(0).getValueOfId());
      history.setAiMsg(ai_msg_);
      history.setHumanMsg(human_msg_);
      history.setCreateTime(trantor::Date::now());

      history_mapper.insert(history, callback_,
                            handler_impl::DbExceptionHandler);
    } else {
      callback_(history);
    }
  };
  GetConversation(cid, find_by);
  // 异步调用
  // auto find_one = [ai_msg_ = std::move(ai_msg),
  //                 human_msg_ = std::move(human_msg),
  //                 callback_ = std::move(callback),
  //                 this](const qwen_chat::Conversation conv) {
  //  qwen_chat::History history;
  //  history.setConvId(conv.getValueOfId());
  //  history.setAiMsg(ai_msg_);
  //  history.setHumanMsg(human_msg_);
  //  history.setCreateTime(trantor::Date::now());

  //  history_mapper.insert(history, callback_,
  //  handler_impl::DbExceptionHandler);
  //};
  //// 异步调用
  // conversation_mapper.findOne(
  //     orm::Criteria("cid", orm::CompareOperator::EQ, cid), find_one,
  //     handler_impl::DbExceptionHandler);
}

/**
 * \param cid.
 * \param callback response.
 */
void ChatService::GetHistotyMessage(
    const std::string& cid,
    std::function<void(std::vector<qwen_chat::History>)>&& callback) {
  auto find_by = [this, callback_ = std::move(callback)](
                     std::vector<qwen_chat::Conversation> result) {
    if (!result.empty()) {
      auto criteria = orm::Criteria("conv_id", orm::CompareOperator::EQ,
                                    result.at(0).getValueOfId());
      history_mapper.findBy(criteria, callback_,
                            handler_impl::DbExceptionHandler);
    } else {
      callback_({});
    }
  };
  GetConversation(cid, find_by);
}

/**
 * \param callback.
 */
void ChatService::GetConversation(
    const std::string& cid,
    std::function<void(std::vector<qwen_chat::Conversation>)>&& callback) {
  conversation_mapper.findBy(
      orm::Criteria("cid", orm::CompareOperator::EQ, cid), callback,
      handler_impl::DbExceptionHandler);
}
