#include "Buffer.h"

Buffer::Buffer(int sep):sep_(sep){

}

void Buffer::append(const char *data, size_t size){
//std::cout <<  data << std::endl;
    buf_.append(data, size);
           //std::cout <<  buf_ << std::endl;
}

void Buffer::appendWithsep(const char *data, size_t size){
    if(sep_ == 1){
        buf_.append((const char*)&size, 4);
        buf_.append(data, size);
 
    }
    else if(sep_ == 0){
        buf_.append(data, size);
    }
    //可扩展http协议格式报文
}

size_t Buffer::size()
{
    return buf_.size();
}

const char *Buffer::data()
{
    return buf_.data();
}

void Buffer::clear(){
    buf_.clear();
}

void Buffer::erase(size_t pos, size_t n){
    buf_.erase(pos, n);
}

bool Buffer::pickMessage(std::string & str){
    if(buf_.size() == 0)return false;
    if(sep_ == 0){
        str = buf_;
        buf_.clear();
    }
    else if(sep_ == 1){
        int len;
        memcpy(&len, buf_.data(), 4);//取报文长度
        if(buf_.size() < len+4)//数据接收不完整
            return false;
        str = buf_.substr(4,len);//将报文取出
        buf_.erase(0, len+4);
    }
    return true;
}
