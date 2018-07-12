#include <iostream>
#include <boost/shared_ptr.hpp>
#include "conexao_servidor.h"

using namespace std;
using namespace boost;


int main()
{
    boost::shared_ptr<Conexao> chat(new Conexao);

    std::cout << "Servidor chat iniciado..." << endl;

    chat->Iniciar();

    return 0;
}
