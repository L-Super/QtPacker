#include "CmdProcess.h"
#include <QProcess>
#include "Log.h"

CmdProcess::CmdProcess(QObject *parent)
    : QObject{parent},
      p(new QProcess())
{
    connect(p, &QProcess::readyReadStandardOutput, this, [=] {
        emit readStandardOutput(p->readAllStandardOutput());

    });
    connect(p, &QProcess::readyReadStandardError, this, [=] {
        emit readStandardError(p->readAllStandardError());
    });
}

//1. D:/Qt/5.15.2/mingw81_64/bin/qtenv2.bat
//2. windeployqt D:/Code/QtProjects/process_test.exe
bool CmdProcess::StartProcess(const QString& envBatPath, const QString &appPath)
{
    p->setProgram("cmd");
    p->start();
    p->waitForStarted();

    auto bat = envBatPath + "\n";
    QByteArray windeployqt = "windeployqt " + appPath.toLocal8Bit() + "\n";
    p->write(bat.toLocal8Bit());
    p->write(windeployqt);

    qcout<< "state"<< p->state()<<"id"<<p->processId();

//    p->waitForFinished(3000);
    //TODO:结束进程
    p->terminate();
    qcout<< "state"<< p->state();
//    p->closeWriteChannel();

//    if (!p->waitForFinished()) {
//        qcout<<"QProcess run failed";
//        emit error();
//        return false;
//    }
    emit finished();
    return true;
}
