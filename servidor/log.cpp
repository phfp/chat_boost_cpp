#include "log.h"

Log::Log()
    : filaMenssagem( new std::queue<std::string> )
{
    pathLogFile.append("log.txt");
}

void Log::AppendLog(const std::string &msg)
{
    mutex.lock();
    filaMenssagem->push(msg);
    mutex.unlock();
}

void Log::SaveLog()
{
    mutex.lock();
    if(!filaMenssagem->empty())
    {
        WriteToFile(filaMenssagem->front());
        filaMenssagem->pop();
    }
    mutex.unlock();    
}

void Log::WriteToFile(const std::string &msg)
{
    boost::filesystem::ofstream file;

    file.open(pathLogFile, std::ios_base::app);

    if(file.is_open())
    {
        file << msg;
    }

    file.close();
}


