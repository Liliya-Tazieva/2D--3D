#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QLineEdit>
#include <QDir>
#include <QThread>
#include <QImage>
#include "process_louncher.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    QString current_dir;
    QLabel *question;
    QPushButton *folder;
    QPushButton *video;
    QFileDialog *explorer;
    QHBoxLayout *l1;
    QVBoxLayout *l2;
    QString path;
    QString program;
    QStringList args;
    QDialog *delete_rubbish;
    QLabel *del_question;
    QPushButton *yes;
    QPushButton *no;
    QHBoxLayout *l3;
    QVBoxLayout *l4;
    QHBoxLayout *l5;
    QVBoxLayout *l6;
    QPushButton *ok;
    QWidget *enter_frame_rate;
    QLabel *what_is_frame_rate;
    QLineEdit *line;
    QLabel *image_label;
    bool del_or_not;
    QString frame_rate = "0";
    process_louncher *recreation_process_louncer;
    QWidget *finish_window;
    QPushButton *ok_fw;
    QLabel *recreated;
    QVBoxLayout *l7;
    ~Dialog();

    void GUI_disable();

public slots:
    void choose_video();
    void choose_folder();
    void del_rub();
    void ignore_rub();
    void rate_entered();
    void lounch_proc();
    void GUI_enable(bool);
    void recreation_finished();

signals:
    void may_lounch();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
