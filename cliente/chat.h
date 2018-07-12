#ifndef CHAT_H
#define CHAT_H

#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <conexao_cliente.h>


class Chat{
private:
    void Conversa();
    boost::shared_ptr<Conexao> conexao;

public:
    Chat();
    void Autenticacao();
    void InciciarChat();
    void TerminarChat();
};

#endif // CHAT_H
