#ifndef __HAHA_JSON_JSONERROR_H__
#define __HAHA_JSON_JSONERROR_H__

#include <exception>
#include <sstream>
#include <string>

#define HAHA_JSON_ERROR(msg) JsonError(__FILE__, __LINE__, msg)

namespace haha {

namespace json
{

class JsonError : public std::exception {
public:
    JsonError() {}
    explicit JsonError(const char* file, int line, const std::string &msg)
    {
        std::stringstream ss;
        ss << "SyntaxError: [" << file << ":"
          << std::to_string(line) << "]: [" << msg << "]";
        __error_msg = ss.str();
    }
    virtual ~JsonError() {}
    virtual const char *what() const noexcept { return __error_msg.c_str(); }

protected:
    std::string __error_msg;
};

// class JsonNumberError : public JsonError {
// public:
//     JsonNumberError(int line, int col, const std::string &msg)
//         : __line(line), __col(col) {
//         std::stringstream ss;
//         ss << "SyntaxError: [" << std::to_string(__line) << ","
//           << std::to_string(__col) << "] => [" << msg << "]";
//         __error_msg = ss.str();
//     }
//     int line() const { return __line; }
//     int col() const { return __col; }
// protected:
//     int __line, __col;
// };


// class JsonTypeCastError : public JsonError {
// public:
//     JsonTypeCastError(int line, int col, const std::string &msg)
//         : __line(line), __col(col) {
//         std::stringstream ss;
//         ss << "SyntaxError: [" << std::to_string(__line) << ","
//           << std::to_string(__col) << "] => [" << msg << "]";
//         __error_msg = ss.str();
//     }
//     int line() const { return __line; }
//     int col() const { return __col; }
// protected:
//     int __line, __col;
// };


// class JsonSyntaxError : public JsonError {
// public:
//     JsonSyntaxError(int line, int col, const std::string &msg)
//         : __line(line), __col(col) {
//         std::stringstream ss;
//         ss << "SyntaxError: [" << std::to_string(__line) << ","
//           << std::to_string(__col) << "] => [" << msg << "]";
//         __error_msg = ss.str();
//     }
//     int line() const { return __line; }
//     int col() const { return __col; }

// protected:
//     int __line, __col;
// };

} // namespace json

} // namespace haha
#endif
