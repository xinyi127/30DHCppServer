// 每一个 Reactor 只应该负责事件分发而不应该负责事件处理，构建一个线程数量固定的线程池，用于事件处理。
// 关于线程池，需要特别注意的有两点：
// 一是在多线程环境下任务队列的读写操作都应该考虑互斥锁，二是当任务队列为空时CPU不应该不断轮询耗费 CPU 资源。
// 为了解决第一点，这里使用 std::mutex 来对任务队列进行加锁解锁。为了解决第二个问题，使用了条件变量 std::condition_variable 。

// 线程池的核心思想是，创建一定数量的线程，并让它们从任务队列中获取任务执行。
// 通过互斥量和条件变量的配合，线程可以安全地访问任务队列，并在任务队列为空时等待新任务的到来。这样可以充分利用系统资源，提高并发执行效率。

// day11 update
// 通过右值引用、完美转发等阻止拷贝
// 修改 add 函数，使用线程池前不再需要手动绑定参数，而是直接传递，且可以得到任务返回值

#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

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
    // void add(std::function<void()>);
    template<class F, class... Args>
    auto add(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
};

// 不能放在 cpp 文件，原因是 C++ 编译器不支持模板的分离编译
template<class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    // result_of 用于确定 F 调用结果的类型
    using return_type = typename std::result_of<F(Args...)>::type; // 返回值类型

    // packaged_task 目的是将可调用对象封装成可以异步执行的任务，并可以获取任务执行的结果
    auto task = std::make_shared< std::packaged_task<return_type()> >( // 完美指针
            // bind 函数将可调用对象 f 绑定到 packaged_task 上
           std::bind(std::forward<F>(f), std::forward<Args>(args)...) // 完美转发参数
        );

    // task->get_future() 返回一个与 task 关联的期物对象，该期物对象可以用于获取 task 异步任务的返回值
    std::future<return_type> res = task->get_future(); // 使用期物
    { // 队列锁作用域，好处是任务添加到队列后可以立即释放锁，以便其他线程可以继续执行
        std::unique_lock<std::mutex> lock(tasks_mtx); // 加锁

        // 线程池停止后不允许再添加任务
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        // 匿名 Lambda 函数，表示要添加到任务队列中的任务 [task]
        // 表示捕获的变量，(*task)() 先解引用然后通过函数调用运算符 () 执行任务
        tasks.emplace([task](){ (*task)(); }); // 将任务添加到任务队列
    }
    cv.notify_one(); // 通知一次条件变量
    return res; // 返回一个期物
}
