#include <boost/date_time.hpp>

using namespace std;

inline boost::posix_time::ptime now()
{
  return boost::posix_time::microsec_clock::local_time();
}

class block_duration
{
public:
  explicit block_duration(const std::string & job_name, size_t count = 0)
  : start_(now()), job_name_(job_name), count_(count)
  {
    cout << "------------------------------" << endl;
  }
  
  ~block_duration()
  {
    boost::posix_time::ptime end = now();
    cout << job_name_ << " took " << (end-start_) << " (" << count_ << " items)" << endl;
    if(count_ > 0)
    {
      cout << (end-start_).total_milliseconds()/(double)count_ << "ms per item" << endl;
      cout << (double)count_/(end-start_).total_milliseconds()*1000 << " items per second" << endl;
    }
    cout << "------------------------------" << endl;
  }
  
private:
  boost::posix_time::ptime start_;
  std::string job_name_;
  size_t count_;
};

#define BLOCK_USAGETIME(desc, count) block_duration b_##__LINE__(desc, count);
