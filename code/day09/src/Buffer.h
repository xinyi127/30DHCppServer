// 问题：在之前的源文件中，客户端向服务器端发送数据时，不知道客户端信息的真正大小是多少，只能以1024的读缓冲区去读TCP缓冲区
// （就算TCP缓冲区的数据没有1024，也会把后面的用空值补满）；也不能一次性读取所有客户端数据，再统一发给客户端。
// 这样的设计会导致一些问题，例如无法立即处理大量数据的情况。同时，由于没有动态调整读缓冲区大小的机制，
// 如果客户端发送的数据超过了缓冲区大小，可能会导致数据丢失或溢出的问题。
//
// 方案：封装一个缓冲区。为每一个Connection类分配一个读缓冲区和写缓冲区，从客户端读取来的数据都存放在读缓冲区里，
// 这样Connection类就不再直接使用char buf[]这种最笨的缓冲区来处理读写操作。

#pragma once
#include <string>

class Buffer
{
private:
    std::string buf;
public:
    Buffer();
    ~Buffer();
    void append(const char* _str, int _size);
    ssize_t size();
    const char* c_str();
    void clear();
    void getline();
};
