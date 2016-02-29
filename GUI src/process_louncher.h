#ifndef PROCESS_LOUNCHER_H
#define PROCESS_LOUNCHER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QWidget>

class process_louncher : public QObject
{
    Q_OBJECT
public:

    process_louncher(QString, QStringList);
    void how_finished(QProcess::ExitStatus);
    QWidget *finish_window;

signals:
    void finished();

public slots:
    void process();
    void finishing_process(bool);

private:
    QString program;
    QStringList arguments;
    QProcess* recreation_process = nullptr;
};

#endif
