#ifndef MYT_OBJECT_HPP
#define MYT_OBJECT_HPP

#include <string>

class MytObject {
 public:
  virtual ~MytObject();
  virtual const std::string to_string() const noexcept = 0;
};

class StringObject : MytObject {
 public:
  StringObject() = delete;
  StringObject(const std::string& value) : m_value(value) {};
  [[nodiscard]] const std::string to_string() const noexcept override {
    return m_value;
  };

 private:
  std::string m_value{};
};

class BoolObject : MytObject {
 public:
  BoolObject() = delete;
  BoolObject(const bool& value) : m_value(value) {};
  [[nodiscard]] const std::string to_string() const noexcept override {
    return (m_value) ? "true" : "false";
  };

 private:
  bool m_value{};
};

class IntObject : MytObject {
 public:
  IntObject() = delete;
  IntObject(const int& value) : m_value(value) {};
  [[nodiscard]] const std::string to_string() const noexcept override {
    return std::to_string(m_value);
  };

 private:
  int m_value{};
};

#endif  // MYT_OBJECT_HPP
