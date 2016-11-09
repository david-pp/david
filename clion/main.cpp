#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <time.h>
#include <unistd.h>

// 当前CPU时钟时间:单位纳秒(ns)
inline unsigned long getClockTime()
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long)ts.tv_sec*1000000000L + ts.tv_nsec;
}

//
// 代码用时统计管理器
//
class TimeUsageStats {
public:
    struct TimeStat{
        TimeStat(){
            count = 0;
            totaltime = 0;
        }
        // 执行次数
        unsigned long count;
        // 执行总用时
        unsigned long totaltime;
    };
    typedef std::unordered_map<std::string, TimeStat> TimeStatMap;

    typedef std::function<void(unsigned long, TimeStatMap&)> DumpCallBack;

    static TimeUsageStats* instance() {
        static TimeUsageStats stats_;
        return &stats_;
    }

    static void dumpByCout(unsigned long elapsed_ns, TimeStatMap& stats)
    {
        std::cout << "循环用时:" << elapsed_ns << std::endl;
        for (TimeStatMap::iterator it = stats.begin(); it != stats.end(); ++ it)
        {
            std::cout << it->first << ":"
                      << it->second.count << ","
                      << it->second.totaltime << ","
                      << it->second.totaltime/it->second.count << std::endl;
        }
    }

public:
    TimeUsageStats() {
        using std::placeholders::_1;
        using std::placeholders::_2;
        dump_threshhold_ns_ = (unsigned long)-1;
        dump_cb_ = std::bind(&TimeUsageStats::dumpByCout,  _1, _2);
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
        if (loop_end_ > loop_begin_ && loop_end_ - loop_begin_ >= dump_threshhold_ns_)
        {
            this->dump_cb_(loop_end_-loop_begin_, stats_);
        }
        stats_.clear();
    }

    // 统计用时
    void stat(const std::string& desc, unsigned long timeusage) {
        TimeStat& ts = stats_[desc];
        if (ts.count) {
            ts.count ++;
            ts.totaltime += timeusage;
        } else{
            ts.count ++;
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
    TimeUsage(const std::string& desc, TimeUsageStats* stats = TimeUsageStats::instance())
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
    TimeUsageStats* stats_;
    unsigned long begin_;
    unsigned long end_;
};

//
// 代码块用时统计
//
#define CODE_TIME_USAGE(desc)  TimeUsage __time_usage__(desc);


// ------------------------- 下面为测试 ------------------------------

void dump_test(unsigned long elapsed_ns, TimeUsageStats::TimeStatMap& stats)
{
    std::cout << "test---循环用时:" << elapsed_ns << std::endl;
    for (TimeUsageStats::TimeStatMap::iterator it = stats.begin(); it != stats.end(); ++ it)
    {
        std::cout << it->first << ":"
                  << it->second.count << ","
                  << it->second.totaltime << ","
                  << it->second.totaltime/it->second.count << std::endl;
    }
}

void test_time()
{
    TimeUsageStats::instance()->open();
    TimeUsageStats::instance()->setDumpThreshholdMs(1);
    TimeUsageStats::instance()->setDumpCallBack(dump_test);

    TimeUsageStats::instance()->onLoopBegin();

    {
        CODE_TIME_USAGE("函数1")
        sleep(1);
    }

    {
        TimeUsage time1("X函数");
        usleep(1);
        for (int i = 0; i < 100; i++)
        {
            TimeUsage time1("Y函数");
            usleep(1);
        }
    }

    TimeUsageStats::instance()->onLoopEnd();
}

int main() {
    test_time();
    return 0;
}