#include <cstring>
#include <cstdlib>
#include <list>
#include <string>
#include <iostream>

#include <zookeeper.h>
#include "zoo_lock.h"


using namespace std;

static void yield(zhandle_t *zh, int i)
{
    sleep(i);
}

typedef struct evt {
    string path;
    int type;
} evt_t;

typedef struct watchCtx {
private:
    list<evt_t> events;
public:
    bool connected;
    zhandle_t *zh;
    
    watchCtx() {
        connected = false;
        zh = 0;
    }
    ~watchCtx() {
        if (zh) {
            zookeeper_close(zh);
            zh = 0;
        }
    }

    evt_t getEvent() {
        evt_t evt;
        evt = events.front();
        events.pop_front();
        return evt;
    }

    int countEvents() {
        int count;
        count = events.size();
        return count;
    }

    void putEvent(evt_t evt) {
        std::cout << "........EVETN:" << evt.path << " " << evt.type << std::endl;
        events.push_back(evt);
    }

    bool waitForConnected(zhandle_t *zh) {
        time_t expires = time(0) + 10;
        while(!connected && time(0) < expires) {
            yield(zh, 1);
        }
        return connected;
    }
    bool waitForDisconnected(zhandle_t *zh) {
        time_t expires = time(0) + 15;
        while(connected && time(0) < expires) {
            yield(zh, 1);
        }
        return !connected;
    }
} watchctx_t; 

static void watcher(zhandle_t *, int type, int state, const char *path,void*v)
{
    watchctx_t *ctx = (watchctx_t*)v;

    if (state == ZOO_CONNECTED_STATE) {
        ctx->connected = true;
    } else {
        ctx->connected = false;
    }
    if (type != ZOO_SESSION_EVENT) {
        evt_t evt;
        evt.path = path;
        evt.type = type;
        ctx->putEvent(evt);
    }
}

static char hostPorts[] = "127.0.0.1:2182";

const char *getHostPorts() {
    return hostPorts;
}

zhandle_t *createClient(watchctx_t *ctx) {
    zhandle_t *zk = zookeeper_init(hostPorts, watcher, 10000, 0,
                                   ctx, 0);
    ctx->zh = zk;
    sleep(1);
    return zk;
}


class ZooLock 
{
public:
    static void lock_completion (int rc, void* cbdata) 
    {
        ZooLock* zlock = (ZooLock*)cbdata;

        if (rc == 0)
            zlock->onLocked();
        else if (rc == 1)
            zlock->onUnlocked();
    }

public:
    ZooLock(zhandle_t* zh, const char* path)
    {
        zkr_lock_init_cb(&mutex_, zh, (char*)path, &ZOO_OPEN_ACL_UNSAFE, lock_completion, this);
    }

    ~ZooLock()
    {
        zkr_lock_destroy(&mutex_);
    }

    void lock()
    {
        zkr_lock_lock(&mutex_);

        while (!islocked()) {
            //TODO: sleep
            zkr_lock_lock(&mutex_);
        }
    }

    void unlock()
    {
        zkr_lock_unlock(&mutex_);
    }

    bool islocked()
    {
        return zkr_lock_isowner(&mutex_); 
    }


    void trylock()
    {
        zkr_lock_lock(&mutex_);
    }

    virtual void onLocked() 
    {
    }

    virtual void onUnlocked() {}

private:
    zkr_lock_mutex_t mutex_;
};

void test_1() 
{
    watchctx_t ctx;
    int rc;
    struct Stat stat;
    char buf[1024];
    int blen;
    struct String_vector strings;
    const char *testName;
    zkr_lock_mutex_t mutexes[3];
    
    int count = 3;
    int i = 0;
    char* path = "/test-lock";
    for (i=0; i< 3; i++) {
        zhandle_t *zh = createClient(&ctx);
        zkr_lock_init(&mutexes[i], zh, path, &ZOO_OPEN_ACL_UNSAFE);
        zkr_lock_lock(&mutexes[i]);
    }

   
    sleep(3);

    zkr_lock_mutex leader = mutexes[0];
    zkr_lock_mutex mutex;
    int ret = strcmp(leader.id, leader.ownerid);

    std::cout << leader.id << " " << leader.ownerid << std::endl;
   
    for(i=1; i < count; i++) {
        mutex = mutexes[i];
    } 
    zkr_lock_unlock(&leader);
    sleep(3);
    zkr_lock_mutex secondleader = mutexes[1];
   
    for (i=2; i<count; i++) {
        mutex = mutexes[i];
    }
}

void test_lock()
{
    watchctx_t ctx;
    zhandle_t *zh = createClient(&ctx);

    std::cout << "=======================" << std::endl;

    ZooLock zlock(zh, "/lock-test");

    zlock.lock();

    int count = 0;
    while (count++ < 10) {
        std::cout << count << ".dosomething ...." << std::endl;
        sleep(1);
    }

    zlock.unlock();

    sleep(10);
}

void doSomething(const char* sth, int maxcount)
{
    int count = 0;
    while (count++ < maxcount) {
        std::cout << count << ". " << sth << " .... thread:" << pthread_self() << std::endl;
        sleep(1);
    }
}

void test_trylock()
{
    watchctx_t ctx;
    zhandle_t *zh = createClient(&ctx);

    std::cout << "=======================" << std::endl;

    struct TryLockTest : public ZooLock
    {
        TryLockTest(zhandle_t* zh, const char* path):ZooLock(zh, path) {}

        void onLocked()
        {
            doSomething("dosomething when locked", 10);

            this->unlock();
        }
    };

    TryLockTest zlock(zh, "/lock-test");
    zlock.trylock();

    doSomething("dosomething trylock", 20);
}

int main(int argc, const char* argv[]) 
{
    if (argc > 1)
        strcpy(hostPorts, argv[1]);
    //zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
    //test_lock();
    test_trylock();
}