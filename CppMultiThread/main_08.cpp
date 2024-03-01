/*
    async、future、packaged_task、promise
    1、std::async 用于启动异步任务，返回一个 std::future 对象。
        std::future<int> result = std::async(mythread); // 绑定关系，单线程并不卡在这
        线程运行结束的时候，线程运行返回的结果可以用 std::future 的 get 成员函数获得。
            注意：get() 不能多次调用，第一次之后的调用会报错
                wait() 函数只阻塞，不返回值，类似于 join()
            有意思的是，即便不调用 get 或是 wait，主线程仍然会等子线程运行完再结束。
        
         std::future<int> result = std::async(std::lanuch::deferred, mythread);
        std::lanuch::deferred 表示线程会延迟到 std::future 的 get 或 wait 执行，如果没有调用，则不会运行该子线程
    2、std::packaged_task 打包任务。是个类模板，参数是可调用对象
        通过 std::packaged_task 把各种可调用对象包装起来，方便将来作为线程入口
        {
            std::packaged_task<int(int)> mypt(mythread); // int(int) 是因为 mythread 函数的返回值类型是 int， 参数类型是 int
            std::thread t1(std::ref(mypt), 1); // 1 是要传递给 mythread 函数的值
            t1.join();
            std::future<int> result = mypt.get_future();
            std::cout << result.get() << std::endl;
        }
    3、std::promise 类模板
        在某个线程中给它赋值，可以在其他线程中将值取出来
        {
            // 定义函数
            void mythread(std::promise<int> &tmpp, int calc) // 注意第一个参数
            {
                calc++;
                ...
                int result = calc;
                tmpp.set_value(result); // 将结果保存到 tmpp 对象中
                return;
            }

            // 定义函数
            void mythread2(std::future<int> &tmpf)
            {
                auto result = tmpf.get();
                return;
            }


            int main()
            {
                std::promise<int> myprom; // 声明一个 std::promise 对象 myprom，保存的值类型为 int
                std::thread t1(mythread, std::ref(myprom), 180);
                t1.join();

                // 获取结果
                std::future<int> fu1 = myprom.get_future();
                auto result = fu1.get();
                std::cout << "result = " << result << std::endl;
                return 0;
            }
        }

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
    sleep(5);
    return 5;
}

int main()
{
    std::cout << "main_thread_id: " << std::this_thread::get_id() << std::endl;
    std::future<int> result = std::async(mythread); // 绑定关系，单线程并不卡在这
    std::cout << "contunuing...." << std::endl;
    int def = 0;
    // std::cout << "result: " << result.get() << std::endl; // 卡在这里等待执行 mythread 的线程执行完
    result.wait(); // 卡在这里等待执行 mythread 的线程执行完
    return 0;
}