/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */ 
#include "TimerHeap.h"

namespace haha
{

void TimerHeap::siftup_(size_t i) {
    /* 上浮 */
    assert(i >= 0 && i < heap_.size());
    int j = (i - 1) / 2;
    while(j >= 0) {
        if(heap_[j] < heap_[i]) { break; }
        SwapNode_(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}

bool TimerHeap::siftdown_(size_t index, size_t n) {
    /* 下沉 */
    assert(index >= 0 && index < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    size_t i = index;
    size_t j = i * 2 + 1;
    while(j < n) {
        if(j + 1 < n && heap_[j + 1] < heap_[j]) j++;
        if(heap_[i] < heap_[j]) break;
        SwapNode_(i, j);
        i = j;
        j = i * 2 + 1;
    }
    return i > index;
}

void TimerHeap::SwapNode_(size_t i, size_t j) {
    assert(i >= 0 && i < heap_.size());
    assert(j >= 0 && j < heap_.size());
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].fd] = i;
    ref_[heap_[j].fd] = j;
} 

void TimerHeap::del_(size_t index) {
    /* 删除指定位置的结点 */
    assert(!heap_.empty() && index >= 0 && index < heap_.size());
    /* 将要删除的结点换到队尾，然后调整堆 */
    size_t i = index;
    size_t n = heap_.size() - 1;
    assert(i <= n);
    if(i < n) {
        SwapNode_(i, n);
        if(!siftdown_(i, n)) {
            siftup_(i);
        }
    }
    /* 队尾元素删除 */
    int ret = ref_.erase(heap_.back().fd);
    assert(ret > 0);
    heap_.pop_back();
}

void TimerHeap::push(const Timer& timer){
    ReadWriteLock::RAIIWriteLock lock_gaurd(mtx_);
    assert(timer.fd >= 0);
    size_t i;
    auto x = ref_.find(timer.fd);
    if(x == ref_.end()){
        /* 新节点：堆尾插入，调整堆 */
        i = heap_.size();
        ref_[timer.fd] = i;
        heap_.push_back(timer);
        siftup_(i);
    }
    else{
        /* 已有结点：调整堆 */
        i = ref_[timer.fd];
        heap_[i] = timer;
        if(!siftdown_(i, heap_.size())) {
            siftup_(i);
        }
    }
}

void TimerHeap::adjust(const Timer& timer){
    ReadWriteLock::RAIIWriteLock lock_gaurd(mtx_);
    auto x = ref_.find(timer.fd);
    if(x != ref_.end()){
        int pos = x->second;
        heap_[pos].expire = timer.expire;
        siftdown_(pos, heap_.size());
    }
}

void TimerHeap::remove(const Timer& timer){
    ReadWriteLock::RAIIWriteLock lock_gaurd(mtx_);
    auto x = ref_.find(timer.fd);
    if(x != ref_.end()){
        int pos = x->second;
        del_(pos);
    }
}

void TimerHeap::pop() {
    ReadWriteLock::RAIIWriteLock lock_gaurd(mtx_);
    assert(!heap_.empty());
    del_(0);
}

void TimerHeap::clear() {
    ReadWriteLock::RAIIWriteLock lock_gaurd(mtx_);
    ref_.clear();
    heap_.clear();
}

} // namespace haha