#include "ThreadPool.h"

ThreadPool::ThreadPool(int size) : stop(false){
    for(int i = 0; i < size; i ++){
        threads.emplace_back(std::thread([this](){
            while(true){
                std::function<void()> task;
                // 在这个{}作用域内对std::mutex加锁，出了作用域会自动解锁，不需要调用unlock()
                {
                    // std::unique_lock 则是相对于 std::lock_guard 出现的，std::unique_lock 更加灵活。
                    // std::unique_lock 的对象会以独占所有权（没有其他的 unique_lock 对象同时拥有某个 mutex 对象的所有权）
                    // 的方式管理 mutex 对象上的上锁和解锁的操作
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    // wait() 函数有两种形式。第二个参数用于判断是否继续等待。
                    cv.wait(lock, [this](){
                        return stop | !tasks.empty();
                    });
                    if(stop && tasks.empty()) return ; // 等待条件变量，条件为任务队列不为空或线程池停止。
                    task = tasks.front(); // 从任务队列头取出一个任务
                    tasks.pop();
                }
                task(); // 执行任务
            }
        }));
    }
}

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for(std::thread &th :threads){
        if(th.joinable())
            th.join(); // 需要明确一点，线程初始化后就开始执行了，而不是调用 join() 后才开始执行
    }
}

/*
void ThreadPool::add(std::function<void()> func){
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        if(stop)
            throw std::runtime_error("ThreadPool already stop, can't add task any more");
        tasks.emplace(func);
    }
    cv.notify_one(); // 通知一个等待中的线程
}
*/
