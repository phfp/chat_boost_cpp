#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <queue>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

typedef boost::shared_ptr< std::queue<std::string> > MessageQueuePtr;

class Log
{
public:
    Log();

    void AppendLog(const std::string& msg);
    void SaveLog();

private:
    void WriteToFile(const std::string& msg);

    boost::filesystem::path pathLogFile;
    MessageQueuePtr filaMenssagem;
    boost::mutex mutex;
};

typedef boost::shared_ptr< Log > LogPtr;

#endif // LOG_H
