#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class Simple : public drogon::HttpSimpleController<Simple>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    // list path definitions here;
    PATH_ADD("/path", Post, Get);
    PATH_LIST_END
};
