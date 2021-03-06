#include "codec.h"
#include "dispatcher.h"
#include "remote_power.pb.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h>
#include <muduo/base/noncopyable.h>

//#include <boost/bind.hpp>
#include <functional>

#include <stdio.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef std::shared_ptr<eh2tech::Query> QueryPtr;
typedef std::shared_ptr<eh2tech::QueryAnswer> QueryAnswerPtr;
typedef std::shared_ptr<eh2tech::Answer> AnswerPtr;
typedef std::shared_ptr<eh2tech::Login> LoginPtr;



struct LogData
{
    //eh2tech::Catalog cat;
    int power[4]; //
    int temp[4];
    int flow[4];
    int press[4];
};


std::shared_ptr<muduo::LogFile> g_logFile;
class PowerServer : muduo::noncopyable
{
    public:
        PowerServer(EventLoop* loop,
                const InetAddress& listenAddr)
            : loop_(loop), 
            server_(loop, listenAddr, "PowerServer"),
            dispatcher_(std::bind(&PowerServer::onUnknownMessage, this, _1, _2, _3)),
            codec_(std::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher_, _1, _2, _3))
    {
        dispatcher_.registerMessageCallback<eh2tech::Setting>(
                std::bind(&PowerServer::onSetting, this, _1, _2, _3));
        dispatcher_.registerMessageCallback<eh2tech::Query>(
                std::bind(&PowerServer::onQuery, this, _1, _2, _3));
        dispatcher_.registerMessageCallback<eh2tech::QueryAnswer>(
                std::bind(&PowerServer::onQueryAnswer, this, _1, _2, _3));
        dispatcher_.registerMessageCallback<eh2tech::Login>(
                std::bind(&PowerServer::onLogin, this, _1, _2, _3));
        dispatcher_.registerMessageCallback<eh2tech::Answer>(
                std::bind(&PowerServer::onAnswer, this, _1, _2, _3));

        server_.setConnectionCallback(
                std::bind(&PowerServer::onConnection, this, _1));
        server_.setMessageCallback(
                std::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));

        loop_->runEvery(10, std::bind(&PowerServer::logData, this));
    }

        void start()
        {
            server_.start();
        }

    private:
        void onConnection(const TcpConnectionPtr& conn)
        {
            LOG_INFO << conn->localAddress().toIpPort() << " -> "
                << conn->peerAddress().toIpPort() << " is "
                << (conn->connected() ? "UP" : "DOWN");
        }

        void onSetting(const TcpConnectionPtr& conn,
                const MessagePtr& message,
                Timestamp)
        {
            LOG_INFO << "onSetting: " << message->GetTypeName();
        }
        void onUnknownMessage(const TcpConnectionPtr& conn,
                const MessagePtr& message,
                Timestamp)
        {
            LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
        }

        void onQueryAnswer(const muduo::net::TcpConnectionPtr& conn,
                const QueryAnswerPtr& message,
                muduo::Timestamp)
        {
            //LOG_INFO << "onQueryAnswer:\n" << message->GetTypeName() << "\n" << message->DebugString();

            // update data_
            switch(message->cat()){
                case eh2tech::Catalog::POWER:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].power[i]=message->data(i);
                    }
                    break;
                case eh2tech::Catalog::TEMP:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].temp[i]=message->data(i);
                    }
                    break;
                case eh2tech::Catalog::FLOW:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].flow[i]=message->data(i);
                    }
                    break;
                case eh2tech::Catalog::PRESS:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].press[i]=message->data(i);
                    }
                    break;
                default: break;
            }    
        }

        void onQuery(const muduo::net::TcpConnectionPtr& conn,
                const QueryPtr& message,
                muduo::Timestamp)
        {
            LOG_INFO << "onQuery:\n" << message->GetTypeName() << message->DebugString();
            if(powers_.count(message->sn())) {
                eh2tech::QueryAnswer msg;
                msg.set_cat(message->cat());
                switch(message->cat()){
                    case eh2tech::Catalog::POWER:
                        for(int i=0; i<4; ++i) {
                            msg.add_data(powers_[message->sn()].power[i]);
                        }
                        break;
                    case eh2tech::Catalog::TEMP:
                        for(int i=0; i<4; ++i) {
                            msg.add_data(powers_[message->sn()].temp[i]);
                        }
                        break;
                    case eh2tech::Catalog::FLOW:
                        for(int i=0; i<4; ++i) {
                            msg.add_data(powers_[message->sn()].flow[i]);
                        }
                        break;
                    case eh2tech::Catalog::PRESS:
                        for(int i=0; i<4; ++i) {
                            msg.add_data(powers_[message->sn()].press[i]);
                        }
                        break;
                    default:
                        break;
                }
                codec_.send(conn, msg); 
            }
        }

        void onLogin(const muduo::net::TcpConnectionPtr& conn, 
                const LoginPtr& message, 
                muduo::Timestamp)
        {
            const char* str_sn = message->sn().c_str();//message中存放的std::string即std::__cxx11::basic_string<char>
            string basename(str_sn);//muduo用的__gnu_cxx::__versa_string<char>
            if(logFiles_.count(message->sn())==0) {
                std::shared_ptr<LogFile> p(new muduo::LogFile(basename, 
                            200*1000,//unit: ms. rollFile every xxx bytes 
                            true, 
                            10, //unit: s. at least xx second interval for every flush
                            10));//unit:times. LogFile::append() at least call times for every flush.
                logFiles_[message->sn()] = p;
            } else {
                LOG_INFO << "same sn login repeated";
            }


        }
        void onAnswer(const muduo::net::TcpConnectionPtr& conn,
                const AnswerPtr& message,
                muduo::Timestamp)
        {
            switch(message->id()) {
                case  0:
                    break;
                case 1:
                    break;
                default:
                    break;
            }

            LOG_INFO << "onAnswer: " << message->GetTypeName();
            //conn->shutdown();
        }

        void logData()
        {
            LOG_INFO << "logging data.";

            muduo::Logger::setOutput(fileOutput);
            muduo::Logger::setFlush(fileFlush);
            for(auto iter = powers_.cbegin(); iter!=powers_.cend(); ++iter){
                g_logFile=logFiles_[iter->first];
                LOG_INFO<< iter->second.power[0] \
                    << "\t" \
                    << iter->second.power[1] \
                    << "\t" \
                    << iter->second.power[2] \
                    << "\t" \
                    << iter->second.power[3] \
                    << "\t" \
                    << iter->second.temp[0] \
                    << "\t" \
                    << iter->second.temp[1] \
                    << "\t" \
                    << iter->second.temp[2] \
                    << "\t" \
                    << iter->second.temp[3] \
                    << "\t" \
                    << iter->second.flow[0] \
                    << "\t" \
                    << iter->second.flow[1] \
                    << "\t" \
                    << iter->second.flow[2] \
                    << "\t" \
                    << iter->second.flow[3] \
                    << "\t" \
                    << iter->second.press[0] \
                    << "\t" \
                    << iter->second.press[1] \
                    << "\t" \
                    << iter->second.press[2] \
                    << "\t" \
                    << iter->second.press[3];
            }

            //restore stdout
            muduo::Logger::setOutput(defaultOutput);
            muduo::Logger::setFlush(defaultFlush);
        }

        static void fileOutput(const char* msg, int len)
        {
            g_logFile->append(msg, len);
        }
        static void fileFlush()
        {
            g_logFile->flush();
        }
        static void defaultOutput(const char* msg, int len)
        {
            size_t n = fwrite(msg, 1, len, stdout);
        }
        static void defaultFlush()
        {
            fflush(stdout);
        }

        EventLoop* loop_;
        TcpServer server_;
        ProtobufDispatcher dispatcher_;
        ProtobufCodec codec_;

        std::map<std::string, LogData > powers_;
        std::map<std::string, std::shared_ptr<LogFile>> logFiles_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {
        EventLoop loop;
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        InetAddress serverAddr(port);
        PowerServer server(&loop, serverAddr);
        server.start();
        loop.loop();
    }
    else
    {
        printf("Usage: %s port\n", argv[0]);
    }
}

