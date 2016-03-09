#include "process_louncher.h"

//My constuctor
process_louncher::process_louncher(QString prog, QStringList args)
{
    program = prog;
    arguments = args;
}

//Recreation process
void process_louncher::process()
{
    recreation_process = new QProcess();
    recreation_process->start(program, arguments);
    recreation_process->waitForFinished(-1);


    emit process_louncher::finished();
}

QProcess::ExitStatus process_louncher::getExStat()
{
    return recreation_process->exitStatus();
}
