#include <iostream>
#include <map>
#include <functional>
#include <set>

//
// 数据和容器
//
struct Item {
    uint32_t id;
    std::string name;
    uint32_t data[1024];
};

std::map<uint32_t, Item> items = {
        {1,  {1,  "David1"}},
        {6,  {6,  "David6"}},
        {8,  {8,  "David8"}},
        {11, {11, "David11"}},
        {13, {13, "David13"}},
        {15, {15, "David15"}},
        {17, {17, "David17"}},
        {22, {22, "David22"}},
        {25, {25, "David25"}},
        {27, {27, "David27"}},
};

void print_items(const std::string &title) {
    std::cout << title << "--------------" << std::endl;
    for (auto it = items.begin(); it != items.end(); ++it) {
        std::cout << it->second.id << " - " << it->second.name << std::endl;
    }
}

/////////////////////////////////////////////////////////
//
// 问题：清理掉键值为偶数的元素
//
/////////////////////////////////////////////////////////

// 宕机
void for_crash_1() {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->first % 2 == 0)
            items.erase(it);
    }
}

// C++11之前的写法
void for_standard_1() {
    for (auto it = items.begin(); it != items.end();) {
        if (it->first % 2 == 0)
            items.erase(it++);
        else
            it++;
    }
}

// C++11之后的写法
void for_standard_2() {
    for (auto it = items.begin(); it != items.end();) {
        if (it->first % 2 == 0)
            it = items.erase(it);
        else
            it++;
    }
}

/////////////////////////////////////////////////////////
//
// 问题：循环体里面操作当前遍历的容器？
//     - 读写：读和写当前的元素
//     - 添加：正在遍历之前？之后？
//     - 删除：删除在遍历的？之前的？之后的？
//     - 清空：遍历清空整个容器？
//
/////////////////////////////////////////////////////////

// 遍历时添加元素
void for_add() {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->first == 8) {
            // 当前节点之后添加
            items[9].name = "David9";
            items[9].id = 9;

            // 当前节点之前添加
            items[7].name = "David7";
            items[7].id = 7;
        }

        std::cout << "loop: " << it->first << "-"
                  << it->second.name << std::endl;
    }
}

// 遍历时删除元素
void for_erase() {
    for (auto it = items.begin(); it != items.end(); ++it) {

        if (it->first == 8) {
            items.erase(6);
            items.erase(11);

//            items.erase(8); // 宕机：删除了当前元素，迭代器失效
//            items.clear();  // 宕机：直接清空时也删除了当前元素
        }
    }
}

/////////////////////////////////////////////////////////
//
// 问题：写一个遍历当前所有元素，并进行某种操作的循环？
//
/////////////////////////////////////////////////////////


// cb删除当前迭代的元素会有异常
void foreach_crash(const std::function<void(Item &)> &cb) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        cb(it->second);
    }
}

// cb删除当前迭代的紧接着的元素会有异常
void foreach_crash2(const std::function<void(Item &)> &cb) {
    for (auto it = items.begin(); it != items.end();) {
        auto tmp = it++;
        cb(tmp->second);
    }
}

// 解决方法1：复制正在遍历的容器
void foreach_1(const std::function<void(Item &)> &cb) {
    std::map<uint32_t, Item> copy = items;
    for (auto it = copy.begin(); it != copy.end(); ++it) {
        cb(it->second);
    }
}

// 解决方法2：建立键值容器，然后根据键值查找
// 1. 多了循环时间和键值的存储空间浪费
// 2. cb对容器添加的元素不进行处理
// 3. cb添加或删除、甚至清空对不影响
void foreach_2(const std::function<void(Item &)> &cb) {
    std::set<uint32_t> keys;
    for (auto &it : items)
        keys.insert(it.first);

    for (auto key : keys) {
        auto it = items.find(key);
        if (it != items.end())
            cb(it->second);
    }
}

// 解决方法3：每次循环时根据键值找下一个元素的迭代器
// great (n * lgn)
void foreach_3(const std::function<void(Item &)> &cb) {
    for (auto it = items.begin(); it != items.end();) {
        uint32_t key = it->first;
        cb(it->second);
        it = items.upper_bound(key);
    }
}

// 解决方法4：遵守约定，遍历容器的回调不要进行删除/清空操作！！


// 示例：
void for_crash_2() {
    foreach_crash([](const Item &item) {
        if (item.id == 8) {
            items.erase(8); // 删掉当前的
        }
    });
}

void for_crash_3() {
    foreach_crash2([](const Item &item) {
        if (item.id == 8) {
            items.erase(11); // 删掉下一个
        }
    });
}

void for_1() {

    foreach_3([](const Item &item) {
        if (item.id == 8) {
            items.erase(8);  // 删掉当前的
            items.erase(11); // 删掉下一个
//            items.clear();
        }
    });
}


int main(int argc, char *argv[]) {

    std::string op = "1";
    if (argc > 1) {
        op = argv[1];
    }

    print_items("BEFORE");

    if ("standard_1" == op) {
        for_standard_1();
    } else if ("standard_2" == op) {
        for_standard_2();
    } else if ("crash_1" == op) {
        for_crash_1();
    } else if ("crash_2" == op) {
        for_crash_2();
    } else if ("crash_3" == op) {
        for_crash_3();
    } else if ("add" == op) {
        for_add();
    } else if ("erase" == op) {
        for_erase();
    } else if ("1" == op) {
        for_1();
    }

    print_items("AFTER");
}