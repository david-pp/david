#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>

void callback() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

struct Callback {
    void operator()() const {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

void test_1() {
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    Callback cb;

    std::thread t1(callback);
    std::thread t2(cb);
    std::thread t3{Callback()};  // std::thread t3(CallBack()) 会被解析为函数声明
    std::thread t4([]() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

int addOne(int value)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    return ++value;
}

class Wallet {
    int mMoney;
    std::mutex mutex;
public:
    Wallet() : mMoney(0) {}

    int getMoney() { return mMoney; }

    void addMoney(int money) {
        for (int i = 0; i < money; ++i) {
            std::lock_guard<std::mutex> lock(mutex);
//            mutex.lock();
            mMoney = addOne(mMoney);
//            mutex.unlock();
        }
    }
};

int testMultithreadedWallet() {
    Wallet walletObject;
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread(&Wallet::addMoney, &walletObject, 10));
    }

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    return walletObject.getMoney();
}

void test_2() {
    int val = 0;
    for(int k = 0; k < 10; k++)
    {
        if((val = testMultithreadedWallet()) != 50)
        {
            std::cout << "Error at count = "<<k<<" Money in Wallet = "<<val << std::endl;
        }
    }
}

int main() {
    test_2();
}
