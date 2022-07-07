#ifndef HAHA_BASE_NONCOPYABLE_H
#define HAHA_BASE_NONCOPYABLE_H

namespace haha
{

/*不可拷贝*/
class noncopyable
{
public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;

protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}  // namespace haha

#endif  // HAHA_BASE_NONCOPYABLE_H
