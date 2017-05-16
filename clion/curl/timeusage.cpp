#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <curl/curl.h>

#define CHKSPEED_VERSION "1.0"

std::ofstream result;

static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data) {

//    std::chrono::milliseconds ms(1000);
//    std::this_thread::sleep_for(ms);
    /* we are not interested in the downloaded bytes itself,
       so we only return the size we would have saved ... */
    (void) ptr;  /* unused */
    (void) data; /* unused */

    //result.append((char *) ptr, size * nmemb);
    result.write((char *) ptr, size * nmemb);

//    std::cout << size * nmemb << std::endl;

    return (size_t) (size * nmemb);
}

//
// curl_easy_perform()
//    |
//    |--NAMELOOKUP
//    |--|--CONNECT
//    |--|--|--APPCONNECT
//    |--|--|--|--PRETRANSFER
//    |--|--|--|--|--STARTTRANSFER
//    |--|--|--|--|--|--TOTAL
//    |--|--|--|--|--|--REDIRECT
//

struct ResourceTime {
    double namelookup = 0;
    double connect = 0;
    double app_connect = 0;
    double pre_transfer = 0;
    double start_transfer = 0;
    double total = 0;

    double download_size = 0;
    double download_speed = 0;

    double getDNSTime() { return namelookup; }

    double getConnectionTime() { return (connect > namelookup ? connect - namelookup : 0); }

    double getRequestSendTime() { return (pre_transfer > app_connect ? pre_transfer - app_connect : 0); }

    double getTTFBTime() { return (start_transfer > pre_transfer ? start_transfer - pre_transfer : 0); }

    double getDownloadTime() { return (total > start_transfer ? total - start_transfer : 0); }

    double getTotalTime() { return total; }


    double getDownloadSize() { return download_size; }

    double getDownloadSpeed() { return download_speed; }


    ResourceTime(CURL *curl_handle) {
        double val = 0;
        CURLcode res;
        res = curl_easy_getinfo(curl_handle, CURLINFO_NAMELOOKUP_TIME, &val);
        if ((CURLE_OK == res) && (val > 0))
            namelookup = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_CONNECT_TIME, &val);
        if ((CURLE_OK == res) && (val > 0))
            connect = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_APPCONNECT_TIME, &val);
        if ((CURLE_OK == res) && (val > 0))
            app_connect = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_PRETRANSFER_TIME, &val);
        if ((CURLE_OK == res) && (val > 0))
            pre_transfer = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_STARTTRANSFER_TIME, &val);
        if ((CURLE_OK == res) && (val > 0))
            start_transfer = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME, &val);
        if ((CURLE_OK == res) && (val > 0))
            total = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_SIZE_DOWNLOAD, &val);
        if ((CURLE_OK == res) && (val > 0))
            download_size = val;

        res = curl_easy_getinfo(curl_handle, CURLINFO_SPEED_DOWNLOAD, &val);
        if ((CURLE_OK == res) && (val > 0))
            download_speed = val;
    }

    void dump() {
        std::cout << " --------------------------------- \n";
        std::cout << std::setprecision(3) << "DNS\t:" << namelookup << std::endl;
        std::cout << std::setprecision(3) << "TCP\t:" << connect << std::endl;
        std::cout << std::setprecision(3) << "TLS\t:" << app_connect << std::endl;
        std::cout << std::setprecision(3) << "REQUEST_START\t:" << pre_transfer << std::endl;
        std::cout << std::setprecision(3) << "RESPONSE_START\t:" << start_transfer << std::endl;
        std::cout << std::setprecision(3) << "TOTAL\t:" << total << std::endl;
        std::cout << std::setprecision(3) << std::fixed << "\nSIZE\t:" << download_size << std::endl;
        std::cout << std::setprecision(3) << std::fixed << "SPEED(BYTES/s)\t:" << download_speed << std::endl;
        std::cout << std::setprecision(3) << std::fixed << "SIZE/TOTAL\t:" << download_size/total << std::endl;
        std::cout << std::endl;
    }

    void dump2()
    {
        std::cout << "DNS Lookup:         " <<  std::setprecision(3) << std::fixed << getDNSTime() << std::endl;
        std::cout << "Initial Connection: " <<  std::setprecision(3) << std::fixed << getConnectionTime() << std::endl;
        std::cout << "Request sent:       " <<  std::setprecision(3) << std::fixed << getRequestSendTime() << std::endl;
        std::cout << "Waiting(TTFB):      " <<  std::setprecision(3) << std::fixed << getTTFBTime() << std::endl;
        std::cout << "Content Download:   " <<  std::setprecision(3) << std::fixed << getDownloadTime() << std::endl;
    }
};

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        std::cout << "Usage:" << argv[0] << " url" << std::endl;
        return EXIT_FAILURE;
    }

    int count = 1;
    if (argc >= 3)
        count = std::atoi(argv[2]);

    const char *url = argv[1];

    std::string urlstr = url;

    result.open(urlstr.substr(urlstr.rfind('/')+1).c_str());

    /* init libcurl */
    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    CURL *curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT,
                     "timeusage/"CHKSPEED_VERSION);

    for (int i = 0; i < count; ++ i) {
        /* get it! */
        CURLcode res = curl_easy_perform(curl_handle);

        if (CURLE_OK == res) {

            //std::cout << result << std::endl;

            ResourceTime rt(curl_handle);
            rt.dump();

//        std::cout << "\nRESULT SIZE:" << result.size() << std::endl;

            rt.dump2();
        } else {
            fprintf(stderr, "Error while fetching '%s' : %s\n",
                    url, curl_easy_strerror(res));
        }
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
    return 0;
}