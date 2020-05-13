#include "logger.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace toy {

std::string ErrorType_to_string(ErrorType type) {
  switch (type) {
    case ErrorType::LEXING:
      return "LexingError: ";
    case ErrorType::SYNTAX:
      return "SyntaxError: ";
    case ErrorType::SEMANTIC:
      return "SemanticError: ";
    default:
      throw std::invalid_argument("Invalid TokenType");
  }
}

std::string WarningType_to_string(WarningType type) {
  switch (type) {
    case WarningType::SEMANTIC:
      return "SemanticWarning: ";
    default:
      throw std::invalid_argument("Invalid TokenType");
  }
}

std::shared_ptr<Logger> Logger::instance_ = 0;

std::shared_ptr<Logger> Logger::Instance() {
  if (!instance_) {
    instance_ = std::make_shared<Logger>();
  }
  return instance_;
}

void Logger::Err(const std::string& msg, ErrorType type) {
  std::string log = ErrorType_to_string(type) + msg + "\n";
  Instance()->errors_.emplace_back(-1, log);
}

void Logger::Err(const std::string& msg, int line, ErrorType type) {
  std::string log = ErrorType_to_string(type) + msg + " (line " +
                    std::to_string(line) + ") \n";
  Instance()->errors_.emplace_back(line, log);
}

bool Logger::HasErrors() { return Instance()->errors_.size() > 0; }

void Logger::PrintErrors() {
  auto instance = Instance();
  // Sort the based on line number
  std::sort(instance->errors_.begin(), instance->errors_.end(),
            [](auto& left, auto& right) { return left.first < right.first; });
  std::ofstream ofs("../out/outerrors");
  for (auto err : instance->errors_) {
    std::cout << err.second;
    ofs << err.second;
  }
}

std::vector<std::string> Logger::GetErrors() {
  auto instance = Instance();
  // Sort based on line number
  std::sort(instance->errors_.begin(), instance->errors_.end(),
            [](auto& left, auto& right) { return left.first < right.first; });
  std::vector<std::string> errors;
  for (auto err : instance->errors_) {
    errors.push_back(err.second);
  }
  return errors;
}

void Logger::Warn(const std::string& msg, WarningType type) {
  std::string log = WarningType_to_string(type) + msg + "\n";
  Instance()->warnings_.emplace_back(-1, log);
}

void Logger::Warn(const std::string& msg, int line, WarningType type) {
  std::string log = WarningType_to_string(type) + msg + " (line " +
                    std::to_string(line) + ") \n";
  Instance()->warnings_.emplace_back(line, log);
}

bool Logger::HasWarnings() { return Instance()->warnings_.size() > 0; }

void Logger::PrintWarnings() {
  auto instance = Instance();
  // Sort the based on line number
  std::sort(instance->warnings_.begin(), instance->warnings_.end(),
            [](auto& left, auto& right) { return left.first < right.first; });
  std::ofstream ofs("../out/outwarnings");
  for (auto warn : instance->warnings_) {
    std::cout << warn.second;
    ofs << warn.second;
  }
}

std::vector<std::string> Logger::GetWarnings() {
  auto instance = Instance();
  // Sort based on line number
  std::sort(instance->warnings_.begin(), instance->warnings_.end(),
            [](auto& left, auto& right) { return left.first < right.first; });
  std::vector<std::string> warnings;
  for (auto warn : instance->warnings_) {
    warnings.push_back(warn.second);
  }
  return warnings;
}

void Logger::Clear() {
  auto instance = Instance();
  instance->errors_.clear();
  instance->warnings_.clear();
}

}  // namespace toy