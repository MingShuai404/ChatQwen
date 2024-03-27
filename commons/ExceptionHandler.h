
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Exception.h>

using namespace drogon;

namespace handler_impl {
void DbExceptionHandler(const orm::DrogonDbException& e);
}  // namespace handler_impl
