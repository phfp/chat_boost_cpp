#include "conexao_servidor.h"

#define AUTH_SUCCESS "SUCESSO_RESPOSTA"
#define INVALID_NICKNAME "NICKNAME_INVALIDO"

Conexao::Conexao()
    : acceptor(ios, tcp::endpoint(tcp::v4(), 8001)),
      clientList(new list<ClientePtr>),
      filaMensagens(new queue<ClientMessagePtr>),
      log(new Log)
{}

void Conexao::Iniciar()
{
    boost::thread_group threads;

    threads.create_thread(boost::bind(&Conexao::OnInterfaceAdm, this));
    threads.create_thread(boost::bind(&Conexao::Ping, this));
    threads.create_thread(boost::bind(&Conexao::OnNewConnection, this));
    threads.create_thread(boost::bind(&Conexao::OnRequest, this));
    threads.create_thread(boost::bind(&Conexao::OnResponse, this));
    threads.create_thread(boost::bind(&Conexao::OnLogWrite, this));
    threads.join_all();
}

void Conexao::OnInterfaceAdm()
{

    std::cout << "*--------------------COMANDOS-----------------------*" << endl
              << "[/online]                ->  Listar todos usuarios"    << endl
              << "[/desconectar: usuario]  ->  Desconectar usuário"      << endl << endl;

    while(true)
    {
        char inputBuffer[1024] = {0};
        std::cin.getline(inputBuffer, 1024);
        std::string mensagem(inputBuffer);

        if(!mensagem.empty())
        {
            if(mensagem.find("/online") != string::npos)
            {
                for(ClientePtr& client : *clientList)
                {
                    cout << client->nickname << endl;
                }

            }
            else if(mensagem.find("/desconectar") != string::npos)
            {
                string nicknameSelecionado = mensagem.substr(mensagem.find(" ") + 1);

                if(ObterClientePorNickname(nicknameSelecionado) == nullptr)
                {
                    cout << "O usuário não está na lista!" << endl;
                }
                else
                {
                    mutex.lock();
                    OnDisconnect(ObterClientePorNickname(nicknameSelecionado),false);
                    mutex.unlock();
                }
            }
            else
                cout << "Comando nao reconhecido!" << endl;
        }

        mensagem.clear();
        memset(inputBuffer, 0, 1024);
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

ClientePtr Conexao::ObterClientePorNickname(const string &nickname) const
{
    for(ClientePtr& client : *clientList)
    {
        if(client->nickname == nickname)
            return client;
    }
    return nullptr;
}

bool Conexao::ValidarNickname(const string &nickname)
{
    auto it = find_if(clientList->begin(), clientList->end(), [nickname](const ClientePtr c)
    {
       if(c->nickname == nickname)
           return true;
       return false;
    });

    return (it == clientList->end()) ? true : false;
}

void Conexao::OnDisconnect(ClientePtr client, bool queda_comunicacao)
{
    auto position = find_if(clientList->begin(), clientList->end(), [client] (const ClientePtr c)
    {
        return c->nickname == client->nickname;
    });

    string msg = "[" + ObterData() + "] " + client->nickname + " saiu";


    if(!queda_comunicacao)
    {
        client->socket->shutdown(tcp::socket::shutdown_both);
        client->socket->close();
    }

    clientList->erase(position);

    Broadcast(msg);

    log->AppendLog(msg);

    cout << msg << endl;
}

void Conexao::OnNewConnection()
{
    while(true)
    {

        SocketPtr clientSock(new tcp::socket(ios));

        acceptor.accept(*clientSock); //aceita nova conexao

        string name = LerMensagem(clientSock);

        if(!ValidarNickname(name)) // Verifica validade do nickname
        {
            EnviarMensagem(INVALID_NICKNAME, clientSock);
            clientSock->close();
            continue;
        }

        EnviarMensagem(AUTH_SUCCESS, clientSock);

        string msg = "[" + ObterData() + "] " + name + " entrou no chat!\n";

        cout << msg;

        Broadcast(msg);

        log->AppendLog(msg);

        ClientePtr client(new Cliente);
        client->nickname = name;
        client->socket = clientSock;

        mutex.lock();
        clientList->emplace_back(client);
        mutex.unlock();
    }
}

void Conexao::OnRequest()
{
    while(true)
    {
        if(!clientList->empty())
        {
            mutex.lock();

            for(ClientePtr& client : *clientList)
            {

                if(client->socket->available())
                {
                    string msg = LerMensagem(client->socket);

                    if(msg.find("/sair") != string::npos)
                    {
                        ProcessarComando(msg, client);
                        break;
                    }
                    else if(msg.find("/") != string::npos)
                    {
                        ProcessarComando(msg, client);
                        continue;
                    }

                    ClientMessagePtr message(new ClientMessageMap);
                    message->insert(pair<SocketPtr, string>(client->socket, msg));

                    filaMensagens->push(message);
                }
            }
            mutex.unlock();
        }
    }
}

void Conexao::OnResponse()
{
    while(true)
    {
        if(!filaMensagens->empty())
        {
            mutex.lock();

            auto message = filaMensagens->front();
            for(ClientePtr& client : *clientList)
            {
                if(client->socket != message->begin()->first)
                EnviarMensagem(message->begin()->second, client->socket);
            }

            filaMensagens->pop();
            mutex.unlock();
        }
    }
}

void Conexao::OnLogWrite()
{
    while(true)
    {
        log->SaveLog();

        boost::this_thread::sleep(boost::posix_time::millisec(1000));
    }
}

void Conexao::Ping()
{
    while(true)
    {
        if(!clientList->empty())
        {
            for(ClientePtr &client : *clientList)
            {
                boost::system::error_code erro;
                client->socket->write_some(buffer("", 1),erro);

                if(erro.message() == "Broken pipe")
                {
                    OnDisconnect(client,true);
                    break;
                }
                else if(erro){
                    OnDisconnect(client,false);
                    break;
                }
            }
        }
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
    }
}

void Conexao::EnviarMensagem(const string &msg, SocketPtr socket)
{
    if(socket->is_open())
    {
        boost::system::error_code erro;
        socket->write_some(buffer(msg, TAM_MAX_MENSAGEM ), erro);

        if(erro)
        {
            cout << "Falha ao enviar mensagem\n";
        }
    }
    else
    {
        cout << "Falha ao enviar mensagem (Socket->is_open() == false)\n";
    }
}

void Conexao::ProcessarComando(string msg, ClientePtr requisitante)
{
    string cmd = msg;

    if(msg.find("/online") != string::npos)
    {
        string onlineUsersMsg("\n");

        for(ClientePtr& client : *clientList)
        {
            if(client->nickname == requisitante->nickname)
                onlineUsersMsg.append(client->nickname + " (voce)\n");
            else
                onlineUsersMsg.append(client->nickname + "\n");
        }

        EnviarMensagem(onlineUsersMsg, requisitante->socket);
    }
    else if(msg.find("/sair") != string::npos)
    {
        OnDisconnect(requisitante,false);
    }
    //  /from: /to: msg com espacos
    else if(msg.find("/") != string::npos && msg.find(":") != string::npos)
    {
        string from = cmd.substr(0, cmd.find_first_of(":"));
        string message = cmd.substr(cmd.find_last_of(":") + 1);

        auto fdelim = cmd.find("/");
        auto ldelim = cmd.find_last_of(":");
        string to = cmd.substr (fdelim, ldelim - fdelim);

        auto it = std::remove_if(std::begin(to),std::end(to),[](char c){return (c == '/');});
        to.erase(it, std::end(to));

        string content = message.substr(message.find(" ") + 1);

        message = "(PV) " + from + ": " + content;

        SocketPtr socketTo = GetClientSocketByNickname(to);

        if(socketTo)
            EnviarMensagem(message, socketTo);
        else
        {
            SocketPtr sender = GetClientSocketByNickname(from);
            EnviarMensagem("Este usuario nao esta online\n", sender);
        }
    }
}

string Conexao::LerMensagem(SocketPtr clientSock)
{
    char buff[TAM_MAX_MENSAGEM ] = {0};

    boost::system::error_code erro;
    clientSock->read_some(buffer(buff, TAM_MAX_MENSAGEM), erro);

    if(erro)
    {
        cout << erro.message() << "\n";
        clientSock->close();
        return "";
    }

    string txt(buff);

    return txt;
}

string Conexao::ObterData() const
{
    boost::posix_time::ptime time(boost::gregorian::day_clock::local_day(),
                                  boost::posix_time::second_clock::local_time().time_of_day());

    return boost::posix_time::to_simple_string(time);
}

SocketPtr Conexao::GetClientSocketByNickname(const string &nickname) const
{
    for(ClientePtr& client : *clientList)
    {
        if(client->nickname == nickname)
            return client->socket;
    }
    return nullptr;
}

void Conexao::Broadcast(const string &msg)
{
    for(ClientePtr& client : *clientList)
    {
        EnviarMensagem(msg, client->socket);
    }
}



