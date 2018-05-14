#include "codec.h"
#include "dispatcher.h"
#include "remote_power.pb.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h>
#include "muduo/base/noncopyable.h"

//#include <boost/bind.hpp>
#include <functional>

#include <stdio.h>
#include <iostream>
#include <exception>

#include <mysql/mysql.h>
#include <time.h>
using namespace muduo;
using namespace muduo::net;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

typedef std::shared_ptr<eh2tech::Query> QueryPtr;
typedef std::shared_ptr<eh2tech::QueryAnswer> QueryAnswerPtr;
typedef std::shared_ptr<eh2tech::Answer> AnswerPtr;
typedef std::shared_ptr<eh2tech::Login> LoginPtr;


#define HOST "localhost"
#define USERNAME "eh2tech"
#define PASSWORD "eh2tech"
#define DATABASE "remotepower"


struct LogData
{
    //eh2tech::Catalog cat;
    int deviceid;
    int power[4]; //
    int temp[4];
    int flow[4];
    int press[4];
	int fuel;
	int alarm[4];
	int totaltime;
	int fcState;
	int preFcState;
	int fcStateFlag;
	time_t startTime;
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

	void SaveDatatoDB(const char *col,int deviceid,int *val)
	{
		MYSQL dbconn;
		int res;
		if(deviceid==0) return;
		mysql_init(&dbconn);
		if(mysql_real_connect(&dbconn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS)) 
		{
			char sql[2048];
			sprintf(sql,"insert into %s values(%d,now(),%d,%d,%d,%d)", col,deviceid,val[0],val[1],val[2],val[3]);
			res=mysql_query(&dbconn,sql);
			if(res)
			{
				LOG_INFO <<"Mysql ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn)<<"\n"<< sql;
			}
			mysql_close(&dbconn);
		}
		else
		{
			LOG_INFO <<"Mysql Connect failt! ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn);
		}
	}
	
	const char * GetAlarmName(int alarmno){
		switch(alarmno){
			case 1: return "tbhigh";
			case 2: return "tblow";
			case 3: return "tchigh";
			case 4: return "tclow";
			case 5: return "tfhighe";
			case 6: return "tflow";
			case 7: return "psyshigh";
			case 8: return "ph2high";
			case 9: return "psyslow";
			case 10: return "ph2low";
			case 11: return "vbatlow";
			case 21: return "dcdc";
			case 22: return "fc_temp";
			case 23: return "fc_ph2";
			case 50: return "rpmzero";
			default: return NULL;
		}
	}

	void SaveAlarmtoDB(const char *col,int deviceid,int *val)
	{
		MYSQL dbconn;
		int res;
		if(deviceid==0) return;
		mysql_init(&dbconn);
		if(mysql_real_connect(&dbconn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS)) 
		{
			char sql[1024];
			sprintf(sql,"insert into %s values(%d,now(),%d,%d,\"%d,%d\")", col,deviceid,val[0],val[1],val[2],val[3]);
			res=mysql_query(&dbconn,sql);
			if(res)
			{
				LOG_INFO <<"Mysql ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn)<<"\n"<< sql;
			}
			const char * alarmname=0;
			alarmname = GetAlarmName(val[0]);
			if(alarmname!=NULL){
				sprintf(sql,"update devices  set %s =%d where id=%d",alarmname,val[1],deviceid);
				res=mysql_query(&dbconn,sql);
				if(res)
				{
					LOG_INFO <<"Mysql ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn)<<"\n"<< sql;
				}
			}
			mysql_close(&dbconn);
		}
		else
		{
			LOG_INFO <<"Mysql Connect failt! ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn);
		}
	}

	void UpdateFuel(int deviceid,int val)
	{
		MYSQL dbconn;
		int res;
		if(deviceid==0) return;
		mysql_init(&dbconn);
		if(mysql_real_connect(&dbconn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS)) 
		{
			char sql[2048];
			sprintf(sql,"update devices  set fuel = fuel+%d where id=%d", val,deviceid);
			res=mysql_query(&dbconn,sql);
			if(res)
			{
				LOG_INFO <<"Mysql ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn)<<"\n"<< sql;
			}
			mysql_close(&dbconn);
		}
		else
		{
			LOG_INFO <<"Mysql Connect failt! ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn);
		}
	}

	void UpdateTotalRunTime(LogData *dev)//int deviceid,int val,int pre)
	{
		MYSQL dbconn;
		int res;
		int totalTime=0;
		if(dev->deviceid==0) return;
		if(dev->preFcState!=dev->fcState){
			time_t xx;
			time(&xx);
			//LOG_INFO <<"State:"<<dev->fcState<<",PreState:"<<dev->preFcState;
			if(dev->preFcState==0&&dev->fcState!=0){
				dev->preFcState = dev->fcState;
				time(&dev->startTime);
				//LOG_INFO <<"TIME of Start:"<<dev->startTime;
				return;
			}
			if(dev->fcState==0&&dev->preFcState!=0){
				dev->preFcState = dev->fcState;
				time_t tt;
				time(&tt);
				totalTime = difftime(tt,dev->startTime);
				totalTime=(int)(totalTime/60);
				time(&dev->startTime);
				if(totalTime<=0) return;
			}else{
				dev->preFcState = dev->fcState;
				return;
			}
		}else{
			if(dev->fcState!=0){
				if(dev->fcStateFlag==1){
					time_t tt;
					time(&tt);
					totalTime = difftime(tt,dev->startTime);
					totalTime=(int)(totalTime/60);
					time(&dev->startTime);
				}
				if(totalTime<=0) return;
			}else{
				return;
			}
		}
		mysql_init(&dbconn);
		if(mysql_real_connect(&dbconn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS)) 
		{
			char sql[1024];
			sprintf(sql,"update devices  set totaltime = totaltime + %d where id=%d", totalTime,dev->deviceid);
			res=mysql_query(&dbconn,sql);
			if(res)
			{
				LOG_INFO <<"Mysql ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn)<<"\n"<< sql;
			}
			mysql_close(&dbconn);
		}
		else
		{
			LOG_INFO <<"Mysql Connect failt! ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn);
		}
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
  		    SaveDatatoDB("power(deviceid,uptime,vstack,istack,vout,iout)", powers_[message->sn()].deviceid,powers_[message->sn()].power);
                    break;
                case eh2tech::Catalog::TEMP:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].temp[i]=message->data(i);
                    }
					SaveDatatoDB("temp(deviceid,uptime,rfm_tb,rfm_tc,fc_tin,fc_tout)",\
											powers_[message->sn()].deviceid,powers_[message->sn()].temp);
                    break;
                case eh2tech::Catalog::FLOW:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].flow[i]=message->data(i);
                    }
					SaveDatatoDB("flow(deviceid,uptime,rfm_pumptocat,rfm_back,fc_fanspeed,fc_back)",\
										   powers_[message->sn()].deviceid,powers_[message->sn()].flow);
                    break;
                case eh2tech::Catalog::PRESS:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].press[i]=message->data(i);
                    }
					SaveDatatoDB("press(deviceid,uptime,rfm_syspress,rfm_h2press,vout,iout)",\
											powers_[message->sn()].deviceid,powers_[message->sn()].press);
                    break;
                case eh2tech::Catalog::ALARM:
                    for(int i=0; i<message->data_size(); ++i) {
                        powers_[message->sn()].alarm[i]=message->data(i);
                    }
                    SaveAlarmtoDB("alarm(deviceid,uptime,alarmno,level,note)",\
                                powers_[message->sn()].deviceid,powers_[message->sn()].alarm);
					break;
                case eh2tech::Catalog::FUEL:
                    powers_[message->sn()].fuel = message->data(0);
                    UpdateFuel(powers_[message->sn()].deviceid,powers_[message->sn()].fuel);
                    break;
                case eh2tech::Catalog::TOTALTIME:
                    //powers_[message->sn()].totaltime = message->data(0);
                    powers_[message->sn()].fcState = message->data(0);
					powers_[message->sn()].fcStateFlag = message->data(1);
                    UpdateTotalRunTime(&powers_[message->sn()]);//powers_[message->sn()].deviceid,powers_[message->sn()].totaltime,powers_[message->sn()].preFcState);
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
	    if(powers_.count(message->sn())==0){
		MYSQL dbconn;
		int res;
		mysql_init(&dbconn);
		if(mysql_real_connect(&dbconn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS)) 
		{
			MYSQL_RES *result;
			MYSQL_ROW sql_row;	
			char sql[2048];
			sprintf(sql,"select * from devices where sn='%s'",message->sn().data());
			res=mysql_query(&dbconn,sql);
			if(res)
			{
				LOG_INFO <<"Mysql ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn)<<"\n"<< sql;
			}else{
				result=mysql_store_result(&dbconn);
				if(result){
					sql_row=mysql_fetch_row(result);
					powers_[message->sn()].deviceid=atoi(sql_row[0]);
					time(&powers_[message->sn()].startTime);
					powers_[message->sn()].preFcState=0;
					powers_[message->sn()].fcStateFlag=0;
				}
			}
			if(result!=NULL)
				mysql_free_result(result);
			mysql_close(&dbconn);
		}else{
			LOG_INFO <<"Mysql Connect failt! ERROR:"<< mysql_errno(&dbconn)<<"-"<< mysql_error(&dbconn);
		}
	    }else{
		    LOG_INFO <<"SN:"<<message->sn()<<" reLogin!";
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
    int pt=5010;
    if (argc > 1)
    {
	    pt = atoi(argv[1]);
    }
        
        LOG_INFO <<"Listen:" << pt ;
	EventLoop loop;
        uint16_t port = static_cast<uint16_t>(pt);
        InetAddress serverAddr(port);
        PowerServer server(&loop, serverAddr);
        server.start();
        loop.loop();
}

