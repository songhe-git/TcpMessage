#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "thread.h"
#include <map>
#include <memory>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

class TcpMessageAssembler;
class TcpMessageHandler;

class TcpServer
{
private:
    bool m_isListening;
    int m_port;
    Thread m_thread;
    struct event_base *m_base;
    struct evconnlistener *m_listener;
    std::map<evutil_socket_t, std::shared_ptr<TcpMessageAssembler>> m_map;
    TcpMessageHandler *m_handler;

    void tcp_server_thread();
    friend void listener_accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int len, void *arg);
    friend void server_read_cb(struct bufferevent *bev, void *arg);
    friend void server_event_cb(struct bufferevent *bev, short events, void *arg);

    TcpServer(const TcpServer &) = delete;
    TcpServer &operator = (const TcpServer &) = delete;
public:
    explicit TcpServer(int port);
    ~TcpServer();
    void setHandler(TcpMessageHandler *handler);
    bool start();
    void stop();
    bool isListening() const;
};

#endif
