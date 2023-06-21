#include <iostream>
#include <sys/epoll.h>

using namespace std;

int main(){
    struct epoll_event* events = new epoll_event[1024];
    cout << "sizeof(epoll_event): " << sizeof(epoll_event) << endl;
    cout << "sizeof(events): " << sizeof(events) << endl;
    cout << "sizeof(*events): " << sizeof(*events) << endl;
    cout << "sizeof(*events) * 1024: " << sizeof(*events) * 1024 << endl;
    return 0;
}
