#ifndef TEXTMESSAGEASSEMBLER_H
#define TEXTMESSAGEASSEMBLER_H

#include <vector>
#include <queue>
#include <memory>

class TcpMessage;

class TcpMessageAssembler
{
private:
    std::queue<char> m_queue;
    int m_type;
    int m_length;
    std::vector<char> m_data;

    void clear();
    std::vector<char> fetch(int n);
    bool makeTypeAndLength();
    TcpMessage *makeMessage();
public:
    TcpMessageAssembler();
    void prepare(const char *data, int len);
    std::shared_ptr<TcpMessage> assemble();
    std::shared_ptr<TcpMessage> assemble(const char *data, int len);
    void reset();
};

#endif // TEXTMESSAGEASSEMBLER_H