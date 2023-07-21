// 每一个 Reactor 只应该负责事件分发而不应该负责事件处理，构建一个线程数量固定的线程池，用于事件处理。
// 关于线程池，需要特别注意的有两点：
// 一是在多线程环境下任务队列的读写操作都应该考虑互斥锁，二是当任务队列为空时CPU不应该不断轮询耗费 CPU 资源。
// 为了解决第一点，这里使用 std::mutex 来对任务队列进行加锁解锁。为了解决第二个问题，使用了条件变量 std::condition_variable 。

// 线程池的核心思想是，创建一定数量的线程，并让它们从任务队列中获取任务执行。
// 通过互斥量和条件变量的配合，线程可以安全地访问任务队列，并在任务队列为空时等待新任务的到来。这样可以充分利用系统资源，提高并发执行效率。

#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
private:
    std::vector<std::thread> threads; // 存储线程
    std::queue<std::function<void()>> tasks; // 存储待执行任务
    std::mutex tasks_mtx; // 用于对任务队列进行互斥访问
    std::condition_variable cv; // 防止任务队列为空时 CPU 不断轮询耗费 CPU 资源
    bool stop; // 指示线程池是否停止
public:
    // 默认 size 也可以设置为 std::thread::hardware_concurrency()，表示系统支持的最大线程数
    ThreadPool(int size = 10); // 参数为线程池大小
    ~ThreadPool();
    // 用于向任务队列中添加新任务。它接受一个可调用对象作为参数，并将其添加到任务队列中等待执行
    void add(std::function<void()>);
};
