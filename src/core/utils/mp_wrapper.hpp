
#pragma once
#include <thread>
#include <atomic>

inline auto workers = std::thread::hardware_concurrency();

template<typename T, typename S>
class PData
{
public:
    virtual T& operator[](S& s)=0;
    long count {}; 
};

template<typename T, typename S, typename FnType, typename... Args>
void wrapper(unsigned int workers, std::atomic<PData<T,S>> meta, FnType fn, Args&&... args)
{
    std::thread t(fn, std::forward<Args>(args)...);
    t.join();
}