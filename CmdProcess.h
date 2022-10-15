#ifndef CMDPROCESS_H
#define CMDPROCESS_H

#include <QObject>
/**
 * @brief 调用QProcess执行windeployqt命令
 */
class QProcess;
class CmdProcess : public QObject
{
    Q_OBJECT
public:
    explicit CmdProcess(QObject *parent = nullptr);
    bool StartProcess(const QString &envBatPath, const QString& appPath);

signals:
    void finished();
    void error();
    void readStandardOutput(const QByteArray &);
    void readStandardError(const QByteArray&);

private:
    QProcess *p;
};

#endif // CMDPROCESS_H
