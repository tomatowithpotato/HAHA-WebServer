#include "http/HttpMultiPart.h"

#include <cstring>

namespace haha{

RetOption<std::string> HttpMultiPart::getValue(const std::string &name) const{
    return form_.get(name);
}

auto HttpMultiPart::getFile(const std::string &name, size_t index) const
    -> HttpMultiPart::Part * {
    Part *p = nullptr;
    auto it = files_.get(name);
    if(it.exist()){
        if(index >= it.value().size()){
            return p;
        }
        p = &(it.value().at(index));
    }
    return p;
}

void HttpMultiPart::parse(std::string_view body) {
    size_t i = 0, bl = bd_.size(), len = body.size();
    std::string name, filename, type, form_file_data;
    bool file_mark = false;
    const char *boundary_s = bd_.data();
    State state = start_body;

    static constexpr const size_t buf_size = 4096;
    char buffer[buf_size];

    while (i < len) {
        switch (state) {
            case start_body: {
                // boundary begin --boundary
                if (i + 1 < len && body[i] == '-' && body[i + 1] == '-') {
                    i += 2;
                    state = start_boundary;
                }
                break;
            }
            case start_boundary: {
                if(::strncmp(body.data() + i, boundary_s, bl) == 0) {
                    i += bl;
                        // --boundary\r\n
                    if (i + 1 < len && body[i] == CR && body[i + 1] == LF) {
                        i += 2;
                        state = end_boundary;
                    } else if (i + 1 < len && body[i] == '-' && body[i + 1] == '-') {
                        // --boundary--
                        i += 2;
                        state = end_body;
                    } else {
                        // bad body, ignored
                    }
                }
                break;
            }
            case end_boundary: {
                if (::strncasecmp(body.data() + i, "Content-Disposition", 19) == 0) {
                    state = start_content_disposition;
                    i += 19; // skip "Content-Disposition"
                } else {
                    // bad body
                }
                break;
            }
            case start_content_disposition: {
                // name="mike"\r\n
                // name="file1"; filename="test.txt"\r\n
                i += 13; // skip ": form-data; "
                bool start_name_filename = false;
                bool is_name = true;
                while (i < len) {
                    if (i + 1 < len && body[i] == CR && body[i + 1] == LF) {
                        i += 2;
                        state = end_content_disposition;
                        break;
                    }
                    if (i + 1 < len && body[i] == '=' && body[i + 1] == '\"') {
                        i += 2;
                        start_name_filename = true;
                    } else if (body[i] == '\"') {
                        start_name_filename = false;
                        is_name = !is_name; // name/filename
                        i++;
                    } else if (start_name_filename) {
                        if (is_name)
                        name += body[i++];
                        else
                        filename += body[i++];
                    } else {
                        i++;
                    }
                }
                break;
            }
            case end_content_disposition: {
                // html form content data
                if (i + 1 < len && body[i] == CR && body[i + 1] == LF) {
                    i += 2;
                    file_mark = false;
                    state = start_content_data;
                } else {
                    // file type
                    if (::strncasecmp(body.data() + i, "Content-Type", 12) == 0) {
                        i += 14; // skip "Content-Type"
                        file_mark = true;
                        state = start_content_type;
                    } else {
                        // bad body
                    }
                }
                break;
            }
            case start_content_type: {
                while (i < len) {
                    if (i + 1 < len && body[i] == CR && body[i + 1] == LF) {
                        i += 2;
                        state = end_content_type;
                        break;
                    } else{
                        type += body[i++];
                    }
                }
                break;
            }
            case end_content_type: {
                if (i + 1 < len && body[i] == CR && body[i + 1] == LF) {
                    i += 2;
                    state = start_content_data;
                } else {
                // bad body
                }
                break;
            }
            case start_content_data: {
                form_file_data.reserve(buf_size);
                size_t k = 0, ix = 1, pi = i;
                while (i < len) {
                    if (i + 4 < len && body[i] == CR && body[i + 1] == LF &&
                        body[i + 2] == '-' && body[i + 3] == '-' &&
                        ::strncmp(body.data() + i + 4, boundary_s, bl) == 0) 
                    {
                        if (k != 0){
                            form_file_data.append(buffer, k);
                        }
                        form_file_data.resize(i - pi);
                        i += 2;
                        state = end_content_data;
                        break;
                    } else {
                        buffer[k++] = body[i++];
                        if (k >= buf_size) {
                            k = 0, ix++;
                            form_file_data.append(buffer, buf_size);
                            form_file_data.reserve(buf_size * ix);
                        }
                    }
                }
                break;
            }
            case end_content_data: {
                if (!file_mark) {
                    // form
                    form_.add(name, form_file_data);
                } 
                else {
                    // file
                    auto x = files_.get(name);
                    if (x.exist()) {
                        x.value().emplace_back(std::move(name), std::move(filename),
                                            std::move(type), std::move(form_file_data));
                    } else {
                        files_.add(name, std::move(std::vector{
                                            Part(name, filename, type, form_file_data)}));
                    }
                }
                name = filename = type = form_file_data = "";
                file_mark = false;
                state = start_body;
                break;
            }
            case end_body: {
                i += 2; // skip \r\n
                break;
            }
            default: {
                i++; // ignored
                break;
            }
        }
    }
}

}