#include "../include/TcpServer.hpp"


TcpServer::TcpServer(const std::string& ip, const uint16_t port)
{
    _M_acceptor_ptr = new Acceptor(&_M_loop, ip, port);
    _M_acceptor_ptr->set_create_connection_callback(
        std::bind(&TcpServer::create_connection, this, std::placeholders::_1));
}

void TcpServer::start()
{
    _M_loop.run();
}

void TcpServer::deal_message(Connection* conn, std::string& message)
{
    // 假设将数据经过计算后             
    message = "reply: " + message;

    int len = message.size(); // 获取返回报文的长度
    std::string tmpbuf((char*)&len, 4); // 填充报文头部
    tmpbuf.append(message);             // 填充报文内容 

    // 将报文发送出去
    conn->send(tmpbuf.c_str(), tmpbuf.size());
}

void TcpServer::create_connection(Socket* clnt_sock)
{
    // 创建Connection对象
    Connection* conn = new Connection(&_M_loop, clnt_sock);
    conn->set_close_callback(std::bind(&TcpServer::close_connection, this, std::placeholders::_1));
    conn->set_error_callback(std::bind(&TcpServer::error_connection, this, std::placeholders::_1));
    conn->set_send_complete_callback(std::bind(&TcpServer::send_complete, this, std::placeholders::_1));
    conn->set_deal_message_callback(std::bind(&TcpServer::deal_message, this, 
                                                    std::placeholders::_1, std::placeholders::_2));

    printf("new connection(fd = %d, ip = %s, port = %d) ok.\n", 
            conn->get_fd(), conn->get_ip().c_str(), conn->get_port());

    // 将连接用map来管理
    _M_connections_map[conn->get_fd()] = conn;
}

void TcpServer::close_connection(Connection* conn)
{
    printf("client(clnt_fd = %d) disconnected\n", conn->get_fd());
    _M_connections_map.erase(conn->get_fd());
    delete conn;
}

void TcpServer::error_connection(Connection* conn)
{
    printf("client(clnt_fd = %d) error.\n", conn->get_fd());
    _M_connections_map.erase(conn->get_fd());
    delete conn;
}

void TcpServer::send_complete(Connection* conn)
{
    printf("send complete!\n");

    // 可增加其它代码
}

TcpServer::~TcpServer()
{
    delete _M_acceptor_ptr;

    // 释放全部的连接
    for(auto& [fd, conn] : _M_connections_map) {
        delete conn;
    }
}