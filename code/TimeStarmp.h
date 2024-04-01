#pragma once
#include <iostream>
#include <string>

class TimeStarmp//时间戳类
{
private:
    time_t secsinceepoch_;//整数表示的时间（从1970到现在已经逝去的秒数）
public:
    TimeStarmp(/* args */);
    TimeStarmp(int64_t secsinceepoch);
    ~TimeStarmp();

    static TimeStarmp now();
    time_t toInt()const;
    std::string toString()const;
};

