#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <time.h>
#include <unistd.h>

// 当前CPU时钟时间:单位纳秒(ns)
inline unsigned long getClockTime() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long) ts.tv_sec * 1000000000L + ts.tv_nsec;
}

//
// 代码用时统计管理器
//
class TimeUsageStats {
public:
    struct TimeStat {
        TimeStat() {
            count = 0;
            totaltime = 0;
        }

        // 执行次数
        unsigned long count;
        // 执行总用时
        unsigned long totaltime;
    };

    typedef std::unordered_map<std::string, TimeStat> TimeStatMap;

    typedef std::function<void(unsigned long, TimeStatMap &)> DumpCallBack;

    static TimeUsageStats *instance() {
        static TimeUsageStats stats_;
        return &stats_;
    }

    static void dumpByCout(unsigned long elapsed_ns, TimeStatMap &stats) {
        std::cout << "循环用时:" << elapsed_ns << std::endl;
        for (TimeStatMap::iterator it = stats.begin(); it != stats.end(); ++it) {
            std::cout << it->first << ":"
                      << it->second.count << ","
                      << it->second.totaltime << ","
                      << it->second.totaltime / it->second.count << std::endl;
        }
    }

public:
    TimeUsageStats() {
        using std::placeholders::_1;
        using std::placeholders::_2;
        dump_threshhold_ns_ = (unsigned long) -1;
        dump_cb_ = std::bind(&TimeUsageStats::dumpByCout, _1, _2);
        open_ = false;
    }

    bool isOpen() { return open_; }

    void open() { open_ = true; }

    void close() { open_ = false; }

    // Dump时间阈值和回调
    void setDumpThreshholdMs(unsigned long ms) { dump_threshhold_ns_ = ms * 100000L; }

    void setDumpCallBack(DumpCallBack cb) { dump_cb_ = cb; }

    // 循环回调
    void onLoopBegin() {
        loop_begin_ = getClockTime();
    }

    void onLoopEnd() {
        loop_end_ = getClockTime();
        if (loop_end_ > loop_begin_ && loop_end_ - loop_begin_ >= dump_threshhold_ns_) {
            this->dump_cb_(loop_end_ - loop_begin_, stats_);
        }
        stats_.clear();
    }

    // 统计用时
    void stat(const std::string &desc, unsigned long timeusage) {
        TimeStat &ts = stats_[desc];
        if (ts.count) {
            ts.count++;
            ts.totaltime += timeusage;
        } else {
            ts.count++;
            ts.totaltime = timeusage;
        }
    }

protected:
    /// 用时统计
    TimeStatMap stats_;
    /// 开启与否?
    bool open_;
    /// 超时Dump时间阈值(ns)
    unsigned long dump_threshhold_ns_;
    /// 超时Dump回调
    DumpCallBack dump_cb_;
    /// 循环开始时钟(ns)
    unsigned long loop_begin_;
    /// 循环结束时钟(ns)
    unsigned long loop_end_;
};

//
// 代码块用时
//
class TimeUsage {
public:
    TimeUsage(const std::string &desc, TimeUsageStats *stats = TimeUsageStats::instance())
            : desc_(desc), stats_(stats) {
        if (stats_ && stats_->isOpen())
            begin_ = getClockTime();
    }

    ~TimeUsage() {
        if (stats_ && stats_->isOpen()) {
            end_ = getClockTime();
            stats_->stat(desc_, (end_ > begin_ ? end_ - begin_ : 0));
        }
    }

private:
    std::string desc_;
    TimeUsageStats *stats_;
    unsigned long begin_;
    unsigned long end_;
};

//
// 代码块用时统计
//
#define CODE_TIME_USAGE(desc)  TimeUsage __time_usage__(desc);


// ------------------------- 下面为测试 ------------------------------

void dump_test(unsigned long elapsed_ns, TimeUsageStats::TimeStatMap &stats) {
    std::cout << "循环用时测试:" << elapsed_ns << std::endl;
    for (TimeUsageStats::TimeStatMap::iterator it = stats.begin(); it != stats.end(); ++it) {
        std::cout << it->first << ":"
                  << it->second.count << ","
                  << it->second.totaltime << ","
                  << it->second.totaltime / it->second.count << std::endl;
    }
}

void test_time() {
    TimeUsageStats::instance()->open();
    TimeUsageStats::instance()->setDumpThreshholdMs(1);
    TimeUsageStats::instance()->setDumpCallBack(dump_test);

    TimeUsageStats::instance()->onLoopBegin();

    {
        CODE_TIME_USAGE("X函数")
        sleep(1);
    }

    {
        CODE_TIME_USAGE("Y函数");
        usleep(1);
        for (int i = 0; i < 100; i++) {
            CODE_TIME_USAGE("Y.1函数");
            usleep(1);
        }
    }

    TimeUsageStats::instance()->onLoopEnd();
}

#define BLOCK_TIME_CONSUMING_ENABLE

#include <chrono>
#include <mutex>
#include <memory>

struct BlockInfo {
    std::string name;
    std::string filename;
    std::string function;
    uint32_t line = 0;
};

struct BlockTimeStat : public BlockInfo {
    uint64_t calls = 0;
    uint64_t total_ms = 0;
};

typedef std::shared_ptr<BlockTimeStat> BlockTimeStatPtr;

class BlockTimeUsageStat {
public:
    static BlockTimeUsageStat *instance() {
        static BlockTimeUsageStat stats_;
        return &stats_;
    }

    static void dump2cout(BlockTimeStatPtr stat) {
        std::cout << stat->name << " - " << stat->function << ", " << stat->calls << ", "
                  << stat->total_ms << std::endl;
    }

public:
    void called(const BlockInfo &block, uint64_t ms) {
        std::lock_guard<std::mutex> lock_(mutex_);

        auto it = blocks_.find(block.name);
        if (it != blocks_.end()) {
            it->second->calls++;
            it->second->total_ms += ms;

        } else {
            BlockTimeStatPtr stat = std::make_shared<BlockTimeStat>();
            if (stat) {
                stat->name = block.name;
                stat->filename = block.filename;
                stat->function = block.function;
                stat->line = block.line;
                stat->calls++;
                stat->total_ms += ms;

                blocks_.insert(std::make_pair(stat->name, stat));
            }
        }

        total_ms_ += ms;
    }

    void forEach(const std::function<void(BlockTimeStatPtr)> &cb) {
        std::lock_guard<std::mutex> lock_(mutex_);

        for (auto &it : blocks_)
            cb(it.second);
    }

    void forEachByOrder(const std::function<void(BlockTimeStatPtr)> &cb) {
        std::lock_guard<std::mutex> lock_(mutex_);

        std::multimap<uint64_t, BlockTimeStatPtr> order;
        for (auto &it : blocks_)
            order.insert(std::make_pair(it.second->total_ms, it.second));

        for (auto &it : order)
            cb(it.second);
    }

    void
    dump(const std::function<void(BlockTimeStatPtr)> &cb = BlockTimeUsageStat::dump2cout, uint64_t threshhold = 0) {
        forEachByOrder([threshhold, cb](BlockTimeStatPtr stat) {
            if (stat->total_ms >= threshhold)
                cb(stat);
        });
    }

    void reset() {
        std::lock_guard<std::mutex> lock_(mutex_);
        total_ms_ = 0;
        blocks_.clear();
    }

    bool isOpen() const { return open_; }

    void setOpen(bool value) { open_ = value; }

private:
    volatile bool open_ = true;
    std::mutex mutex_;
    uint64_t total_ms_ = 0;
    std::unordered_map<std::string, BlockTimeStatPtr> blocks_;
};

class BlockTimeUsage {
public:
    BlockTimeUsage(const std::string &name,
                   const std::string &file, uint32_t line,
                   const std::string function,
                   BlockTimeUsageStat *stats = BlockTimeUsageStat::instance())
            : stats_(stats) {
        if (stats && stats->isOpen()) {
            block_.name = name;
            block_.filename = file;
            block_.line = line;
            block_.function = function;
            begin_ = std::chrono::high_resolution_clock::now();
        }
    }

    ~BlockTimeUsage() {
        if (stats_ && stats_->isOpen()) {
            end_ = std::chrono::high_resolution_clock::now();
            long ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - begin_).count();
            stats_->called(block_, ms);
        }
    }

private:
    BlockInfo block_;
    BlockTimeUsageStat *stats_;

    std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_;
};

#ifndef __FILENAME__
#  define __FILENAME__ __FILE__
#endif

#define FILE_LINE (std::string(__FILENAME__) + ":" + std::to_string(__LINE__))

#ifdef BLOCK_TIME_CONSUMING_ENABLE
#  define BLOCK_TIME_CONSUMING(name) BlockTimeUsage __time_usage__(name, __FILENAME__, __LINE__, __FUNCTION__);
#  define MYBLOCK_TIME_CONSUMING(stat, name) BlockTimeUsage __time_usage__(name, __FILENAME__, __LINE__, __FUNCTION__, stat);
#else
# define BLOCK_TIME_CONSUMING(name)
# define MYBLOCK_TIME_CONSUMING(stat, name)
#endif

void func_1(int a) {
    MYBLOCK_TIME_CONSUMING(BlockTimeUsageStat::instance(), FILE_LINE);

    {
        BLOCK_TIME_CONSUMING("uleep:" + std::to_string(a));
        usleep(1);
    }
}

void func_1(int a, int b) {
    BLOCK_TIME_CONSUMING(FILE_LINE);

    usleep(1);
}


void test_function() {

    BlockTimeUsageStat::instance()->setOpen(false);

    for (int i = 0; i < 555; ++i) {
        func_1(i);
        func_1(0, 0);
    }

    BlockTimeUsageStat::instance()->dump();
}


#include <list>
#include <algorithm>

void test_lambda_1() {
    std::list<int> ids;
    for (int i = 0; i < 10; i++)
        ids.push_back(i);

    std::for_each(ids.begin(), ids.end(), [](int i) {
        std::cout << i << std::endl;
    });
}

int main() {
//    test_time();
//    test_lambda_1();
    test_function();
    return 0;
}