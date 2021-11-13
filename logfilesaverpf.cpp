#include "logfilesaverpf.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>

LogFileSaverPf::LogFileSaverPf()
{
  openFile();
writeHeader();

}


LogFileSaverPf LogFileSaverPf::logfilesaver;

void LogFileSaverPf::writeString( std::stringstream & entry)
{

    myfile << entry.rdbuf()<<std::endl;
    myfile.flush();

}
//void LogFileSaverPf::writeEntry(LogEntry entry)
//{

//    myfile << entry.ts<<"\t"<< entry.dist<<"\t"<< entry.angle<<"\t"<< (int)entry.lightBumps<<"\n";
//}



void LogFileSaverPf::openFile()
{
    string filename;
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S.vsmlog");
    filename = ss.str();
       myfile= ofstream(filename);
    //myfile = std::fstream(filename, std::ios::out);// | std::ios::binary);
    this->fileName =filename;

}

void LogFileSaverPf::writeHeader()
{
    myfile<< "time\tdist\tangle\tlightbumps\n";

}

bool LogFileSaverPf::closeFile()
{
    getCurrentFileSize(); //writes file size in fileSize field for possible later access
    myfile.close();
    return (myfile.fail());

}

