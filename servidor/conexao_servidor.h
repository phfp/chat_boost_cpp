#ifndef CONEXAO_SERVIDOR_H
#define CONEXAO_SERVIDOR_H

#include "log.h"
#include <iostream>
#include <map>
#include <string>
#include <queue>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> SocketPtr;

struct Cliente {
    std::string nickname;
    SocketPtr socket;
};

typedef boost::shared_ptr<Cliente>                     ClientePtr; //ponteiro shared_ptr para Cliente
typedef std::map<SocketPtr, string>                    ClientMessageMap;
typedef boost::shared_ptr<ClientMessageMap>            ClientMessagePtr;
typedef boost::shared_ptr< list<ClientePtr> >          ClientListPtr;
typedef boost::shared_ptr< queue<ClientMessagePtr> >   ClientMsgQueuePtr;

#define TAM_MAX_MENSAGEM 1024

class Conexao
{
private:

    void OnInterfaceAdm();
    bool ValidarNickname(const string& nickname);
    void OnDisconnect(ClientePtr cliente, bool queda_comunicacao);
    void OnNewConnection();
    void OnRequest();
    void OnResponse();
    void OnLogWrite();
    void Ping();
    void EnviarMensagem(const string& msg, SocketPtr socket);
    void ProcessarComando(string msg, ClientePtr requisitante);
    void Broadcast(const string& msg);

    string LerMensagem(SocketPtr clientSock);
    string ObterData() const;
    SocketPtr GetClientSocketByNickname(const string& nickname) const;
    ClientePtr ObterClientePorNickname(const string& nickname) const;

    io_service          ios;
    tcp::acceptor       acceptor;
    boost::mutex        mutex;

    ClientListPtr       clientList;
    ClientMsgQueuePtr   filaMensagens;
    LogPtr              log;

public:
    Conexao();
    void Iniciar();
};

#endif // CONEXAO_SERVIDOR_H
