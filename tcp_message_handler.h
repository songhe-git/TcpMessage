#ifndef TCP_MESSAGE_HANDLER_H
#define TCP_MESSAGE_HANDLER_H

class TcpMessage;

class TcpMessageHandler
{
public:
    virtual void handle(int fd, const TcpMessage &message) = 0;
};

#endif
