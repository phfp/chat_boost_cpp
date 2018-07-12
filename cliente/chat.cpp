#include "chat.h"
#include "conexao_cliente.h"

Chat::Chat() : conexao(new Conexao){}

void Chat::Autenticacao()
{

    bool autenticacao = false;

    while(!autenticacao)
    {
        std::string nickname;
        std::cout << "Nickname: ";
        std::cin >> nickname;
        autenticacao = conexao->Autentincar(nickname);
    }
}

void Chat::InciciarChat(){

    std::system("clear");

    if(conexao->Autenticado())
    {
        boost::thread enviarMsgTrd(boost::bind(&Chat::Conversa, this));
        enviarMsgTrd.detach();
        conexao->Iniciar();
    }
}

void Chat::Conversa()
{
    std::system("clear");
    std::cout << "*--------------------COMANDOS-----------------------*" << endl
              << "[/sair]               ->  Sair do chat"                << endl
              << "[/online]             ->  Listar todos usuarios"       << endl
              << "[/usuario: mensagem]  ->  Enviar uma mensagem privada" << endl << endl;
    while(true)
    {
        char inputBuffer[1024] = {0};
        std::cin.getline(inputBuffer, 1024);
        std::string mensagem(inputBuffer);

        if(!mensagem.empty())
        {
            if(mensagem.find("/sair") != string::npos)
            {
                conexao->EnviarMensagem(conexao->ObterNickname() + ": " + mensagem);
                 TerminarChat();
            }else
                conexao->EnviarMensagem(conexao->ObterNickname() + ": " + mensagem);
        }

        mensagem.clear();
        memset(inputBuffer, 0, 1024);
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}

void Chat::TerminarChat(){
    conexao->Sair();
    exit(1);
}
