#include "conexao_mysql.h"

using namespace std;

conexao_mysql::conexao_mysql() : driver(get_driver_instance()),
    con(driver->connect("tcp://127.0.0.1:3306","[usuarioMySql]", "[senhaMySql]"))
{}

bool conexao_mysql::autenticar(std::string nome, std::string senha){

    try {

      con->setSchema("chat"); //seleciona banco de dados

      stmt = con->createStatement();

      string strQuery = "select nickname as nickname from cliente where nickname='"+nome+"' and senha='"+senha+"'";

      res = stmt->executeQuery(strQuery);

      if(res->rowsCount())
      {
          delete res;
          delete stmt;

          return true;
      }
      else
      {
          delete res;
          delete stmt;

          return false;
      }

    } catch (sql::SQLException &e) {
      cout << "# ERR: SQLException in " << __FILE__;
      cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
      cout << "# ERR: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
    return false;
}

