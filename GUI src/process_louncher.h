#ifndef PROCESS_LOUNCHER_H
#define PROCESS_LOUNCHER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
class process_louncher : public QObject
{
    Q_OBJECT
public:
    process_louncher(QString, QStringList);
    QProcess::ExitStatus getExStat();

signals:
    void finished();

public slots:
    void process();

private:
    QString program;
    QStringList arguments;
    QProcess* recreation_process = nullptr;
};

#endif
