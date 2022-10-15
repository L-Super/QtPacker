#pragma once

#include <QMainWindow>
#include <QProcess>
#include "QtPath.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class PackTool;
}
QT_END_NAMESPACE

class QLabel;
class QMovie;
class QVBoxLayout;
class ProgressWidget : public QWidget
{
    Q_OBJECT
public:
    ProgressWidget(QWidget *parent = nullptr);
    void Start();
    void Stop();

private:
    QLabel *label;
    QMovie *movie;
    QVBoxLayout *layout;
};

class ZipTool;
class CmdProcess;
class PackTool : public QMainWindow
{
    Q_OBJECT

public:
    PackTool(QWidget *parent = nullptr);
    ~PackTool();
    void OpenAppPath();
    void SavePath();
    void SetComboBox();
    bool CopyApp();
    int LaunchProcess();
    void SelectionAfterProcessDone();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_appPathPushButton_clicked();

    void on_savePathPushButton_clicked();

    void on_packPushButton_clicked();

    void on_qtPathPushButton_clicked();

signals:
    void zipSignal(const QString &, const QString &);

private:
    Ui::PackTool *ui;
    QLabel *versionLabel;
    QLabel *instructionLabel;
    QString filePathAndName; // 文件路径及文件名
    QString appPath;         // 文件路径
    QString appName;
    QString savePath;
    QVector<QString> compilers; // windeployqt路径及文件名
    QProcess *p;
    QtPath qtPath;
    ProgressWidget *maskLayer;
    ZipTool *zip;
    QThread *workerThread;
    CmdProcess *cmd;
};
