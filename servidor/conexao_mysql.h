#ifndef CONEXAO_MYSQL_H
#define CONEXAO_MYSQL_H

#include <string>
#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class conexao_mysql
{
private:
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

public:
    conexao_mysql();
    bool autenticar(std::string nome, std::string senha);
};

#endif // CONEXAO_MYSQL_H
