#include "tcp_message_assembler.h"
#include "tcp_message.h"

void TcpMessageAssembler::clear()
{
    m_type = 0;
    m_length = 0;
    m_data.clear();
}

std::vector<char> TcpMessageAssembler::fetch(int n)
{
    std::vector<char> ret;

    for (int i = 0; i < n; i++)
    {
        if (m_queue.empty())
        {
            break;
        }

        ret.push_back(m_queue.front());
        m_queue.pop();
    }

    return ret;
}

bool TcpMessageAssembler::makeTypeAndLength()
{
    bool ret = (m_queue.size() >= 8);

    if (ret)
    {
        std::vector<char> vectype = fetch(4);
        m_type = (vectype[0] << 24) | (vectype[1] << 16) | (vectype[2] << 8) | vectype[3];

        std::vector<char> veclength = fetch(4);
        m_length = (veclength[0] << 24) | (veclength[1] << 16) | (veclength[2] << 8) | veclength[3];

        if ((m_type <= 0) || (m_length < 0))
        {
            clear();
            ret = false;
        }
    }

    return ret;
}

TcpMessage *TcpMessageAssembler::makeMessage()
{
    TcpMessage *ret = NULL;

    if (m_type > 0)
    {
        int needed = m_length - m_data.size();
        int n = (needed <= m_queue.size()) ? needed: m_queue.size();

        std::vector<char> vecdata = fetch(n);
        m_data.insert(m_data.end(), vecdata.begin(), vecdata.end());

        if (m_length == m_data.size())
        {
            ret = new TcpMessage(m_type, m_length, m_data);
        }
    }

    return ret;
}

TcpMessageAssembler::TcpMessageAssembler()
{
    clear();
}

void TcpMessageAssembler::prepare(const char *data, int len)
{
    if (data != NULL)
    {
        for (int i = 0; i < len; i++)
        {
            m_queue.push(data[i]);
        }
    }
}

std::shared_ptr<TcpMessage> TcpMessageAssembler::assemble()
{
    TcpMessage *ret = NULL;
    bool tryMakeMsg = (m_type > 0);

    if (!tryMakeMsg)
    {
        tryMakeMsg = makeTypeAndLength();
    }

    if (tryMakeMsg)
    {
        ret = makeMessage();
    }

    if (ret != NULL)
    {
        clear();
    }

    return std::shared_ptr<TcpMessage>(ret);
}

std::shared_ptr<TcpMessage> TcpMessageAssembler::assemble(const char *data, int len)
{
    prepare(data, len);

    return assemble();
}

void TcpMessageAssembler::reset()
{
    clear();

    std::queue<char> empty;
    std::swap(empty, m_queue);
}
