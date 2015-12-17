#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QLineEdit>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
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
    QHBoxLayout *l6;
    QLineEdit *line;
    QPushButton *ok;
    QWidget *enter_frame_rate;
    QProcess *recreation_process = nullptr;
    QLabel *how_finished;
    bool del_or_not;
    QString frame_rate = "0";
    ~Dialog();

public slots:
    void choose_video();
    void choose_folder();
    void del_rub();
    void ignore_rub();
    void rate_entered();
    void GUI_disable();
    void lounch_proc();

signals:
    void main_dialog();
    void may_lounch();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
