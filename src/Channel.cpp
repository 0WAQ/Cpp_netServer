#include "../include/Channel.hpp"
#include "../include/Connection.hpp"

Channel::Channel(EventLoop* loop_ptr, int fd) : _M_loop_ptr(loop_ptr), _M_fd(fd)
{

}

int Channel::get_fd() {
    return _M_fd;
}

void Channel::set_ET() {
    _M_monitored_events |= EPOLLET;
}

// 除了设置为读事件, 还直接调用updata_channel去添加
void Channel::set_read_events() {
    _M_monitored_events |= EPOLLIN;
    _M_loop_ptr->updata_channel(this);
}

void Channel::set_in_epoll() {
    _M_in_epoll = true;
}

bool Channel::get_in_epoll() {
    return _M_in_epoll;
}

void Channel::set_happened_events(uint32_t events) {
    _M_happened_events = events;
}

uint32_t Channel::get_happened_events() {
    return _M_happened_events;
}

uint32_t Channel::get_monitored_events() {
    return _M_monitored_events;
}

void Channel::handle()
{
    // 连接中断事件
    if(_M_happened_events & EPOLLRDHUP) { 
        printf("client(clnt_fd = %d) disconnected\n", _M_fd);
        close(_M_fd);
    }
    // 读事件
    else if(_M_happened_events & (EPOLLIN | EPOLLPRI)) 
    {
        // 利用回调函数调用处理函数
        _M_read_callback();
    }
    // 写事件
    else if(_M_happened_events & EPOLLOUT) 
    {
        
    }
    // 错误
    else 
    {
        printf("client(clnt_fd = %d) error.\n", _M_fd);
        close(_M_fd);
    }
}

void Channel::new_message()
{
    char buf[1024];

    // 只用边缘触发, 需要确保将发送过来的数据读取完毕
    while(true) // non-blocking IO
    {
        // init all buf as 0
        bzero(&buf, sizeof(buf));

        ssize_t nlen = read(_M_fd, buf, sizeof(buf));

        // read datas successfully
        if(nlen > 0) 
        {
            printf("recv(clnt_fd = %d): %s\n", _M_fd, buf);
            send(_M_fd, buf, strlen(buf), 0);
        }
        // read failed because interrupted by system call
        else if(nlen == -1 && errno == EINTR) 
        {
            continue;
        }
        // read failed because datas've been read
        else if(nlen == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) 
        {
            break;
        }
        // clnt has been disconnected
        else if(nlen == 0) 
        {
            printf("client(clnt_fd = %d) disconnected.\n", _M_fd);
            close(_M_fd);
            break;
        }
    }
}

void Channel::set_read_callback(std::function<void()> func)
{
    _M_read_callback = func;
}

Channel::~Channel()
{

}