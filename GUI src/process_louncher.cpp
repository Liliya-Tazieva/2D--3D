#include "process_louncher.h"
#include <QLabel>

//Constructor
/*
process_louncher::process_louncher(QObject *parent, QString prog, QStringList args) : QObject(parent)
{
    program = prog;
    arguments = args;
}*/

//My constuctor
process_louncher::process_louncher(QString prog, QStringList args)
{
    program = prog;
    arguments = args;
}

//Finish window
void process_louncher::how_finished(QProcess::ExitStatus ex_stat)
{
    QFont fk1("Calibri", 22);
    QLabel *finish_window;
    finish_window = new QLabel;
    finish_window->setFont(fk1);
    if(ex_stat == QProcess::NormalExit) finish_window->setText("Recreated successfully");
    else finish_window->setText("Something went wrong/nLook into log file");
    finish_window->move(470,300);
    finish_window->setMaximumSize(300,100);
    finish_window->show();
}

void process_louncher::process()
{
    recreation_process = new QProcess();
    recreation_process->start(program, arguments);

    bool f = recreation_process->waitForFinished(-1);
    if(f) how_finished(recreation_process->exitStatus());
    //else ////////////////////;
    delete recreation_process;
    emit process_louncher::finished();
}
