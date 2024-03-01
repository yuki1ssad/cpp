/*
    线程传参详解，detach()大坑，成员函数做线程函数
    1、传递临时对象作为线程参数
        1.1 ===void myprint(const int &i, char *pmybuf);
            // 1. 这里的 mvar 的传参方式是复制后的值传递，即便主线程 detach 了子线程，主线程结束后，子线程中的 mvar 的使用仍然是安全的
            // 2. 对于参数 mybuf， 由于是指针的值传递，myprint 函数中的 mybuf 地址和 main 函数中的 mybuf 地址是一样的，所以如果 detach 了，主线程结束后，子线程中的 mybuf 的使用是不安全的
            // 3. 总结：不建议传引用，不能传指针
        1.2 ===void myprint(const int i, const string &pmybuf);
            // 执行 std::thread mytobj(myprint, mvar, mybuf); 时，mybuf 是什么时候被转成 string 的？ -----> 有可能是在 main 函数执行完之后（不安全）
            // 修改后 -> string(mybuf)构造临时 string 对象 (ok)
            // 在创建线程的同时构造临时对象的方法传递参数是可行的；
        1.3 总结（遇到有 detach 的情况）：
            A： 若传递 int 这种简单的类型参数，建议值传递，不要引用。
            B： 如果传递类对象，避免隐式类型转换。全部都在创建线程这一行就构建临时对象。然后在函数参数里用引用来接收参数。（windows(不用引用会有两次拷贝构造)和linux(用不用都是两次拷贝构造)有点不一样）。
            C：终极结论 -> 建议不用 detach， 只使用 join， 这样就不存在 main 函数中的局部变量失效导致子线程对内存的非法引用问题。
        1.4 通过结合线程id查看，发现，
            // 使用隐式类型转换时，临时对象是在子线程中构造的；（不安全）
            // 如果是在创建线程的同时构造临时对象，则该临时对象是在主线程中构造的。（安全）

    3、传递类对象、智能指针作为线程参数
        3.1 类对象
            std::thread mytobj2(aprint2, main_obj); --> 在子线程中修改对象成员变量的值不会影响 main 函数里的   (这种情况创建子线程使用的是拷贝构造函数)
            如果想要修改能够影响，则用 std::ref()函数    --> std::thread mytobj2(aprint2, std::ref(main_obj)); (这种情况创建子线程不会进行拷贝构造，而是真正的引用)
        3.2 智能指针
            std::thread mytobj3(aprint3, std::move(myp)); ---> 独占指针没有拷贝构造函数，所以用 std::move
            main 函数中的独占指针 myp.get() 和 aprint3 中 pzn.get() 是一样的

    4、用成员函数指针做线程函数
        A a4(10);
        std::thread mytobj4(&A::thread_work, a4, 15);

*/


#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <unistd.h>


void myprint(const int i, const std::string &pmybuf) 
{
    std::cout << "\ni: " << i << std::endl;

    // sleep(1);

    std::cout << "\npmybuf: " << pmybuf << std::endl;

    return;
}

class A
{
public:
    mutable int m_i;
    A(int a) : m_i(a) // 类型转换构造函数，将 int 转为 A 对象
    {
        std::cout << "thread_id = " << std::this_thread::get_id() << " A::A(int a) 构造函数被执行------------ this: " << this << std::endl;
    }

    A(const A &a) : m_i(a.m_i)
    {
        std::cout << "thread_id = " << std::this_thread::get_id() << " A::A(const A &a) 拷贝构造函数被执行*********** this: " << this << std::endl;
    }

    ~A()
    {
        std::cout << "thread_id = " << std::this_thread::get_id() << " A::~A 析构函数被执行=========== this: " << this << std::endl;
    }

    void thread_work(int num)
    {
        std::cout << "sub_threadwork run " << std::this_thread::get_id() << " this: " << this << std::endl;
    }
};

void aprint(const int i, const A &pmybuf)
{
    std::cout << "thread_id = " << std::this_thread::get_id() << " In aprint, pmybuf: " << &pmybuf << std::endl;
}

void aprint2(const A &pmybuf)
{
    pmybuf.m_i = 199; // 这里的修改不会影响 main 函数中的对应值
    std::cout << "thread_id = " << std::this_thread::get_id() << " In aprint2, pmybuf: " << &pmybuf << " m_i = " << pmybuf.m_i << std::endl;
}

void aprint3(std::unique_ptr<int> pzn)
{
    std::cout << "thread_id = " << std::this_thread::get_id() << " In aprint3, pzn: " << pzn.get() << std::endl;
}


int main() 
{
    std::cout << "main_thread_id = " << std::this_thread::get_id() << std::endl;
    // 一：传递临时对象作为线程参数
    int mvar = 1;
    int &mvary = mvar;
    char mybuf[] = "this is a test";
    // std::thread mytobj(myprint, mvar, mybuf); // ===void myprint(const int &i, char *pmybuf);
                                              // 1. 这里的 mvar 的传参方式是复制后的值传递，即便主线程 detach 了子线程，主线程结束后，子线程中的 mvar 的使用仍然是安全的
                                              // 2. 对于参数 mybuf， 由于是指针的值传递，myprint 函数中的 mybuf 地址和 main 函数中的 mybuf 地址是一样的，所以如果 detach 了，主线程结束后，子线程中的 mybuf 的使用是不安全的
                                              // 3. 总结：不建议传引用，不能传指针
                                              // ===void myprint(const int i, const string &pmybuf);
                                              // 执行 std::thread mytobj(myprint, mvar, mybuf); 时，mybuf 是什么时候被转成 string 的？ -----> 有可能是在 main 函数执行完之后（不安全）
    
    std::thread mytobj(myprint, mvar, std::string(mybuf)); // 修改后 -> string(mybuf)构造临时 string 对象 (ok)
                                                            // 在创建线程的同时构造临时对象的方法传递参数是可行的；
    mytobj.join();
    // mytobj.detach();
    
    int a_mvar = 1;
    int a_secondpart = 12;
    std::thread mytobj1(aprint, a_mvar, A(a_secondpart));
    mytobj1.join();
    // mytobj.detach();


    A main_obj(10);
    std::thread mytobj2(aprint2, std::ref(main_obj));
    mytobj2.join();

    std::unique_ptr<int> myp(new int(100));
    std::cout << "thread_id = " << std::this_thread::get_id() << " In main, before, myp: " << myp.get() << std::endl;
    std::thread mytobj3(aprint3, std::move(myp));
    mytobj3.join();

    A a4(10);
    std::thread mytobj4(&A::thread_work, a4, 15);
    mytobj4.join();

    std::cout << "thread_id = " << std::this_thread::get_id() << " In main, after, myp: " << &myp << std::endl;
    std::cout << "thread_id = " << std::this_thread::get_id() << " In main, pmybuf: " << &main_obj << " m_i = " << main_obj.m_i << std::endl;
    std::cout << "\nmain: hhh" << std::endl;
    return 0;
}