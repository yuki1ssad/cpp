/*
    unique_lock 详解
    1、unique_lock 取代 lock_guard
        1.1 unique_lock 是个类模板，工作中，一般用 lock_guard；
        1.2 unique_lock 比 lock_guard 灵活很多，效率上差一点，内存占用多一点

    2、unique_lock 第二个参数
        2.1 std::adopt_lock：表示这个互斥量已经提前 lock 了
        2.2 std::try_to_lock：尝试用 mutex 的 lock() 去锁定这个 mutex， 如果没有锁定成功，会立即返回(去干点别的事)而不是阻塞。用 try_to_lock 之前不能用 lock()
            std::unique_lock<std::mutex> sbguard(my_mutex, std::try_to_lock);
            { // 会卡死
                my_mutex.lock();
                std::unique_lock<std::mutex> sbguard(my_mutex, std::try_to_lock);
            }
        2.3 std::defer_lock：初始化一个没有加锁的 mutex，与用 try_to_lock 一样，之前不能用 lock()

    3、unique_lock 成员函数
        lock()  加锁
        unlock()    解锁
        try_lock()  尝试给互斥量加锁，成功返回 true，失败返回 false
        release()   返回它所管理的 mutex 对象指针，并释放所有权，也就是这个 unique_lock 和这个 mutex 没有关系了
            {
                std::unique_lock<std::mutex> sbguard(my_mutex);
                std::mutex *ptr = sbguard.release(); // ptr 是指向 my_mutex 的指针
                ...
                ptx->unlock(); // 自己负责 my_mutex 的解锁
            }
    4、unique_lock 所有权的传递
        4.1 std::move()
            std::unique_lock<std::mutex> sbguard(my_mutex);
            // sbguard 拥有 my_mutex 的所有权；可以转移不能复制。 
                std::unique_lock<std::mutex> sbguard1(std::move(sbguard)); // sbguard1 指向 my_mutex， sbguard 指向空
        4.2 return std::unique_lock<std::mutex> (返回函数局部变量，系统会调用移动构造函数创建临时变量)
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
            std::unique_lock<std::mutex> sbguard(my_mutex, std::defer_lock); // 没有加锁的 my_mutex
            // sbguard.lock(); // 加锁，不用自己 unlock, 也可以用 sbguard.unlock(); 解锁
            if (sbguard.try_lock()) 
            {
                // 拿到锁了
            } 
            else 
            {
                // 没拿到锁
            }

            // std::lock_guard<std::mutex> sbguard(my_mutex);
            msgRecvQueue.push_back(i);
        }
    }

    void outMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            // my_mutex.lock();
            std::unique_lock<std::mutex> sbguard(my_mutex, std::try_to_lock);
            if (sbguard.owns_lock()) {
                // 拿到了锁
            }

            // std::chrono::milliseconds dura(20000); // 20 秒
            // std::this_thread::sleep_for(dura);
            // std::lock_guard<std::mutex> sbguard(my_mutex); // 局部变量。 1. lock_guard 构造函数里执行了 mutex::lock() 2. lock_guard 析构函数里执行了 mutex::unlock()
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