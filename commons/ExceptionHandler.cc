#include "ExceptionHandler.h"

using namespace drogon;

namespace handler_impl {
void DbExceptionHandler(const orm::DrogonDbException& e) {
  LOG_ERROR << "====================================";
  LOG_ERROR << e.base().what();
  LOG_ERROR << "====================================";
}
}  // namespace handler_impl
