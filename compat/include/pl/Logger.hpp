#pragma once
#include <android/log.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <utility>
namespace pl::log {
class Logger {
public:
  explicit Logger(std::string name) : mName(std::move(name)) {}
  static Logger& getOrCreate(std::string name) {
    static std::mutex mutex;
    static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;
    std::lock_guard<std::mutex> lock(mutex);
    auto it = loggers.find(name);
    if (it != loggers.end()) return *it->second;
    auto ptr = std::make_unique<Logger>(std::move(name));
    auto& ref = *ptr;
    loggers.emplace(ref.mName, std::move(ptr));
    return ref;
  }
  template<class... Args> void info(std::string_view message, Args&&...) const { write(ANDROID_LOG_INFO, message); }
  template<class... Args> void debug(std::string_view message, Args&&...) const { write(ANDROID_LOG_DEBUG, message); }
  template<class... Args> void warn(std::string_view message, Args&&...) const { write(ANDROID_LOG_WARN, message); }
  template<class... Args> void error(std::string_view message, Args&&...) const { write(ANDROID_LOG_ERROR, message); }
private:
  void write(int level, std::string_view message) const { __android_log_print(level, mName.c_str(), "%.*s", static_cast<int>(message.size()), message.data()); }
  std::string mName;
};
}
