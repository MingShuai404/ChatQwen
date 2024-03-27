#include "Result.h"

Json::Value Result::Ok(const std::string& data) {
  Json::Value res;
  res["msg"] = 200;
  res["data"] = data;
  return res;
}

Json::Value Result::Ok(const Json::Value& json) {
  Json::Value res;
  res["msg"] = 200;
  res["data"] = json;
  return res;
}

Json::Value Result::Error(const std::string& data) {
  Json::Value res;
  res["msg"] = -1;
  res["data"] = data;
  return res;
}

Json::Value Result::Construct(int status, const std::string& data) {
  Json::Value res;
  res["msg"] = status;
  res["data"] = data;
  return res;
}
