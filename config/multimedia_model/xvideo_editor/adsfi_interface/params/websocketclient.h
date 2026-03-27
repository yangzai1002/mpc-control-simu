#ifndef __AVOSPARAMWEBCLIENT_H__
#define __AVOSPARAMWEBCLIENT_H__

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>

#include <websocketpp/base64/base64.hpp>

#include <string>
#include <math.h>

//#include <roscpp_json_serialize/serialize.h>
//#include <roscpp_json_serialize/deserialize.h>
//#include <rapidjson/writer.h>
//#include <rapidjson/stringbuffer.h>
#include <iostream>

#include "param_globalque.h"
#include "shm_param_datapool.h"

// #include "geo/geotool.h"

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using namespace std;

class CAvosParamWebClient
{
public:
    std::string uri = "";
    std::string node_name = "";
    int wait_n_us = 1000*1000*3;

    typedef websocketpp::client<websocketpp::config::asio_client> client;
    typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;

    CAvosParamWebClient() : m_open(false), m_done(false)
    {
        // set up access channels to only log interesting things
        m_client.clear_access_channels(websocketpp::log::alevel::all);
        m_client.set_access_channels(websocketpp::log::alevel::connect);
        m_client.set_access_channels(websocketpp::log::alevel::disconnect);
        m_client.set_access_channels(websocketpp::log::alevel::app);

        m_client.init_asio();

        // Bind the handlers we are using
        using websocketpp::lib::bind;
        using websocketpp::lib::placeholders::_1;
        m_client.set_open_handler(bind(&CAvosParamWebClient::on_open, this, websocketpp::lib::placeholders::_1));
        m_client.set_close_handler(bind(&CAvosParamWebClient::on_close, this, websocketpp::lib::placeholders::_1));
        m_client.set_fail_handler(bind(&CAvosParamWebClient::on_fail, this, websocketpp::lib::placeholders::_1));
        m_client.set_message_handler(bind(&CAvosParamWebClient::on_message, this, websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));

        param_que = CParamGlobalQue::Instance();
        param_protocol = CParamProtocol::Instance();
        shm_ptr = SHMParamApi::Instance();
    }

    std::string m_status;
    std::string pointcloud;
    void wait_a_bit()
    {
#ifdef WIN32
        Sleep(1000);
#else
        usleep(wait_n_us);
#endif
    }

    void init()
    {
        // Create a new connection to the given URI
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);

        if (ec)
        {
            m_client.get_alog().write(websocketpp::log::alevel::app,
                                      "Get Connection Error: " + ec.message());
            return;
        }

        // Grab a handle for this connection so we can talk to it in a thread
        // safe manor after the event loop starts.
        m_hdl = con->get_handle();

        // Queue the connection. No DNS queries or network connections will be
        // made until the io_service event loop is run.
        m_client.connect(con);
    }


    // This method will block until the connection is complete
    void run()
    {
        init();

        // Create a thread to run the ASIO io_service event loop
        websocketpp::lib::thread asio_thread(&client::run, &m_client);

        // Create a thread to run the telemetry loop
        websocketpp::lib::thread telemetry_thread(&CAvosParamWebClient::telemetry_loop, this);

        asio_thread.join();
        telemetry_thread.join();
    }

    bool sendMsg(string msg)
    {
        if(msg=="")
        {
            return true;
        }
        websocketpp::lib::error_code ec;
        m_client.send(m_hdl, msg, websocketpp::frame::opcode::text, ec);

        // The most likely error that we will get is that the connection is
        // not in the right state. Usually this means we tried to send a
        // message to a connection that was closed or in the process of
        // closing. While many errors here can be easily recovered from,
        // in this simple example, we'll stop the telemetry loop.
        if (ec)
        {
            m_client.get_alog().write(websocketpp::log::alevel::app,
                                      "Send Error: " + ec.message());
            return false;
        }
        return true;

    }

    // The open handler will signal that we are ready to start sending telemetry
    void on_open(websocketpp::connection_hdl)
    {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Connection opened, starting telemetry!");

        scoped_lock guard(m_lock);
        m_open = true;
    }

    // The close handler will signal that we should stop sending telemetry
    void on_close(websocketpp::connection_hdl)
    {
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Connection closed, stopping telemetry!");

        scoped_lock guard(m_lock);
        m_done = true;
        param_que->param_sem.setZero();
    }

    // The fail handler will signal that we should stop sending telemetry
    void on_fail(websocketpp::connection_hdl)
    {
        if (getenv("AVOS_MIDDLEWARE_DEBUG") && strcmp(getenv("AVOS_MIDDLEWARE_DEBUG"), "1") == 0)
        m_client.get_alog().write(websocketpp::log::alevel::app,
                                  "Connection failed, stopping telemetry!");

        scoped_lock guard(m_lock);
        m_done = false;
        m_open = false;

        usleep(1000000 * 5);

        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        m_hdl = con->get_handle();
        m_client.connect(con);
    }

    void on_message(connection_hdl hdl, client::message_ptr msg)
    {
        //        cout<<msg->get_payload().c_str()<<endl;
        if(msg->get_payload().find("PARAM_RESPONSE") != string::npos)
        {
            // cout<<msg->get_payload().c_str()<<endl;
            std::string strRep = msg->get_payload();
            param_que->pushResQue(strRep);
            param_que->param_rep_sem.Post();
        }
        if(msg->get_payload().find(PARAM_SET_CMD) != string::npos)
        {
            setParam2Shm(msg->get_payload());
        }

    }

    void setParam2Shm(string msg)
    {
        vector<string> res_ver;
        CShmBaseFunc::Instance()->splitWithStl(msg,res_ver,":");
        if(res_ver.size()>=6)
        {
            string module_name=res_ver[2];
            string param_name=res_ver[3];
            string param_type=res_ver[4];
            string param_value=res_ver[5];
            if (param_type == "-i")
            {
                if(CShmBaseFunc::Instance()->isContain(param_value,","))
                {
                    vector<int> data_int_vec;
                    CShmBaseFunc::Instance()->splitWithStl(param_value,data_int_vec,",");
                    if(shm_ptr->SetIntModuleParam(param_name, data_int_vec, module_name))
                        std::cout << "set ok" << std::endl;
                    else
                        std::cout << "set fault" << std::endl;
                }
                else
                {
                    int data;
                    std::stringstream ss;
                    ss << std::string(param_value);
                    ss >> data;
                    if(shm_ptr->SetIntModuleParam(std::string(param_name), data,std::string(module_name)))
                        std::cout << "set ok" << std::endl;
                    else
                        std::cout << "set fault" << std::endl;
                }
                return ;
            }
            if (std::string(param_type) == "-d")
            {
                if(CShmBaseFunc::Instance()->isContain(string(param_value),","))
                {
                    vector<double> data_int_vec;
                    CShmBaseFunc::Instance()->splitWithStl(string(param_value),data_int_vec,",");
                    if(shm_ptr->SetDoubleModuleParam(std::string(param_name), data_int_vec,std::string(module_name)))
                        std::cout << "set ok" << std::endl;
                    else
                        std::cout << "set fault" << std::endl;
                }
                else
                {
                    double data;
                    std::stringstream ss;
                    ss << std::string(param_value);
                    ss >> data;
                    if(shm_ptr->SetDoubleModuleParam(std::string(param_name), data,std::string(module_name)))
                        std::cout << "set ok" << std::endl;
                    else
                        std::cout << "set fault" << std::endl;
                }
                return ;
            }
            if (std::string(param_type) == "-s")
            {

                if(CShmBaseFunc::Instance()->isContain(string(param_value),","))
                {
                    vector<string> data_int_vec;
                    CShmBaseFunc::Instance()->splitWithStl(string(param_value),data_int_vec,",");
                    if(shm_ptr->SetStringModuleParam(std::string(param_name), data_int_vec,std::string(module_name)))
                        std::cout << "set ok" << std::endl;
                    else
                        std::cout << "set fault" << std::endl;
                }
                else
                {
                    if (shm_ptr->SetStringModuleParam(std::string(param_name), std::string(param_value), std::string(module_name)))
                        std::cout << "set ok" << std::endl;
                    else
                        std::cout << "set fault" << std::endl;
                }
                return ;
            }
        }

    }

    void telemetry_loop()
    {
        uint64_t count = 0;
        std::stringstream val;
        websocketpp::lib::error_code ec;

        while (1)
        {
            bool wait = false;
            {
                scoped_lock guard(m_lock);

                // If the connection has been closed, stop generating telemetry
                if (m_done)
                {
                    break;
                }

                // If the connection hasn't been opened yet wait a bit and retry
                if (!m_open)
                {
                    wait = true;
                }
            }

            if (wait)
            {
                wait_a_bit();
                continue;
            }
            if(!m_registered)
            {
                string registered = PARAM_REGISTER_NODE+node_name;
                sendMsg(registered);
                m_registered = true;
            }
            param_que->param_sem.sem_timedwait_millsecs(1000);
            if(!sendMsg(param_que->popLogQue()))
            {
                break;
            }
        }
    }

    bool isOpen(){return m_open;}
    bool isDone(){return m_done;}

private:
    client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::mutex m_lock;
    bool m_open;
    bool m_done;
    bool m_registered = false;

    CParamGlobalQue * param_que;
    CParamProtocol * param_protocol;
    string out_cmd;
    SHMParamApi* shm_ptr;
};

class CAvosParamWebSocketClient
{
public:
    CAvosParamWebSocketClient(const string &node_name);
    ~CAvosParamWebSocketClient();
    bool Init();
    int Run();
    void Close();

private:
    string m_node_name;
    CAvosParamWebClient* chmi;
public:
    int wait_n_us = 1000*1000*3;
};

#endif
