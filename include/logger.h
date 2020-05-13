#ifndef TOY_LOGGER_H_
#define TOY_LOGGER_H_

#include <memory>
#include <vector>

namespace toy {

enum class ErrorType { LEXING, SYNTAX, SEMANTIC };
enum class WarningType { LEXING, SYNTAX, SEMANTIC };

/**
 * Logger singleton to keep track of errors and warnings
 * across different compilation stages.
 */
class Logger {
 public:
  static void Err(const std::string& msg, ErrorType type);
  static void Err(const std::string& msg, int line, ErrorType type);
  static bool HasErrors();
  static void PrintErrors();
  static std::vector<std::string> GetErrors();

  static void Warn(const std::string& msg, WarningType type);
  static void Warn(const std::string& msg, int line, WarningType type);
  static bool HasWarnings();
  static void PrintWarnings();
  static std::vector<std::string> GetWarnings();

  static void Clear();
  static std::shared_ptr<Logger> Instance();

 private:
  static std::shared_ptr<Logger> instance_;
  std::vector<std::pair<int, std::string>> errors_;
  std::vector<std::pair<int, std::string>> warnings_;
};

}  // namespace toy

#endif  // TOY_LOGGER_H_