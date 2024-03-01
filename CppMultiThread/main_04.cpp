/*
    互斥量概念、用法、死锁演示
    1、互斥量 --> 是个类对象，理解为锁。多个线程尝试用 lock() 成员函数来对这把锁加锁，只有一个线程能成功上锁，没成功的线程卡在 lock() 这里不断地尝试上锁
        1.1 lock(), unlock() 要成对使用
        1.2 std::lock_guard类模板  
            防止程序员忘记 unlock()
            取代 lock(), unlock()， 用了 std::lock_guard 就不能使用 lock(), unlock()，反之亦然

            std::lock_guard<std::mutex> sbguard(my_mutex); 可以通过加 {} 限定作用域
            局部变量。 1. lock_guard 构造函数里执行了 mutex::lock() 2. lock_guard 析构函数里执行了 mutex::unlock()

    2、死锁
        2.1 产生死锁的前提条件，至少两把锁
            {
                thread1 
                {
                    mutex1.lock();
                    mutex2.lock();
                    ...
                    mutex2.unlock();
                    mutex1.unlock();

                }

                ...

                thread2
                {
                    mutex2.lock();
                    mutex1.lock();
                    ......
                    mutex1.unlock();
                    mutex2.unlock();

                }
            }
        2.2 死锁的一般解决方案：
            A: 保证所有互斥量的上锁顺序一致
        2.3 std::lock() 函数模板
            std::lock(mutex1, mutex2, ...); // 相当于每个 mutex 都调用了 .lock()
            一次锁住两个或两个以上的互斥量  (谨慎使用，建议一个一个锁)
            不存在因为顺序问题导致的死锁问题， 要么所有互斥量同时上锁，要么全部都不锁
            缺憾：没有对应的 std::unlock()， 需要手动 unlock()

            std::lock() 函数模板 结合 std::lock_guard类模板 
            {
                std::lock(mutex1, mutex2, ...);
                ...
                std::lock_guard<std::mutex> sbguard1(mutex1, std::adopt_lock); // std::adopt_lock 是个结构体对象，起标记作用，不用再 lock() 了
                std::lock_guard<std::mutex> sbguard1(mutex2, std::adopt_lock);
                ...
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


class A 
{
public:
    void inMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            std::cout << "\ninMsgRecvQueue insert an element " << std::endl;
            std::lock_guard<std::mutex> sbguard(my_mutex);
            // my_mutex.lock();
            msgRecvQueue.push_back(i);
            // my_mutex.unlock();
        }
    }

    void outMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            std::lock_guard<std::mutex> sbguard(my_mutex); // 局部变量。 1. lock_guard 构造函数里执行了 mutex::lock() 2. lock_guard 析构函数里执行了 mutex::unlock()
            // my_mutex.lock();
            if (!msgRecvQueue.empty())
            {
                int command = msgRecvQueue.front(); // 返回第一个元素，但不检查第一个元素是否存在
                msgRecvQueue.pop_front();
                std::cout << "\noutMsgRecvQueue run, successfully get an element" << std::endl;
            } 
            else 
            {
                std::cout << "\noutMsgRecvQueue run, but inMsgRecvQueue is empty" << std::endl;
            }
            // my_mutex.unlock();
        }
        std::cout << "\noutMsgRecvQueue finish run" << std::endl;
    }

private:
    std::list<int> msgRecvQueue;
    std::mutex my_mutex;
    // std::mutex my_mutex_1;
};

int main()
{
    A aobj;
    std::thread myOutMsgObj(&A::outMsgRecvQueue, &aobj);
    std::thread myInMsgObj(&A::inMsgRecvQueue, &aobj);
    myOutMsgObj.join();
    myInMsgObj.join();

    // 互斥量



    std::cout << "hhh" << std::endl;
}