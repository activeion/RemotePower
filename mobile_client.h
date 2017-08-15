#include "dispatcher.h"
#include "codec.h"
#include "remote_power.pb.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>

#include <boost/bind.hpp>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

typedef boost::shared_ptr<eh2tech::Empty> EmptyPtr;
typedef boost::shared_ptr<eh2tech::Answer> AnswerPtr;
typedef boost::shared_ptr<eh2tech::QueryAnswer> QueryAnswerPtr;

class MobileClient : boost::noncopyable
{
    public:
        MobileClient(EventLoop* loop,
                const InetAddress& serverAddr);

        void connect();

    private:

        void onConnection(const TcpConnectionPtr& conn);

        void onSetting(const TcpConnectionPtr&,
                const MessagePtr& message,
                Timestamp);
        void onUnknownMessage(const TcpConnectionPtr&,
                const MessagePtr& message,
                Timestamp);

        void onAnswer(const muduo::net::TcpConnectionPtr&,
                const AnswerPtr& message,
                muduo::Timestamp);
        void onQueryAnswer(const muduo::net::TcpConnectionPtr&,
                const QueryAnswerPtr& message,
                muduo::Timestamp);
        void onEmpty(const muduo::net::TcpConnectionPtr&,
                const EmptyPtr& message,
                muduo::Timestamp);

        void submitMessage();

        TcpConnectionPtr conn_;
        eh2tech::Query data_;
        eh2tech::Setting setting_;

        EventLoop* loop_;
        TcpClient client_;
        ProtobufDispatcher dispatcher_;
        ProtobufCodec codec_;
};

