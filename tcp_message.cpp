#include "tcp_message.h"

TcpMessage::TcpMessage()
{
    m_type = 0;
    m_length = 0;
    m_data.clear();
}

TcpMessage::TcpMessage(int type, int length, const std::vector<char> &data)
{
    m_type = type;
    m_length = length;
    m_data.clear();

    if (m_length > MaxDataLength)
    {
        m_length = MaxDataLength;
        m_data.reserve(MaxDataLength);
    }

    for (int i = 0; i < m_length; i++)
    {
        m_data.push_back(data[i]);
    }
}

int TcpMessage::type() const
{
    return m_type;
}

int TcpMessage::length() const
{
    return m_length;
}

std::vector<char> TcpMessage::data() const
{
    return m_data;
}

std::vector<char> TcpMessage::serialize()
{
    std::vector<char> ret;

    ret.push_back(m_type >> 24);
    ret.push_back(m_type >> 16);
    ret.push_back(m_type >> 8);
    ret.push_back(m_type);

    ret.push_back(m_length >> 24);
    ret.push_back(m_length >> 16);
    ret.push_back(m_length >> 8);
    ret.push_back(m_length);

    for (int i = 0; i < m_length; i++)
    {
        ret.push_back(m_data[i]);
    }

    return ret;
}

bool TcpMessage::unserialize(std::vector<char> &msg)
{
    bool ret = (msg.size() >= 8);

    if (ret)
    {
        int type = (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | msg[3];
        int length = (msg[4] << 24) | (msg[5] << 16) | (msg[6] << 8) | msg[7];

        if ((type > 0) && (length >= 0))
        {
            m_type = type;
            m_length = length;

            m_data.clear();

            for (int i = 0; i < m_length; i++)
            {
                m_data.push_back(msg[i + 8]);
            }
        }
    }

    return true;
}
