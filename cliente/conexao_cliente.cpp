#include "chat.h"

#define AUTH_SUCCESS "SUCESSO_RESPOSTA"
#define INVALID_NICKNAME "NICKNAME_INVALIDO"

Conexao::Conexao(){
    esta_autenticado = false;
    endpoint = tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8001);
}

bool Conexao::Autentincar(const std::string &nickname, const std::string &senha)
{
    try{
        this->nickname = nickname;
        socket = SocketPtr(new tcp::socket(ios));
        socket->connect(endpoint);

        string msg = nickname+"/"+senha;

        boost::system::error_code erroEscrita;
        socket->write_some(boost::asio::buffer(msg, TAM_MAX_MENSSAGEM ), erroEscrita);

        if(erroEscrita){
            cout << "Error: " << erroEscrita.message() << "\n";
            return false;
        }

        char respostaServidor[TAM_MAX_MENSSAGEM] = {0};
        boost::system::error_code erroLeitura;
        socket->read_some(boost::asio::buffer(respostaServidor, TAM_MAX_MENSSAGEM), erroLeitura);

        if(erroLeitura){
            cout << "Ocorreu um erro ao tentar autenticar.\n" << erroLeitura.message() << "\n";
            return false;
        }

        if(strcmp(respostaServidor, "SENHA_OU_USUARIO_INVALIDO") == 0){
            cout << "Usuário ou senha incorretos.\n";
            return false;
        }

        if(strcmp(respostaServidor, "NICKNAME_INVALIDO") == 0){
            cout << "Este nickname ja esta sendo utilizado, escolha outro.\n";
            return false;
        }

        if(strcmp(respostaServidor, "SUCESSO_RESPOSTA") == 0){
            esta_autenticado = true;
            return true;
        }
    }
    catch(std::exception& e){
        cout << e.what() << endl;
    }
    return false;
}

void Conexao::Ping()
{
    while(true)
    {
        boost::system::error_code erro;
        socket->write_some(boost::asio::buffer("",1), erro);

         if(erro){
            cout << "Voce foi desconectado!" << endl;
            Sair();
            exit(1);
        }

        boost::this_thread::sleep(boost::posix_time::millisec(1000));
    }
}

void Conexao::Iniciar()
{

    boost::thread_group threads;

    threads.create_thread(boost::bind(&Conexao::OuvirNovasMensagens, this));
    threads.create_thread(boost::bind(&Conexao::Ping, this));
    threads.join_all();
}

void Conexao::Sair()
{
    esta_autenticado = false;
    socket->close();
}

void Conexao::EnviarMensagem(const std::string& mensagem){
    boost::system::error_code codErro;

    socket->write_some(boost::asio::buffer(mensagem,TAM_MAX_MENSSAGEM), codErro);

    if(codErro){
        cout << "Erro ao enviar a mensagem." << endl << codErro.message() << endl;
    }
}

void Conexao::OuvirNovasMensagens()
{
    while(true)
    {
        if(socket->available() && esta_autenticado)
        {
            char bufferLeitura[TAM_MAX_MENSSAGEM] = {0};

            int bytesread = socket->read_some(boost::asio::buffer(bufferLeitura, TAM_MAX_MENSSAGEM));

            string mensagem(bufferLeitura, bytesread);

            if(mensagem.size() > 1)
                cout << mensagem << endl;
        }
        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
}
