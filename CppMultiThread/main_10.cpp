/*
    std::atomic 续谈、std::async 深入谈
    1、std::atomic 续谈
        针对 ++ -- += &= |= 是正确的
        {
            // g_cnt++; // 正确
            // g_cnt += 1; // 正确
            // g_cnt = g_cnt + 1; // 结果不对
        }

    2、std::async 参数详述
        {
            std::async(std::launch::async, func)
            std::async(std::launch::deferred, func)
            std::async(std::launch::async | std::launch::deferred, func)
            std::async(func) // 同 std::async(std::launch::async | std::launch::deferred, func)，系统自行决定使用那种发射方式
        }
        std::launch::defered    // 延迟调用，不创建新线程，延迟到 future 对象调用 get() 或 wait() 时才执行入口函数；如果没调用，则不会执行入口函数。
        std::launch::asyc       // 强制异步任务在新线程上运行

    3、std::async 和 std::thread 的区别
        std::async() 一般不叫创建线程，而是创建一个异步任务
        std::thread() 如果系统资源紧张，那么就可能创建线程失败，执行 std::thread() 时整个程序可能奔溃；另外 std::thread() 不方便拿回线程入口函数返回值
        两者最明显的不同是，std::async() 有时并不会创建子线程，之前碰到过，可能是由主线程来对函数进行调用

    4、std::async 不确定性问题的解决
        不加发射参数的 async 调用，系统会自行决定是否创建新线程
        {
            std::future_status status = result.wait_for(0s);
            if (status == std::future_status::timeout) // 超时，表示线程还没执行完
            {
                std::cout << "timeout" << std::endl;

            }
            else if (status == std::future_status::ready) // 线程成功返回
            {
                std::cout << "ready" << std::endl;
            }
            else if (status == std::future_status::deferred) // 如果 std::async(std::launch::deferred, mythread);
            {                                                   // 这种情况下，不会创建新的子线程，而是在主线程中对 mythread 进行调用
                std::cout << "deferred" << std::endl;
                std::cout << result.get() << std::endl;
            }
        }
*/


#include <iostream>
#include <thread>
#include <atomic>
#include <future>

std::atomic<int> g_cnt(0);

void mythread()
{
    for (int i = 0; i < 100000; i++)
    {
        // g_cnt++; // 正确
        // g_cnt += 1; // 正确
        g_cnt = g_cnt + 1; // 结果不对
    }

    return;
}

int func()
{
    std::cout << "in func, thread_id: " << std::this_thread::get_id() << std::endl;
    return 5;
}

int main()
{
    // // std::atomic 续谈
    // std::thread mytobj1(mythread);
    // std::thread mytobj2(mythread);
    // mytobj1.join();
    // mytobj2.join();
    // std::cout << "g_cnt = " << g_cnt <<std::endl;
    // return 0;

    // std::async 参数详述
    std::cout << "main start, thread_id: " << std::this_thread::get_id() << std::endl;
    std::future<int> result = std::async(func);
    std::cout << "result = " << result.get() << std::endl;
}


