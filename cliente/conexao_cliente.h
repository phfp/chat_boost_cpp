#ifndef CONEXAO_H
#define CONEXAO_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <queue>

using namespace std;
using namespace boost;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> SocketPtr;

#define TAM_MAX_MENSSAGEM 1024

class Conexao{
private:
    string                      nickname;
    bool                        esta_autenticado;
    SocketPtr                   socket;
    boost::asio::io_service     ios;
    tcp::endpoint               endpoint;
    boost::mutex                mutex;
    void OuvirNovasMensagens();

public:
    void Ping();
    Conexao();
    bool Autentincar(const string &nickname);
    void Iniciar();
    void Sair();
    void EnviarMensagem(const string &mensagem);
    bool Autenticado() const {return esta_autenticado;}
    string ObterNickname() const {return nickname;}
};

#endif // CONEXAO_H
