#ifndef __HAHA_HTTPMULTIPART_H__
#define __HAHA_HTTPMULTIPART_H__

#include <string>
#include <vector>
#include <unordered_map>
#include "http/HttpMap.h"
#include "base/RetOption.h"

namespace haha{

class HttpMultiPart {
public:
    struct Part {
        std::string name;
        std::string file_name;
        std::string file_type;
        std::string data;

        Part() {}
        Part(std::string name, std::string fn, std::string ft, std::string data)
            : name(std::move(name)), file_name(std::move(fn)),
            file_type(std::move(ft)), data(std::move(data)) {}
    };

    typedef HttpBaseMap<std::string, std::vector<Part>> Files;
    typedef HttpBaseMap<std::string, std::string> Form;

    void setBoundary(const std::string &boundary) noexcept { bd_ = boundary; }
    const std::string &getBoundary() const noexcept { return bd_; }
    RetOption<std::string> getValue(const std::string &name) const;
    Part *getFile(const std::string &name, size_t index = 0) const;

    void parse(std::string_view body);

private:
    enum State {
        start_body,
        start_boundary,
        end_boundary,
        start_content_disposition,
        end_content_disposition,
        start_content_type,
        end_content_type,
        start_content_data,
        end_content_data,
        end_body
    };

    std::string bd_;
    Form form_;
    mutable Files files_;

    inline constexpr static char CR = '\r';
    inline constexpr static char LF = '\n';
};

}


#endif