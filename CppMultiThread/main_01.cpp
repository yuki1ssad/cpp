
/*
    线程启动、结束、创建线程、join、detach
    1、演示线程运行的开始和结束
        1.1 thread c++11支持
        1.2 join() 用法：线程名.join() -> 让主线程等待本线程执行结束
        1.3 detach() 用法：线程名.join() -> 使本线程和主线程脱离，当主线程结束后，如果本线程未结束，则被c++运行时库接管
        1.4 joinable() 当代码量上去了，可以在使用 join 或 detach 之前用 线程名.joinable() 判断是否可以 join 或 detach。 如果之前 join 或 detach 了， 就不能再调用 join 或 detach。
    2、 其他创建线程的手法
        2.1 用类， 以及一个问题范例（如果用了 detach， 主线程结束后，类里面使用了对main局部变量的引用或者指针会出问题，因为主线程结束后，main中的局部变量会被回收）
        2.2 用 lambda 表达式
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <unistd.h>


void myprint() 
{
    std::cout << "print start" << std::endl;

    // sleep(1);

    std::cout << "print end" << std::endl;

    return;
}

class TA
{
public:
    int &m_i;
    TA(int &i) : m_i(i)
    {
        std::cout << "TA 构造函数被执行------------" << std::endl;
    }

    TA(const TA &ta) : m_i(ta.m_i)
    {
        std::cout << "TA 拷贝构造函数被执行***********" << std::endl;
    }

    ~TA()
    {
        std::cout << "TA 析构函数被执行===========" << std::endl;
    }

    void operator()()  // 重载 () ----> 仿函数 ----> 可调用对象
    {
        std::cout << "operator start" << std::endl;
        std::cout << "operator end" << std::endl;
    }
};


int main() 
{
    // std::cout << "hhh" << std::endl;

    // 1. myprint: 可调用函数
    std::thread mytobj(myprint);  // 1. 创建了子线程 myprint， 线程执行入口 myprint() 2. myprint 线程开始执行

    std::cout << "Before: mytobj.joinable() is " << mytobj.joinable() << std::endl;
    mytobj.join();  // 主线程阻塞在这里等待子线程 myprint
    // mytobj.detach(); //  主线程结束后， mytobj线程会被c++运行时库接管， 一旦调用了 detach， 就不能再 join 了

    std::cout << "After: mytobj.joinable() is " << mytobj.joinable() << std::endl;


    // 2. 用类创建子线程
    int myi = 6;
    TA ta(myi);
    std::thread mytobj1(ta); // ta: 可调用对象
    mytobj1.join();
    // mytobj1.detach();


    // 3. 用 lamda 表达式创建子线程
    auto mylamthread = []() {
        std::cout << "thread 3 start" << std::endl;

        std::cout << "thread 3 end" << std::endl;
    };

    std::thread mytobj2(mylamthread);
    mytobj2.join();

    // sleep(1);
    std::cout << "hhh" << std::endl;
    std::cout << "hhh" << std::endl;
    std::cout << "hhh" << std::endl;
    return 0;
}