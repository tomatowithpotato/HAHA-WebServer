#ifndef __HAHA_RETOPTION_H__
#define __HAHA_RETOPTION_H__


namespace haha{

template<typename T>
class RetOption{
public:
    RetOption(const T& val, bool exist):val_(val),exist_(exist){}
    T& value() { return val_; }
    bool exist() { return exist_; }
private:
    T val_;
    bool exist_;
};

}


#endif