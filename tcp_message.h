#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <vector>

class TcpMessage
{
private:
    int m_type;
    int m_length;
    std::vector<char> m_data;
public:
    enum TcpMessageType
    {
        KEEP_ALIVE = 1,
		VIRTUAL_KEY
    };

    enum {MaxDataLength = 1024};

    TcpMessage();
    TcpMessage(int type, int length, const std::vector<char> &data);
    int type() const;
    int length() const;
    std::vector<char> data() const;
    std::vector<char> serialize();
    bool unserialize(std::vector<char> &msg);
};

#endif
