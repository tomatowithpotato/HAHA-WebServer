#include "http/HttpRequest.h"

namespace haha{

HttpRequest::HttpRequest(Buffer::ptr data)
    :data_(data)
    ,state_(CHECK_REQUESTLINE)
    ,lastEnd_(0)
    ,max_requestLine_len_(2048)
    ,max_requestHeader_len_(8192)
    ,keepAlive_(false)
    ,compressed_(false)
    ,hasContentType_(false)
    ,hasContentLength_(false)
    ,hasCookies_(false)
    ,hasTransferEncoding_(false)
    ,chunked_(false)
    ,chunk_size_str_limit(5) // 16^5
    ,cur_chunkedState_(HttpChunkedState::NO_LEN){

}


HttpRequest::RET_STATE HttpRequest::parseRequest(){
    while(true){
        switch (state_)
        {
        case CHECK_REQUESTLINE:
            retState_ = parseRequestLine();
            if(retState_ == OK_REQUEST){
                state_ = CHECK_HEADER;
            }
            break;
        case CHECK_HEADER:
            retState_ = parseRequestHeader();
            if(retState_ == OK_REQUEST){
                state_ = CHECK_CONTENT;
            }
            break;
        case CHECK_CONTENT:
            retState_ = parseRequestContent();
            if(retState_ == OK_REQUEST){
                state_ = CHECK_DONE;
            }
            break;
        default:
            break;
        }

        if(retState_ != OK_REQUEST || state_ == CHECK_DONE){
            break;
        }
    }
    return retState_;
}

HttpRequest::RET_STATE HttpRequest::parseRequestLine(){
    if(data_->ReadableBytes() == 0){
        return AGAIN_REQUEST;
    }
    // 超出最大限制，结束
    if(data_->ReadableBytes() > max_requestLine_len_){
        return BAD_REQUEST;
    }
    std::string_view view(data_->Peek(), data_->ReadableBytes());

    // 寻找行结束位置，没有就需要继续接收数据
    size_t end = view.find("\r\n");
    if(end == view.npos){
        return AGAIN_REQUEST;
    }

    size_t i = 0;
    std::string_view line = view.substr(0, end);

    // 解析method
    if (line.starts_with("GET")){
        method_ = HttpMethod::GET;
        i += 4;
    }
    else if (line.starts_with("POST")){
        method_ = HttpMethod::POST;
        i += 5;
    }
    else if (line.starts_with("HEAD")){
        method_ = HttpMethod::HEAD;
        i += 5;
    }
    else{
        return BAD_REQUEST;
    }
    line.remove_prefix(i);

    // 解析url
    size_t pos = line.find_first_of(' ');
    if (pos == line.npos){
        return BAD_REQUEST;
    }
    reqUrl_ = HttpUrl(std::string(line.data(), pos));
    if(reqUrl_.getState() == HttpUrl::PARSE_FAIL){
        return BAD_REQUEST;
    }

    line.remove_prefix(pos + 1);
    i += pos + 1;

    // 解析version
    if (line.starts_with("HTTP/1.0")){
        version_ = HttpVersion::HTTP_1_0;
    }
    else if (line.starts_with("HTTP/1.1")){
        version_ = HttpVersion::HTTP_1_1;
    }
    else if (line.starts_with("HTTP/2.0")){
        version_ = HttpVersion::HTTP_2_0;
    }
    else{
        return BAD_REQUEST;
    }

    line.remove_prefix(8);
    i += 10;    // 8 + 2
    data_->Retrieve(i);

    return OK_REQUEST;
}

HttpRequest::RET_STATE HttpRequest::parseRequestHeader(){
    if(data_->ReadableBytes() == 0){
        return AGAIN_REQUEST;
    }
    std::string_view header(data_->Peek(), data_->ReadableBytes());
    std::string_view line_header;
    while (header.size()) {
        // 碰到空行，结束
        if (header.size() >= 2 && header[0] == '\r' && header[1] == '\n') {
            data_->Retrieve(2);
            /* 解析请求头的内容 */
            parseKeepAlive();
            parseAcceptEncoding();
            parseContentType();
            parseCookies();
            parseContentLength();
            parseTransferEncoding();
            return OK_REQUEST;
        }
        // 超出最大限制，结束
        if(header_.size() > max_requestHeader_len_){
            return BAD_REQUEST;
        }

        // 寻找行结束位置，没有就需要继续接收数据
        size_t crlf_pos = header.find("\r\n");
        if (crlf_pos == header.npos)
            break;

        line_header = header.substr(0, crlf_pos);

        size_t pos2 = line_header.find_first_of(":");
        if (pos2 == line_header.npos)
            return BAD_REQUEST;

        std::string_view key = line_header.substr(0, pos2);
        std::string_view value = "";

        /* 处理冒号后没有空格和有多个空格的情况 */
        size_t pos3 = line_header.find_first_of(" ", pos2);
        if(pos3 == line_header.npos){
            pos3 = pos2+1;
        }
        size_t pos4 = line_header.find_first_not_of(" ", pos3);
        if(pos4 != line_header.npos){
            value = line_header.substr(pos4);
        }

        // std::string_view value = line_header.substr(pos2 + 2, crlf_pos - pos2 - 2);
        header_.add(std::string(key.data(), key.size()),
                    std::string(value.data(), value.size()));

        header.remove_prefix(crlf_pos + 2);
        data_->Retrieve(crlf_pos + 2);
    }
    return AGAIN_REQUEST;
}



HttpRequest::RET_STATE HttpRequest::parseRequestContent(){
    if(data_->ReadableBytes() == 0){
        return AGAIN_REQUEST;
    }

    /* 分块传输要咋搞？卧槽 */
    if(hasTransferEncoding_){
        if(chunked_){
            return parseChunked();
        }
        else{
            // 不支持其他编码
            HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "unsupport type other than chunked";
            return BAD_REQUEST;
        }
    }
    // 根据Content-Length判断数据是否读完
    else if(hasContentLength_){
        if(contentLength_ > body_.size() + data_->ReadableBytes()){
            return AGAIN_REQUEST;
        }
        else{
            size_t accept_size = contentLength_ - body_.size();
            body_.append(data_->Peek(), accept_size);
            data_->Retrieve(accept_size);
        }
    }

    if(hasContentType_){
        if(contentType_ == HttpContentType::MULTIPART){
            std::string eob = "--" + multipart_.getBoundary() + "--\r\n";
            if (!body_.ends_with(eob)){
                return AGAIN_REQUEST;
            }
            std::string_view view = body_;
            multipart_.parse(view);
        }
        else if(contentType_ == HttpContentType::URLENCODED){
            parseKeyValue(body_, "=", "&", form_, true);
        }
        else if(contentType_ == HttpContentType::PLAIN){
            parseKeyValue(body_, "=", "&", form_, false);
        }
        else if(contentType_ == HttpContentType::JSON){
            // 暂未实现
        }
    }

    return OK_REQUEST;
}




void HttpRequest::parseKeepAlive(){
    // HTTP1.0以上的协议默认开启长连接
    if(version_ != HttpVersion::HTTP_1_0){
        keepAlive_ = true;
    }
    auto ret = header_.get("Connection");
    if(ret.exist()){
        std::string val = toLowers(ret.value());
        if(val == "keep-alive"){
            keepAlive_ = true;
        }
        else if(val == "close"){
            keepAlive_ = false;
        }
        else{
            keepAlive_ = false;
        }
    }
}

void HttpRequest::parseContentType(){
    auto ret = header_.get("Content-Type");
    if(ret.exist()){
        hasContentType_ = true;
        std::string val = toLowers(ret.value());
        if(val.starts_with("application/x-www-form-urlencoded")){
            contentType_ = HttpContentType::URLENCODED;
        }
        else if(val.starts_with("multipart/form-data")){
            contentType_ = HttpContentType::MULTIPART;
        }
        else if(val.starts_with("text/plain")){
            contentType_ = HttpContentType::PLAIN;
        }
        else if(val.starts_with("text/html")){
            contentType_ = HttpContentType::HTML;
        }
        else if(val.starts_with("application/json")){
            contentType_ = HttpContentType::JSON;
        }
        else{
            contentType_ = HttpContentType::UNKNOW;
            HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "unsupport Content-Type";
        }
    }
}

void HttpRequest::parseAcceptEncoding(){
    auto ret = header_.get("Accept-Encoding");
    if(ret.exist()){
        compressed_ = ret.value().find("gzip") != std::string::npos;
    }
}

void HttpRequest::parseCookies(){
    auto ret = header_.get("Cookie");
    if(ret.exist()){
        hasCookies_ = true;
        cookies_ = HttpCookie(ret.value());
    }
}

void HttpRequest::parseContentLength(){
    auto ret = header_.get("Content-Length");
    if(ret.exist()){
        hasContentLength_ = true;
        contentLength_ = stoul(ret.value());
    }
}

void HttpRequest::parseTransferEncoding(){
    auto ret = header_.get("Transfer-Encoding");
    if(ret.exist()){
        hasTransferEncoding_ = true;
        transferEncoding_ = toLowers(ret.value());
        if(transferEncoding_ == "chunked"){
            chunked_ = true;
        }
    }
}

HttpRequest::RET_STATE HttpRequest::parseChunked(){
    std::string_view view(data_->Peek(), data_->ReadableBytes());
    while(true){
        // 读取长度
        if(cur_chunkedState_ == HttpChunkedState::NO_LEN){
            size_t pos = view.substr(0, chunk_size_str_limit+2).find("\r\n");
            if(pos == view.npos){
                return AGAIN_REQUEST;
            }
            // 数据长度超限
            if(pos > chunk_size_str_limit){
                return BAD_REQUEST;
            }
            cur_chunk_size_ = hexString2Int(view.substr(0,pos));
            // 读到最后的空块，完毕
            if(cur_chunk_size_ == 0){
                data_->Retrieve(pos+2);
                view.remove_prefix(pos+2);
                cur_chunkedState_ = HttpChunkedState::DONE;
                continue;
            }
            // 错误的数字
            if(cur_chunk_size_ == -1){
                return BAD_REQUEST;
            }
            // 读取完长度，转为读取数据
            data_->Retrieve(pos+2);
            view.remove_prefix(pos+2);
            cur_chunkedState_ = HttpChunkedState::NO_DATA;
        }
        // 读取数据
        else if(cur_chunkedState_ == HttpChunkedState::NO_DATA){
            if(view.size() < cur_chunk_size_ + 2){
                return AGAIN_REQUEST;
            }
            // 读取数据成功，开始准备下一个chunk
            else if(view[cur_chunk_size_] == '\r' && view[cur_chunk_size_+1] == '\n'){
                body_.append(data_->Peek(), cur_chunk_size_);
                data_->Retrieve(cur_chunk_size_+2);
                view.remove_prefix(cur_chunk_size_+2);
                cur_chunkedState_ = HttpChunkedState::NO_LEN;
            }
            else{
                return BAD_REQUEST;
            }
        }
        // 所有chunk都接受完毕
        else if(cur_chunkedState_ == HttpChunkedState::DONE){
            if(view.size() < 2){
                return AGAIN_REQUEST;
            }
            else if(view[0] == '\r' && view[1] == '\n'){
                data_->Retrieve(2);
                view.remove_prefix(2);
                return OK_REQUEST;
            }
            else{
                return BAD_REQUEST;
            }
        }
    }
    return AGAIN_REQUEST;
}

}