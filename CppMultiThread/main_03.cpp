/*
    创建多个线程、数据共享问题分析、案例代码
    1、创建和等待多个线程
        1.1 多个线程的运行是乱序的，跟操作系统的线程调度算法有关
        1.2 将对象放入 vector 等容器中，方便创建和管理多个线程

    2、数据共享问题分析
        2.1 只读的数据 ---> 安全，稳定
        2.2 有读有写 ---> 需要特别的处理手段防止崩溃
        2.3 其他案例 ---> 临界资源的访问
    3、共享数据的保护案例代码
        互斥量
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <unistd.h>
#include <list>

std::vector<int> shared_data = {1, 2, 3};

// 线程入口函数
void myprint(int inum)
{
    std::cout << "thread_num: " << inum << " myprint start run." << std::endl;

    std::cout << "thread_num: " << inum << " myprint finish run." << std::endl;
    return;
}

void myprint1(int inum)
{
    std::cout << "\nthread_id: " << std::this_thread::get_id() << " print shared_data: " << shared_data[0] << ", " << shared_data[1] << ", " << shared_data[2] << std::endl;
    return;
}

class A 
{
public:
    void inMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            std::cout << "\ninMsgRecvQueue insert an element " << std::endl;
            msgRecvQueue.push_back(i);
        }
    }

    void outMsgRecvQueue()
    {
        for (int i = 0; i < 100000; i++) 
        {
            if (!msgRecvQueue.empty())
            {
                // sleep(1);
                int command = msgRecvQueue.front(); // 返回第一个元素，但不检查第一个元素是否存在
                msgRecvQueue.pop_front();
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
};

int main()
{
    // // 1、创建和等待多个线程
    // std::vector<std::thread> mythreads;
    // for (int i = 0; i < 10; i++) 
    // {
    //     mythreads.push_back(std::thread(myprint, i)); // 创建 10 个线程，同时这 10 个线程已经开始执行
    // }

    // for (auto iter = mythreads.begin(); iter != mythreads.end(); iter++) 
    // {
    //     iter->join();
    // }

    // // 2、数据共享问题分析
    // // 2.1 只读的数据
    // std::vector<std::thread> mythreads1;
    // for (int i = 0; i < 10; i++) 
    // {
    //     mythreads1.push_back(std::thread(myprint1, i)); // 创建 10 个线程，同时这 10 个线程已经开始执行
    // }

    // for (auto iter = mythreads1.begin(); iter != mythreads1.end(); iter++) 
    // {
    //     iter->join();
    // }

    // 3、共享数据的保护案例代码
    A aobj;
    std::thread myOutMsgObj(&A::outMsgRecvQueue, &aobj);
    std::thread myInMsgObj(&A::inMsgRecvQueue, &aobj);
    myOutMsgObj.join();
    myInMsgObj.join();

    std::cout << "hhh" << std::endl;
}