#include "TimeStarmp.h"
TimeStarmp::TimeStarmp(/* args */)
{
}

TimeStarmp::TimeStarmp(int64_t secsinceepoch)
:secsinceepoch_(secsinceepoch){

}

TimeStarmp::~TimeStarmp()
{
}

TimeStarmp TimeStarmp::now(){
    return TimeStarmp();//返回当前时间
}

time_t TimeStarmp::toInt()const{
    return secsinceepoch_;
}

std::string TimeStarmp::toString()const{
    char buf[32] = {0};
    tm *tm_time = localtime(&secsinceepoch_);
    snprintf(buf, 32, "%4d-%02d-%02d %02d:%02d:%02d", 
        tm_time->tm_year + 1900,
        tm_time->tm_mon + 1,
        tm_time->tm_mday,
        tm_time->tm_hour,
        tm_time->tm_min,
        tm_time->tm_sec
    );
    return buf;
}