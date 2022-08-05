#pragma once

#include <QMainWindow>
#include <QProcess>
#include "QtPath.h"

#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class PackagingTool; }
QT_END_NAMESPACE

class PackagingTool : public QMainWindow
{
    Q_OBJECT

public:
    PackagingTool(QWidget *parent = nullptr);
    ~PackagingTool();
    void OpenAppPath();
    void SavePath();
    void SetComboBox();
    bool CopyApp();
    int PackProcess();

private slots:
    void on_appPathPushButton_clicked();

    void on_savePathPushButton_clicked();

    void on_packPushButton_clicked();

    void on_qtPathPushButton_clicked();

private:
    Ui::PackagingTool *ui;
    QString filePathAndName; // 文件路径及文件名
    QString appPath; // 文件路径
    QString appName;
    QString savePath;
    QVector<QString> compilers; // windeployqt路径及文件名
    QProcess *p;
    QtPath qtPath;
};
