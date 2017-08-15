#include "power_client.h"

PowerClient::PowerClient(EventLoop* loop,
        const InetAddress& serverAddr)
    : loop_(loop),
    client_(loop, serverAddr, "PowerClient"),
    dispatcher_(boost::bind(&PowerClient::onUnknownMessage, this, _1, _2, _3)),
    codec_(boost::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher_, _1, _2, _3))
{
    dispatcher_.registerMessageCallback<eh2tech::Setting>(
            boost::bind(&PowerClient::onSetting, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<eh2tech::Answer>(
            boost::bind(&PowerClient::onAnswer, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<eh2tech::Empty>(
            boost::bind(&PowerClient::onEmpty, this, _1, _2, _3));
    client_.setConnectionCallback(
            boost::bind(&PowerClient::onConnection, this, _1));
    client_.setMessageCallback(
            boost::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));

    setting_.set_id(1);
    eh2tech::Setting::CatFreq* catfreq;
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::TEMP); catfreq->set_freq(15);
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::POWER); catfreq->set_freq(3);
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::PRESS); catfreq->set_freq(9);
    catfreq=setting_.add_catfreq(); catfreq->set_cat(eh2tech::Catalog::FLOW); catfreq->set_freq(5);
}

void PowerClient::connect()
{
    client_.connect();
}

void PowerClient::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
        << conn->peerAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");
    conn_ = conn;
    loop_->runEvery(1, boost::bind(&PowerClient::submitMessage,this));

    eh2tech::Login login;
    login.set_sn("M100-20170630");
    login.set_id(1);

    codec_.send(conn_, login);
}

void PowerClient::onSetting(const TcpConnectionPtr&,
        const MessagePtr& message,
        Timestamp)
{
    LOG_INFO << "onSetting: " << message->GetTypeName();
}

void PowerClient::onUnknownMessage(const TcpConnectionPtr&,
        const MessagePtr& message,
        Timestamp)
{
    LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
}

void PowerClient::onAnswer(const muduo::net::TcpConnectionPtr&,
        const AnswerPtr& message,
        muduo::Timestamp)
{
    LOG_INFO << "onAnswer:\n" << message->GetTypeName() << message->DebugString();
}

void PowerClient::onEmpty(const muduo::net::TcpConnectionPtr&,
        const EmptyPtr& message,
        muduo::Timestamp)
{
    LOG_INFO << "onEmpty: " << message->GetTypeName();
}

void PowerClient::submitMessage()
{
    // prepare for fake data
    data_.Clear();
    data_.set_id(1);
    data_.set_sn("M100-20170630");
    data_.set_cat(eh2tech::Catalog::POWER);
    data_.add_data(240);
    data_.add_data(100);
    data_.add_data(530);
    data_.add_data(40);

    if(!conn_->connected()) loop_->quit();
    static int count=0;
    count++;
    for(int i=0; i<setting_.catfreq_size(); ++i) {
        data_.set_cat(eh2tech::Catalog(i));
        eh2tech::Setting_CatFreq catfreq= setting_.catfreq(i);
        int freq = catfreq.freq();
        if(count%freq==0){
            codec_.send(conn_, data_);
            LOG_INFO << "QueryAnswer sent!"<< "cat="<<catfreq.cat();
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

        PowerClient client(&loop, serverAddr);
        client.connect();
        loop.loop();
    }
    else
    {
        printf("Usage: %s host_ip port [q|e]\n", argv[0]);
    }
}

