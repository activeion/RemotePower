#include "mobile_client.h"

MobileClient::MobileClient(EventLoop* loop,
        const InetAddress& serverAddr)
    : loop_(loop),
    client_(loop, serverAddr, "MobileClient"),
    dispatcher_(std::bind(&MobileClient::onUnknownMessage, this, _1, _2, _3)),
    codec_(std::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher_, _1, _2, _3))
{
    dispatcher_.registerMessageCallback<eh2tech::Setting>(
            std::bind(&MobileClient::onSetting, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<eh2tech::Answer>(
            std::bind(&MobileClient::onAnswer, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<eh2tech::QueryAnswer>(
            std::bind(&MobileClient::onQueryAnswer, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<eh2tech::Empty>(
            std::bind(&MobileClient::onEmpty, this, _1, _2, _3));
    client_.setConnectionCallback(
            std::bind(&MobileClient::onConnection, this, _1));
    client_.setMessageCallback(
            std::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));

    setting_.set_id(1);
    eh2tech::Setting::CatFreq* catfreq;
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::TEMP); catfreq->set_freq(15);
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::POWER); catfreq->set_freq(3);
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::PRESS); catfreq->set_freq(9);
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::FLOW); catfreq->set_freq(5);
}

void MobileClient::connect()
{
    client_.connect();
}

void MobileClient::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
        << conn->peerAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");
    conn_ = conn;
    loop_->runEvery(1, std::bind(&MobileClient::submitMessage,this));

}

void MobileClient::onSetting(const TcpConnectionPtr&,
        const MessagePtr& message,
        Timestamp)
{
    LOG_INFO << "onSetting: " << message->GetTypeName();
}

void MobileClient::onUnknownMessage(const TcpConnectionPtr&,
        const MessagePtr& message,
        Timestamp)
{
    LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
}

void MobileClient::onAnswer(const muduo::net::TcpConnectionPtr&,
        const AnswerPtr& message,
        muduo::Timestamp)
{
    LOG_INFO << "onAnswer:\n" << message->GetTypeName() << message->DebugString();
}
void MobileClient::onQueryAnswer(const muduo::net::TcpConnectionPtr&,
        const QueryAnswerPtr& message,
        muduo::Timestamp)
{
    LOG_INFO << "onQueryAnswer:\n" << message->GetTypeName() << "\n" << message->DebugString();
}

void MobileClient::onEmpty(const muduo::net::TcpConnectionPtr&,
        const EmptyPtr& message,
        muduo::Timestamp)
{
    LOG_INFO << "onEmpty: " << message->GetTypeName();
}

void MobileClient::submitMessage()
{
    // prepare for fake data
    data_.Clear();
    data_.set_id(1);
    data_.set_sn("M100-20170630");
    data_.set_cat(eh2tech::Catalog::POWER);

    if(!conn_->connected()) loop_->quit();
    static int count=0;
    count++;
    for(int i=0; i<setting_.catfreq_size(); ++i) {
        data_.set_cat(eh2tech::Catalog(i));
        eh2tech::Setting_CatFreq catfreq= setting_.catfreq(i);
        int freq = catfreq.freq();
        if(count%freq==0){
            codec_.send(conn_, data_);
            LOG_INFO << "Query sent!"<< "cat="<<catfreq.cat();
        }
    }


}

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 2)
    {
        EventLoop loop;
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress serverAddr(argv[1], port);

        MobileClient client(&loop, serverAddr);
        client.connect();
        loop.loop();
    }
    else
    {
        printf("Usage: %s host_ip port [q|e]\n", argv[0]);
    }
}

