#pragma once

#include <json/json.h>

class Result {
 public:
  static Json::Value Ok(const std::string& data);
  static Json::Value Ok(const Json::Value& json);
  static Json::Value Error(const std::string& data);
  static Json::Value Construct(int status, const std::string& data);
};
