#include "tcp_server.h"
#include "tcp_message.h"
#include "tcp_message_assembler.h"
#include "tcp_message_handler.h"
#include <utility>
#include <cstring>
#include <errno.h>

void listener_accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int len, void *arg);
void server_read_cb(struct bufferevent *bev, void *arg);
void server_event_cb(struct bufferevent *bev, short events, void *arg);

TcpServer::TcpServer(int port)
    : m_isListening(false),
      m_port(port),
      m_thread(std::bind(&TcpServer::tcp_server_thread, this)),
      m_base(NULL),
      m_listener(NULL),
      m_map(),
      m_handler(NULL)
{

}

TcpServer::~TcpServer()
{
    stop();
}

void TcpServer::setHandler(TcpMessageHandler *handler)
{
    m_handler = handler;
}

bool TcpServer::start()
{
    if (!isListening())
    {
        m_thread.start();
        m_isListening = true;
    }

    return m_isListening;
}

void TcpServer::stop()
{
    if (isListening())
    {
        if (event_base_loopbreak(m_base) == 0)
        {
            typedef std::map<evutil_socket_t, std::shared_ptr<TcpMessageAssembler>>::iterator Iterator;

            for (Iterator pos = m_map.begin(); pos != m_map.end(); )
            {
                m_map.erase(pos++);
            }

            m_isListening = false;
        }
    }
}

bool TcpServer::isListening() const
{
    return m_isListening;
}

void TcpServer::tcp_server_thread()
{
    struct sockaddr_in sin = {0};

    sin.sin_family = AF_INET;
    sin.sin_port = htons(m_port);

    m_base = event_base_new();
    m_listener = evconnlistener_new_bind(m_base,
                                         listener_accept_cb,
                                         this,
                                         LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                         -1,
                                         reinterpret_cast<struct sockaddr *>(&sin),
                                         sizeof(sin));
    event_base_dispatch(m_base);

    evconnlistener_free(m_listener);
    event_base_free(m_base);
}

void listener_accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int len, void *arg)
{
    TcpServer *server = static_cast<TcpServer *>(arg);

    if (server != NULL)
    {
        struct event_base *base = evconnlistener_get_base(listener);
        struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        evutil_socket_t connfd = bufferevent_getfd(bev);
        std::shared_ptr<TcpMessageAssembler> assembler(new TcpMessageAssembler);
        server->m_map.insert(std::make_pair(connfd, assembler));

        bufferevent_setcb(bev, server_read_cb, NULL, server_event_cb, server);
        bufferevent_enable(bev, EV_READ | EV_WRITE);
    }
}

void server_read_cb(struct bufferevent *bev, void *arg)
{
    TcpServer *server = static_cast<TcpServer *>(arg);
    char buf[1024] = {0};
    size_t len = 0;

    if (server != NULL)
    {
        evutil_socket_t connfd = bufferevent_getfd(bev);
        std::shared_ptr<TcpMessageAssembler> assembler = server->m_map[connfd];

        while ((len = bufferevent_read(bev, buf, sizeof(buf))) > 0)
        {
            std::shared_ptr<TcpMessage> ptm = (assembler != NULL) ? assembler->assemble(buf, len) : NULL;

            if ((ptm != NULL) && (server->m_handler != NULL))
            {
                printf("assembler success from connfd %d\n", connfd);
                server->m_handler->handle(connfd, *ptm);
            }
        }
    }
}

void server_event_cb(struct bufferevent *bev, short events, void *arg)
{
    TcpServer *server = static_cast<TcpServer *>(arg);
    evutil_socket_t connfd = bufferevent_getfd(bev);

    if (events & BEV_EVENT_EOF)
    {
        server->m_map.erase(connfd);
        printf("Connection %d closed.\n", connfd);
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Got an error on the connection %d: %s\n", connfd, strerror(errno));
    }

    bufferevent_free(bev);
}
