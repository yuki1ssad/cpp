/*
    condition_variable、wait、notify_one、notify_all
    1、
        condition_variable, 和互斥量配合工作
        my_cond.wait() // 第一个参数：条件变量，第二个参数：如果没有默认false；如果有返回false，那么wait()将解锁互斥量并阻塞在该行，如果返回true，则继续往后执行。
        my_cond.notify_one(); // 尝试将 wait my_cond 的线程唤醒

    2、notify_all() 通知所有线程
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

class A 
{
public:
    void inMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            std::cout << "\ninMsgRecvQueue insert an element " << std::endl;
            std::unique_lock<std::mutex> sbguard(my_mutex);
            msgRecvQueue.push_back(i);
            // my_cond.notify_one(); // 尝试将 wait my_cond 的线程唤醒
            my_cond.notify_all();
        }
    }

    void outMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            std::unique_lock<std::mutex> sbguard(my_mutex);
            my_cond.wait(sbguard, [this] { // 一个 lambda 就是一个可调用对象
                if (!msgRecvQueue.empty()) {
                    return true;
                }
                return false;
            });
            int command = msgRecvQueue.front();
            msgRecvQueue.pop_front();
            sbguard.unlock();
            std::cout << "\noutMsgRecvQueue run, successfully get an element, thread_id: " << std::this_thread::get_id() << std::endl;
        }
    }

private:
    std::list<int> msgRecvQueue;
    std::mutex my_mutex;
    std::condition_variable my_cond;
};

int main()
{
    A aobj;
    std::thread myOutMsgObj(&A::outMsgRecvQueue, &aobj);
    std::thread myOutMsgObj2(&A::outMsgRecvQueue, &aobj);
    std::thread myInMsgObj(&A::inMsgRecvQueue, &aobj);
    myOutMsgObj.join();
    myOutMsgObj2.join();
    myInMsgObj.join();

    // 互斥量



    std::cout << "hhh" << std::endl;
}