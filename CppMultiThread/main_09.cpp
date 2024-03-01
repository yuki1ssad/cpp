/*
    future 其他成员函数、shared_future、atomic
    1、std::future_status status = result.wait_for(std::chrono::seconds(6));
        /// Status code for futures
            enum class future_status
            {
                ready,
                timeout,
                deferred
            };

    2、std::shared_future 也是个类模板
        future 对象的 get 函数是转移对象中的值
            {
                /// Retrieving the value
                    _Res
                    get()
                    {
                        typename _Base_type::_Reset __reset(*this);
                        return std::move(this->_M_get_result()._M_value());
                    }
            }
        shared_future 对象的 get 函数是复制数据
            {
                /// Retrieving the value
                    const _Res&
                    get() const { return this->_M_get_result()._M_value(); }
            }
    3、std::atomic
        原子操作一般针对一个变量
        互斥量一般用于约束一段代码段

*/
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <unistd.h>
#include <list>
#include <mutex>
#include <condition_variable>
#include <future>

int mythread()
{
    std::cout << "sub_thread_id: " << std::this_thread::get_id() << std::endl;
    sleep(2);
    return 5;
}

// int g_cnt = 0;
std::atomic<int> g_cnt(0); // 封装了一个类型为 int 的对象
void atomic_fun()
{
    for (int i = 0; i < 100000; i++)
    {
        g_cnt++;
    }
    return;
}

int main()
{
    std::cout << "main_thread_id: " << std::this_thread::get_id() << std::endl;
    // std::future<int> result = std::async(std::launch::deferred, mythread); // 绑定关系，单线程并不卡在这

    // // 执行下面两句的任何一句都会将 result 移动到 result_s， 或者直接用 shared_future ： 
    // // std::shared_future<int> result_s(std::move(result));
    // std::shared_future<int> result_s(result.share());


    // std::cout << "contunuing...." << std::endl;
    // int def = 0;
    // // std::cout << "result: " << result.get() << std::endl; // 卡在这里等待执行 mythread 的线程执行完
    // // std::future_status status = result.wait_for(std::chrono::seconds(6));
    // // if (status == std::future_status::timeout) // 超时，表示线程还没执行完
    // // {
    // //     std::cout << "timeout" << std::endl;

    // // }
    // // else if (status == std::future_status::ready) // 线程成功返回
    // // {
    // //     std::cout << "ready" << std::endl;
    // // }
    // // else if (status == std::future_status::deferred) // 如果 std::async(std::launch::deferred, mythread);
    // // {                                                   // 这种情况下，不会创建新的子线程，而是在主线程中对 mythread 进行调用
    // //     std::cout << "deferred" << std::endl;
    // //     std::cout << result.get() << std::endl;
    // // }

    // // 可以多次 get
    // std::cout << result_s.get() << std::endl;
    // std::cout << result_s.get() << std::endl;
    // std::cout << result_s.get() << std::endl;


    // std::atomic
    std::thread mytobj1(atomic_fun);
    std::thread mytobj2(atomic_fun);
    mytobj1.join();
    mytobj2.join();
    std::cout << "finally, g_cnt = " << g_cnt << std::endl;

    std::cout << "main finish" << std::endl;
    return 0;
}
