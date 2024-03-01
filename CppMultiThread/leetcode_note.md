## 1114. 按序打印
给你一个类：
```
public class Foo {
  public void first() { print("first"); }
  public void second() { print("second"); }
  public void third() { print("third"); }
}
```
三个不同的线程 A、B、C 将会共用一个 Foo 实例。

### 使用 semaphore.h
```cpp
/*
    C语言中的 Semaphore
    信号量类型为 sem_t，类型及相关操作定义在头文件 semaphore.h 中，
    int sem_init(sem_t *sem, int pshared, unsigned int value);  // 创建信号量
    int sem_post(sem_t *sem);  // 信号量的值加 1
    int sem_wait(sem_t *sem);  // 信号量的值减 1
    int sem_destroy(sem_t *sem);  // 信号量销毁
*/
#include <semaphore.h>

class Foo {

    sem_t firstJobDone;
    sem_t secondJobDone;

public:

    Foo() {
        sem_init(&firstJobDone, 0, 0);
        sem_init(&secondJobDone, 0, 0);
    }

    void first(function<void()> printFirst) {
        // printFirst() outputs "first".
        printFirst();
        sem_post(&firstJobDone);
    }

    void second(function<void()> printSecond) {
        sem_wait(&firstJobDone);
        // printSecond() outputs "second".
        printSecond();
        sem_post(&secondJobDone);
        
    }

    void third(function<void()> printThird) {
        sem_wait(&secondJobDone);
        // printThird() outputs "third".
        printThird();
    }
};
```

### mutex 结合 condition_variable
```cpp
// 成功提交
class Foo {
    std::mutex m;
    std::condition_variable cv1;
    std::condition_variable cv2;
    bool b_flag = false;
    bool c_flag = false;

public:
    Foo() {
    }

    void first(function<void()> printFirst) {
        std::unique_lock<std::mutex> lk(m);
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        b_flag = true;
        lk.unlock();
        cv1.notify_one();
    }

    void second(function<void()> printSecond) {
        std::unique_lock<std::mutex> lk(m);
        cv1.wait(lk, [this]{return b_flag;});
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        b_flag = false;
        c_flag = true;
        lk.unlock();
        cv2.notify_one();
    }

    void third(function<void()> printThird) {
        std::unique_lock<std::mutex> lk(m);
        cv2.wait(lk, [this]{return c_flag;});
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
        c_flag = false;
        lk.unlock();
    }
};

//  (有时成功有时失败)，为啥一个条件变量不可以呢
class Foo {
    std::mutex m;
    std::condition_variable cv;
    bool b_flag = false;
    bool c_flag = false;

public:
    Foo() {
    }

    void first(function<void()> printFirst) {
        std::unique_lock<std::mutex> lk(m);
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        b_flag = true;
        cv.notify_one();
    }

    void second(function<void()> printSecond) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]{return b_flag;});
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        b_flag = false;
        c_flag = true;
        cv.notify_one();
    }

    void third(function<void()> printThird) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]{return c_flag;});
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
        c_flag = false;
    }
};
```

### 使用 counting_semaphore （c++20才支持信号量）
```cpp
class Foo {
    std::counting_semaphore<1> a{1}; // 最大信号量为1，可用初始值为1
    std::counting_semaphore<1> b{0}; // 最大信号量为1，可用初始值为0
    std::counting_semaphore<1> c{0}; // 最大信号量为1，可用初始值为0

public:
    Foo() {
    }

    void first(function<void()> printFirst) {
        a.acquire();
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        b.release();
    }

    void second(function<void()> printSecond) {
        b.acquire();
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        c.release();
    }

    void third(function<void()> printThird) {
        c.acquire();
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};
```

## 1115. 交替打印 FooBar
给你一个类：
```
class FooBar {
  public void foo() {
    for (int i = 0; i < n; i++) {
      print("foo");
    }
  }

  public void bar() {
    for (int i = 0; i < n; i++) {
      print("bar");
    }
  }
}
```
两个不同的线程将会共用一个 FooBar 实例：

线程 A 将会调用 foo() 方法，而
线程 B 将会调用 bar() 方法
请设计修改程序，以确保 "foobar" 被输出 n 次。

### 使用 semaphore.h
```cpp
#include <semaphore.h>
class FooBar {
private:
    int n;
    sem_t sem_foo;
    sem_t sem_bar;

public:
    FooBar(int n) {
        this->n = n;
        sem_init(&sem_foo, 0, 1);
        sem_init(&sem_bar, 0, 0);
    }

    void foo(function<void()> printFoo) {
        
        for (int i = 0; i < n; i++) {
            sem_wait(&sem_foo);
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            sem_post(&sem_bar);
        }
    }

    void bar(function<void()> printBar) {
        
        for (int i = 0; i < n; i++) {
            sem_wait(&sem_bar);
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            sem_post(&sem_foo);
        }
    }
};
```

### mutex 结合 condition_variable
```cpp
class FooBar {
private:
    int n;
    std::mutex m;
    std::condition_variable cv_foo;
    std::condition_variable cv_bar;
    bool flag_foo = true;
    bool flag_bar = false;

public:
    FooBar(int n) {
        this->n = n;
    }

    void foo(function<void()> printFoo) {
        
        for (int i = 0; i < n; i++) {
            std::unique_lock<std::mutex> lk(m);
            cv_foo.wait(lk, [this]{return flag_foo;});
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            flag_bar = true;
            flag_foo = false;
            cv_bar.notify_one();
        }
    }

    void bar(function<void()> printBar) {
        
        for (int i = 0; i < n; i++) {
            std::unique_lock<std::mutex> lk(m);
            cv_bar.wait(lk, [this]{return flag_bar;});
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            flag_foo = true;
            flag_bar = false;
            cv_foo.notify_one();
        }
    }
};
```

### 使用 counting_semaphore （c++20才支持信号量）
```cpp
class FooBar {
private:
    int n;
    std::counting_semaphore<1> sem_foo{1};
    std::counting_semaphore<1> sem_bar{0};

public:
    FooBar(int n) {
        this->n = n;
    }

    void foo(function<void()> printFoo) {
        
        for (int i = 0; i < n; i++) {
            sem_foo.acquire();
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            sem_bar.release();
        }
    }

    void bar(function<void()> printBar) {
        
        for (int i = 0; i < n; i++) {
            sem_bar.acquire();
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            sem_foo.release();
        }
    }
};
```

## 1116. 打印零与奇偶数
现有函数 printNumber 可以用一个整数参数调用，并输出该整数到控制台。

例如，调用 printNumber(7) 将会输出 7 到控制台。
给你类 ZeroEvenOdd 的一个实例，该类中有三个函数：zero、even 和 odd 。ZeroEvenOdd 的相同实例将会传递给三个不同线程：

线程 A：调用 zero() ，只输出 0
线程 B：调用 even() ，只输出偶数
线程 C：调用 odd() ，只输出奇数
修改给出的类，以输出序列 "010203040506..." ，其中序列的长度必须为 2n 。

实现 ZeroEvenOdd 类：

ZeroEvenOdd(int n) 用数字 n 初始化对象，表示需要输出的数。
void zero(printNumber) 调用 printNumber 以输出一个 0 。
void even(printNumber) 调用printNumber 以输出偶数。
void odd(printNumber) 调用 printNumber 以输出奇数。
 

示例 1：

输入：n = 2
输出："0102"
解释：三条线程异步执行，其中一个调用 zero()，另一个线程调用 even()，最后一个线程调用odd()。正确的输出为 "0102"。
示例 2：

输入：n = 5
输出："0102030405"

### binary_semaphore
关键点：对于 n 的奇偶性处理
```cpp
class ZeroEvenOdd {
private:
    int n;
    std::binary_semaphore sem_z{1}, sem_e{0}, sem_o{0};
    // int i = 1;
    std::atomic<int> i{0};

public:
    ZeroEvenOdd(int n) {
        this->n = n;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        while (i < this->n) {
            // std::cout << "in zero , i = " << i << std::endl;
            sem_z.acquire();
            printNumber(0);
            i++;
            if (i % 2) {
                sem_o.release();
            } else {
                sem_e.release();
            }
        }
        return;
    }

    void even(function<void(int)> printNumber) {
        if (this->n % 2) {
            while (i < this->n - 1) {
                sem_e.acquire();
                // std::cout << " even, i = " << i << std::endl;
                printNumber(i);
                sem_z.release();
            }
        } else {
            while (i < this->n) {
                sem_e.acquire();
                // std::cout << " even, i = " << i << std::endl;
                printNumber(i);
                sem_z.release();
            }
        }
            
    }

    void odd(function<void(int)> printNumber) {
        if (this->n % 2) {
            while(i < this->n) {
                sem_o.acquire();
                // std::cout << " ood, i = " << i << std::endl;
                printNumber(i);
                sem_z.release();
            }
        } else {
            while(i < this->n - 1) {
                sem_o.acquire();
                // std::cout << " ood, i = " << i << std::endl;
                printNumber(i);
                sem_z.release();
            } 
        }
        
    }
};
```

参考网上的关于奇偶性的另一种处理方法
```cpp 
class ZeroEvenOdd {
private:
    int n;
    std::binary_semaphore sem_z{1}, sem_e{0}, sem_o{0};

public:
    ZeroEvenOdd(int n) {
        this->n = n;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        for (int i = 1; i <= this->n; i++) {
            sem_z.acquire();
            printNumber(0);
            if (i % 2) {
                sem_o.release();
            } else {
                sem_e.release();
            }
        }
        return;
    }

    void even(function<void(int)> printNumber) {
        for (int j = 2; j <= this->n; j += 2) {
            sem_e.acquire();
            printNumber(j);
            sem_z.release();
        }
            
    }

    void odd(function<void(int)> printNumber) {
        for (int j = 1; j <= this->n; j += 2) {
            sem_o.acquire();
            printNumber(j);
            sem_z.release();
        }
        
    }
};
```

## 1117. H2O 生成
生产者-消费者解法
把 hydrogen 线程看作生产者，oxygen 线程看作消费者，缓冲队列大小为2。

hydrogen 把生成的氢放入队列；oxygen 线程每次从队列里消费两个氢元素。

生产者生产两个氢元素后会因为缓冲队列满而阻塞，使用条件变量使生产者阻塞。

消费者只有当缓冲队列满时才会从缓冲队列中消费元素，缓冲队列未满时消费者阻塞。

当生产者生成的氢元素填满缓冲队列时，生产者唤醒被阻塞的消费者；当消费者消费掉缓冲队列的元素后，消费者唤醒被阻塞的生产者。

由于不需要对队列中的氢元素做操作，我们只需维护缓冲队列的大小即可。

缺点：这种解法只能产生 HHO 序列
```cpp
class H2O {
    int cnt_h = 0;
    std::condition_variable cv;
    std::mutex m;
public:
    H2O() {
        
    }

    void hydrogen(function<void()> releaseHydrogen) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]{return this->cnt_h < 2;});
        
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        releaseHydrogen();
        cnt_h++;
        if (cnt_h == 2) {
            cv.notify_all();
        }
    }

    void oxygen(function<void()> releaseOxygen) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]{return this->cnt_h == 2;});
        
        // releaseOxygen() outputs "O". Do not change or remove this line.
        releaseOxygen();
        cnt_h = 0;
        cv.notify_all();
    }
};
```

## 1195. 交替打印字符串

### mutex 结合 条件变量
要注意结束之后所有的线程都必须退出，否则会死锁，超出时间限制

```cpp 这个代码也不稳定，有时会报超出时间限制，但多提交几次就过了。。。
class FizzBuzz {
private:
    int n;
    std::mutex m;
    std::condition_variable cv;
    std::atomic<int> i{1};
    bool flag = false;

public:
    FizzBuzz(int n) {
        this->n = n;
    }

    // printFizz() outputs "fizz".
    void fizz(function<void()> printFizz) {
        while(i <= this->n) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [this]{return (i % 3 == 0) | flag;});
            if (i > this->n | flag) {
                flag = true;
                cv.notify_all();
                return;
            }
            printFizz();
            std::cout << "fizz, ";
            ++i;
            cv.notify_all();
        }
    }

    // printBuzz() outputs "buzz".
    void buzz(function<void()> printBuzz) {
        while(i <= this->n) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [this]{return (i % 5 == 0) | flag;});
            if (i > this->n | flag) {
                flag = true;
                cv.notify_all();
                return;
            }
            printBuzz();
            std::cout << "buzz, ";
            ++i;
            cv.notify_all();
        }
    }

    // printFizzBuzz() outputs "fizzbuzz".
	void fizzbuzz(function<void()> printFizzBuzz) {
        while(i <= this->n) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [this]{return (i % 5 == 0 && i % 3 == 0) | flag;});
            if (i > this->n | flag) {
                flag = true;
                cv.notify_all();
                return;
            }
            printFizzBuzz();
            std::cout << "fizzbuzz, ";
            ++i;
            cv.notify_all();
        }
    }

    // printNumber(x) outputs "x", where x is an integer.
    void number(function<void(int)> printNumber) {
       while(i <= this->n) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [this]{return (i % 5 != 0 && i % 3 != 0) | flag;});
            if (i > this->n  | flag) {
                flag = true;
                cv.notify_all();
                return;
            }
            printNumber(i);
            std::cout << i << ", ";
            ++i;
            cv.notify_all();
        } 
    }
};
```

## 1226. 哲学家进餐

### 互斥量，一次只保证一个哲学家进餐（低效）
```cpp
class DiningPhilosophers {
    std::mutex m;
public:
    DiningPhilosophers() {
        
    }

    void wantsToEat(int philosopher,
                    function<void()> pickLeftFork,
                    function<void()> pickRightFork,
                    function<void()> eat,
                    function<void()> putLeftFork,
                    function<void()> putRightFork) {
        m.lock();
        pickLeftFork();
		pickRightFork();
        eat();
        putRightFork();
        putLeftFork(); 
        m.unlock();
    }
};
```

### 控制最多只有四名哲学家同时拿叉子
```cpp
#include <semaphore.h>

class DiningPhilosophers {
    vector<sem_t> fork;
    sem_t room;
public:
    DiningPhilosophers() {
        fork = vector<sem_t>(5);
        sem_init(&room, 0, 4);
        for (int i = 0; i < 5; i++) {
            sem_init(&fork[i], 0, 1);
        }
    }

    void wantsToEat(int philosopher,
                    function<void()> pickLeftFork,
                    function<void()> pickRightFork,
                    function<void()> eat,
                    function<void()> putLeftFork,
                    function<void()> putRightFork) {
		sem_wait(&room);
        sem_wait(&fork[philosopher]);
        sem_wait(&fork[(philosopher + 1) % 5]);
        pickLeftFork();
        pickRightFork();
        eat();
        putRightFork();
        putLeftFork();
        sem_post(&fork[(philosopher + 1) % 5]);
        sem_post(&fork[philosopher]);
        sem_post(&room);
    }
};
```

### 奇偶号哲学家拿左右叉子的先后顺序不同
```cpp
#include <semaphore.h>

class DiningPhilosophers {
    vector<sem_t> fork;
public:
    DiningPhilosophers() {
        fork = vector<sem_t>(5);
        for (int i = 0; i < 5; i++) {
            sem_init(&fork[i], 0, 1);
        }
    }

    void wantsToEat(int philosopher,
                    function<void()> pickLeftFork,
                    function<void()> pickRightFork,
                    function<void()> eat,
                    function<void()> putLeftFork,
                    function<void()> putRightFork) {
        if (philosopher % 2) { // 奇数号哲学家先拿左边的叉子
            sem_wait(&fork[philosopher]);
            sem_wait(&fork[(philosopher + 1) % 5]);
            pickLeftFork();
            pickRightFork();
            eat();
            putRightFork();
            putLeftFork();
            sem_post(&fork[(philosopher + 1) % 5]);
            sem_post(&fork[philosopher]);
        } else { // 偶数号哲学家先拿右边的叉子
            sem_wait(&fork[(philosopher + 1) % 5]);
            sem_wait(&fork[philosopher]);
            pickLeftFork();
            pickRightFork();
            eat();
            putRightFork();
            putLeftFork();
            sem_post(&fork[philosopher]);
            sem_post(&fork[(philosopher + 1) % 5]);
        }
        
        
    }
};
```
