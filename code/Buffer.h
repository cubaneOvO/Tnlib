#pragma once
#include <string>
#include <iostream>
#include <cstring>

class Buffer
{
private:
    std::string buf_;
    const uint16_t sep_;//报文的分隔符: 0-无分隔符（应用在固定长度/视频会议）1-四字节报头 2-"\r\n\r\n"(http协议)
public:
    Buffer(int sep = 1);
    ~Buffer() = default;
    void append(const char *, size_t);//追加数据
    void appendWithsep(const char *, size_t);//追加数据并根据报文格式进行处理
    size_t size();//返回buf_大小
    const char* data();//返回buf_首地址
    void clear();//清空buf_
    void erase(size_t pos, size_t n);//删除pos开始的n个字符
    bool pickMessage(std::string &);//将报文从buffer中取出
};

