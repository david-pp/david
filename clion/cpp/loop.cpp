#include <iostream>
#include <map>
#include <functional>
#include <set>

template<typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
        typename _Alloc = std::allocator<std::pair<const _Key, _Tp> > >
class MyMap : public std::map<_Key, _Tp, _Compare, _Alloc> {
public:
    typedef std::map<_Key, _Tp, _Compare, _Alloc> Base;
    typedef typename Base::size_type size_type;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    using std::map<_Key, _Tp, _Compare, _Alloc>::map;

    size_type erase(const _Key &__x) {
        keys_.insert(__x);
    }

    iterator erase(iterator __position) {
        keys_.insert(__position->first);
        return ++__position;
    }

    bool is_valid(const _Key &key) const {
        return keys_.find(key) == keys_.end();
    }

    void clear() {
        for (auto it = this->begin(); it != this->end(); ++it)
            keys_.insert(it->first);
    }

    void real_clear() {
        for (auto it = this->begin(); it != this->end();) {
            if (!is_valid(it->first)) {
                keys_.erase(it->first);
                it = Base::erase(it);
            } else
                it++;
        }
    }

public:
    std::set<_Key, _Compare> keys_;
};

//#define MAP_IS_VALID(items, key) items.is_valid(key)

#define MAP_IS_VALID(items, key) true
/////////////////////////////////

struct Item {
    uint32_t id;
    std::string name;
    uint32_t bytes[1024];
};

std::map<uint32_t, Item> items = {
        {1, {1, "David"}},
        {2, {2, "David1"}},
        {3, {3, "David2"}},
        {4, {4, "David3"}},
        {5, {5, "David4"}},
        {7, {7, "David7"}},
        {9, {9, "David9"}},
};


// cb删除当前迭代的元素会有异常
void foreach_item(const std::function<void(Item &)> &cb) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (MAP_IS_VALID(items, it->first))
            cb(it->second);
    }
}

// cb删除当前迭代的紧接着的元素会有异常
void foreach_item2(const std::function<void(Item &)> &cb) {
    for (auto it = items.begin(); it != items.end();) {
        auto tmp = it++;
        cb(tmp->second);
    }
}

// 1. 多了循环时间和键值的存储空间浪费
// 2. cb对容器添加的元素不进行处理
// 3. cb添加或删除、甚至清空对不影响
void foreach_item3(const std::function<void(Item &)> &cb) {
    std::set<uint32_t> keys;
    for (auto &it : items) {
        keys.insert(it.first);
    }

    for (auto key : keys) {
        auto it = items.find(key);
        if (it != items.end()) {
            cb(it->second);
        }
    }
}

// great (n * lgn)
void foreach_item4(const std::function<void(Item &)> &cb) {
    for (auto it = items.begin(); it != items.end();) {
        uint32_t key = it->first;
        cb(it->second);
        it = items.upper_bound(key);
    }
}

//void foreach_item5(const std::function<void(Item &)> &cb) {
//    for (auto it = items.begin(); it != items.end();) {
//        uint32_t key = it->first;
//        cb(it->second);
//
//        if (items.find(key) == items.end())
//            it = items.upper_bound(key);
//        else
//            it++;
//    }
//}

void print_items(const std::string &title) {
    std::cout << title << "--------------" << std::endl;
    foreach_item([](Item &item) {
        std::cout << item.id << " - " << item.name << std::endl;
    });
}

/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////

void for_1() {
    for (auto it = items.begin(); it != items.end();) {
        if (it->first % 2 != 0) {
            items.erase(it);
        }
        it++;
    }

//    items.real_clear();
}

void for_2() {


    for (auto it = items.begin(); it != items.end();) {

        if (it->first == 7) {
            items[8].name = "David8";
            items[8].id = 8;

            items[6].name = "David6";
            items[6].id = 6;
        }


        std::cout << "loop : " << it->first << " - " << it->second.name << std::endl;

        it++;
    }
}

void for_3() {
    for (auto it = items.begin(); it != items.end();) {

        if (it->first == 2) {
            items.erase(1);
//            items.erase(2);
            items.erase(3);
            items.erase(4);
//            items.erase(5);
//            items.clear();
        }

        it++;
    }

}

void for_4() {
    foreach_item2([](Item &item) {
//        if (item.id %2 == 0)
//            items.erase(item.id);

        if (item.id == 2) {
//            items.erase(2);
            items.erase(3);
            items.erase(4);
        }
    });
}

void for_5() {
    foreach_item3([](const Item &item) {
//        if (item.id %2 == 0)
//            items.erase(item.id);

        if (item.id == 2) {
//            items.erase(2);
            items.erase(3);
            items.erase(4);
        }
    });
}

void for_6() {
    foreach_item4([](const Item &item) {
//        if (item.id %2 == 0)
//            items.erase(item.id);

        if (item.id == 2) {
//            items.erase(2);
//            items.erase(3);
//            items.erase(4);
            items.clear();
        }
    });
}

void for_7() {

    foreach_item4([](const Item &item) {
//        if (item.id %2 == 0)
//            items.erase(item.id);

        if (item.id == 2) {
            items.erase(3);
            items.erase(2);
        }

        std::cout << "**************" << item.id << std::endl;
        foreach_item([](const Item &item) {
            std::cout << "loop : " << item.id << " - " << item.name << std::endl;
        });
    });
}


int main() {

    print_items("1");

    // crash
    // for_1();
    for_1();


    print_items("2");
}