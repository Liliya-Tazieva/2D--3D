#include "dialog.h"
#include "ui_dialog.h"
#include <QFont>
#include <QStringList>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    //Appearance of the main dialog
    ui->setupUi(this);
    QFont fk1("Calibri", 22);
    QFont fk2("Calibri", 18);
    question = new QLabel("What do you want to upload?");
    question->setFont(fk1);
    folder=new QPushButton("&Folder with photos");
    folder->setFont(fk2);
    video=new QPushButton("&Video");
    video->setFont(fk2);
    image_label = new QLabel;
    image_label->setPixmap(QPixmap::fromImage(QImage("Bear.png")));
    l1 = new QHBoxLayout;
    l1->addWidget(video);
    l1->addWidget(folder);
    l2 = new QVBoxLayout;
    l2->addWidget(image_label);
    l2->addWidget(question);
    l2->addLayout(l1);
    l2->addStretch();
    setLayout(l2);
    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setWindowTitle("2Dto3D");

    //Appearance of the dialog, that asks about rubbish
    delete_rubbish = new QDialog;
    del_question = new QLabel("Delete rubbish after recreation?");
    del_question->setFont(fk1);
    yes=new QPushButton("&Yes");
    yes->setFont(fk2);
    no=new QPushButton("&No");
    no->setFont(fk2);
    l3 = new QHBoxLayout;
    l3->addWidget(yes);
    l3->addWidget(no);
    l4 = new QVBoxLayout;
    l4->addWidget(del_question);
    l4->addLayout(l3);
    l4->addStretch();
    delete_rubbish->setLayout(l4);
    delete_rubbish->setMaximumSize(200, 50);
    delete_rubbish->setWindowTitle("Delete rubbish or not");

    //Appearance of the finish window
    finish_window = new QWidget;
    ok_fw = new QPushButton;
    recreated = new QLabel;
    l7 = new QVBoxLayout;
    recreated->setFont(fk1);
    ok_fw->setFont(fk1);
    ok_fw->setText("OK");
    l7->addWidget(recreated);
    l7->addWidget(ok_fw);
    finish_window->move(470,300);
    finish_window->setWindowTitle("Recreation finished");
    finish_window->setLayout(l7);
    finish_window->setWindowFlags(Qt::WindowMinMaxButtonsHint);

    //Signal-slot connection for rubbish-dialog
    connect(yes, SIGNAL(clicked()), this, SLOT(del_rub()), Qt::DirectConnection);
    connect(yes, SIGNAL(clicked()), delete_rubbish, SLOT(accept()), Qt::DirectConnection);
    connect(no, SIGNAL(clicked()), this, SLOT(ignore_rub()), Qt::DirectConnection);
    connect(no, SIGNAL(clicked()), delete_rubbish, SLOT(reject()), Qt::DirectConnection);

    //Signal-slot connection for main dialog
    connect(video, SIGNAL(clicked()), this, SLOT(choose_video()));
    connect(folder, SIGNAL(clicked()), this, SLOT(choose_folder()));
    connect(this, SIGNAL(may_lounch()), this, SLOT(lounch_proc()), Qt::DirectConnection);

    //Signal-slot connection for finish window
    connect(ok_fw, SIGNAL(clicked(bool)), this, SLOT(GUI_enable(bool)));
}
//Delete rubbish
void Dialog::del_rub()
{
    del_or_not = true;
}
//Leave rubbish
void Dialog::ignore_rub()
{
    del_or_not = false;
}
//Choosing video explorer
void Dialog::choose_video()
{
    path.clear();
    explorer = new QFileDialog;
    path = explorer->getOpenFileName(this, "Choosing video", QDir::rootPath());
    //Space to enter frame rate for FFMPEG
    QFont fk1("Calibri", 22);
    what_is_frame_rate = new QLabel("Frame rate is an anmount of frames per second\nin process of splitting video.\nIt can be a float number");
    what_is_frame_rate->setFont(fk1);
    line = new QLineEdit;
    line->setText("Enter frame rate, please");
    line->selectAll();
    line->setFont(fk1);
    line->setFixedHeight(48);
    line->setMinimumWidth(300);
    ok = new QPushButton("&OK");
    ok->setFont(fk1);
    l5 = new QHBoxLayout;
    l5->addWidget(line);
    l5->addWidget(ok);
    l5->addStretch();
    l6 = new QVBoxLayout;
    l6->addWidget(what_is_frame_rate);
    l6->addLayout(l5);
    enter_frame_rate = new QWidget;
    enter_frame_rate->setLayout(l6);
    enter_frame_rate->show();
    enter_frame_rate->move(470,300);
    connect(ok, SIGNAL(clicked()), this, SLOT(rate_entered()), Qt::DirectConnection);
}
//Processing entered frame-rate
void Dialog::rate_entered()
{
    frame_rate.clear();
    frame_rate = line->text();
    enter_frame_rate->close();
    emit Dialog::may_lounch();
}
//Choosing folder with photos explorer
void Dialog::choose_folder()
{
    path.clear();
    explorer = new QFileDialog;
    path = explorer->getExistingDirectory(this, "Choosing folder with photos", QDir::rootPath(),
                                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    emit Dialog::may_lounch();
}
//Disable main dialog, while process is executing (function)
void Dialog::GUI_disable()
{
    video->setEnabled(false);
    video->repaint();
    folder->setEnabled(false);
    folder->repaint();
    question->setText("Recreating...");
    question->repaint();
    image_label->setPixmap(QPixmap::fromImage(QImage("Mesh.png")));
    image_label->repaint();
}
//Enable main dialog, when process has finished (slot)
void Dialog::GUI_enable(bool)
{
    finish_window->close();
    recreation_process_louncer->deleteLater();

    video->setEnabled(true);
    video->repaint();
    folder->setEnabled(true);
    folder->repaint();
    question->setText("What do you want to upload?");
    question->repaint();
    image_label->setPixmap(QPixmap::fromImage(QImage("Bear.png")));
    image_label->repaint();
}
//lounching Recreator.exe with 3 arguments: path, rubbish_flag, frame-rate
void Dialog::lounch_proc()
{
    GUI_disable();

    //Command line args for Recreator.exe
    args.clear();
    current_dir = QDir::currentPath();
    delete_rubbish->exec();
    program = "Recreator.exe";
    args.push_back(current_dir);
    args.push_back(path);
    if(del_or_not) args.push_back("1");
    else args.push_back("0");
    args.push_back(frame_rate);

    //Initialize new thread for recreation process
    recreation_process_louncer = new process_louncher(program, args);
    QThread *thread = new QThread;
    recreation_process_louncer->moveToThread(thread);
    thread->start();

    //Signal-slot connection for process_louncher
    connect(thread, SIGNAL(started()), recreation_process_louncer, SLOT(process()));
    connect(recreation_process_louncer, SIGNAL(finished()), thread, SLOT(quit()), Qt::QueuedConnection);
    connect(recreation_process_louncer, SIGNAL(finished()), this, SLOT(recreation_finished()), Qt::QueuedConnection);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()), Qt::QueuedConnection);
}

void Dialog::recreation_finished()
{
    QProcess::ExitStatus ex_stat = recreation_process_louncer->getExStat();
    if(ex_stat == QProcess::NormalExit) recreated->setText("Recreated successfully");
    else recreated->setText("Something went wrong/nLook into log file");
    finish_window->show();
}

Dialog::~Dialog()
{
    delete ui;
}
