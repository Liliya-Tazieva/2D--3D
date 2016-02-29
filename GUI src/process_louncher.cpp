#include "process_louncher.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

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
    how_finished(recreation_process->exitStatus());
}

//Finish window
void process_louncher::how_finished(QProcess::ExitStatus ex_stat)
{
    QWidget *finish_window = new QWidget;
    QFont fk1("Calibri", 22);
    QPushButton *ok_fw = new QPushButton;
    QLabel *label = new QLabel;
    QVBoxLayout *l = new QVBoxLayout;
    label->setFont(fk1);
    if(ex_stat == QProcess::NormalExit) label->setText("Recreated successfully");
    else label->setText("Something went wrong/nLook into log file");
    ok_fw->setFont(fk1);
    ok_fw->setText("OK");
    l->addWidget(label);
    l->addWidget(ok_fw);
    finish_window->move(470,300);
    finish_window->setWindowTitle("Recreation finished");
    finish_window->setLayout(l);
    finish_window->show();

    connect(ok_fw, SIGNAL(clicked(bool)), finish_window, SLOT(close()));
    connect(ok_fw, SIGNAL(clicked(bool)), this, SLOT(finishing_process(bool)));
}

//After recreation, thread's work is done
void process_louncher::finishing_process(bool)
{
    emit process_louncher::finished();
}
