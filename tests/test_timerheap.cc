#include <time.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "TimerHeap.h"

int main(){
    haha::TimerHeap heap;
    std::vector<int> nums;
    for(int i = 0; i < 1000; ++i){
        nums.push_back(i);
    }

    int cnt = 1000;
    while(cnt--){
        std::random_shuffle(nums.begin(), nums.end());
        for(int i = 0; i < nums.size(); ++i){
            heap.push(haha::Timer(
                i,
                haha::TimeStamp(nums[i]),
                nullptr
            ));
        }
        while(!heap.empty()){
            auto t = heap.top();
            // std::cout << t.expire.microsecond() << std::endl;
            heap.pop();
            if(!heap.empty()){
                assert(t < heap.top());
            }
        }
        std::cout << "ok" << std::endl;
    }
    
    return 0;
}