#pragma once

#include <QMainWindow>
#include <QProcess>
#include "QtPath.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PackagingTool; }
QT_END_NAMESPACE

class QLabel;
class QMovie;
class QVBoxLayout;
class ProgressWidget : public QWidget{
    Q_OBJECT
public:
    ProgressWidget(QWidget *parent = nullptr);
    void Start();
    void Stop();
private:
    QLabel* label;
    QMovie* movie;
    QVBoxLayout* layout;
};

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

signals:
    void pfinished();

private:
    Ui::PackagingTool *ui;
    QString filePathAndName; // 文件路径及文件名
    QString appPath; // 文件路径
    QString appName;
    QString savePath;
    QVector<QString> compilers; // windeployqt路径及文件名
    QProcess *p;
    QtPath qtPath;
    ProgressWidget* maskLayer;
};
