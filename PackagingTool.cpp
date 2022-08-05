#include "PackagingTool.h"
#include "ui_PackagingTool.h"
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QRegExp>
#include <iostream>
#include <iterator>
#include "Config.h"

// 宏定义打印log支持打印文件名-所在行号
#define qcout qDebug()<<"["<<__FILE__<<":"<<__LINE__<<"]"

//TODO:状态栏添加使用说明
//样式优化，按钮悬浮效果，字体大小等
PackagingTool::PackagingTool(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PackagingTool),
      p(new QProcess())
{
    ui->setupUi(this);
    setWindowTitle("QtPacker");
    ui->textEdit->setReadOnly(1);

    ui->qtPathLineEdit->setText(Config::instance().GetQtInstallPath());
    ui->appPathLineEdit->setText(Config::instance().GetAppPath());
    ui->savePathLineEdit->setText(Config::instance().GetSavePath());

    //上面的代码会触发此信号,影响combox
//    connect(ui->qtPathLineEdit,&QLineEdit::editingFinished,this,[this](){
//        QString qPath = QDir::toNativeSeparators(ui->qtPathLineEdit->text());
//        qtPath.SetQtPath(qPath);
//        Config::instance().SetQtInstallPath(qPath);
//        SetComboBox();
//    });
}

PackagingTool::~PackagingTool()
{
    delete ui;
}

void PackagingTool::OpenAppPath()
{
    QString str("选择待打包程序");
    filePathAndName = QFileDialog::getOpenFileName(this, str, ".", tr("*.exe"));
//    qDebug()<< "filePathAndName " << filePathAndName;

    // 通过文件对话框打开的路径形式为“D:/Git_Project/”，故匹配“/”即可
    appName = filePathAndName.section(QRegExp("[/]"),-1);
//    qDebug()<< "appName " << appName;

    appPath = filePathAndName.left(filePathAndName.lastIndexOf("/"));
    Config::instance().SetAppPath(appPath);
//    qDebug() << "filePath " << appPath;
}

void PackagingTool::SavePath()
{
    savePath = QFileDialog::getExistingDirectory(this,("保存路径"),appPath);
    Config::instance().SetSavePath(savePath);
}

// 从搜索结果获取下拉框选项
void PackagingTool::SetComboBox()
{
    auto compilerPath = qtPath.GetComplierResult();

    for(const auto& it : compilerPath)
    {
        ui->comboBox->addItem(it);
    }
}

bool PackagingTool::CopyApp()
{
    auto decApp = savePath + "/" + appName;
    // 删除待打包路径已存在的app
    if(QFile::exists(decApp))
        QFile::remove(decApp);
    return QFile::copy(filePathAndName, decApp);
}

int PackagingTool::PackProcess()
{
    if(!CopyApp())
        return -1;

    QString selectCompiler = ui->comboBox->currentText();
    QString programPath = qtPath.GetSelectComplierPath(selectCompiler);
    qcout<<"programPath "<<programPath;

    QStringList arguments;
//    arguments << "--dir"
//              << savePath
//              << copiedAppName;

    QString packedApp = savePath + "/" + appName;
    qcout<<packedApp;
    arguments << packedApp;

    p->start(programPath,arguments);

    // QProcess输出信息
    connect(p, &QProcess::readyReadStandardOutput, this, [=]{
        auto output = p->readAllStandardOutput();
        ui->textEdit->clear();
        ui->textEdit->append(output);
    });
    connect(p, &QProcess::readyReadStandardError, this, [=]{
        auto output = p->readAllStandardError();
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(output);
    });

    if (!p->waitForFinished())
    {
        qcout << "Package failed:" << p->errorString();
        QMessageBox::critical(this, ("错误"), ("编译器选择错误！"));
    }

    return 0;
}

void PackagingTool::on_appPathPushButton_clicked()
{
    OpenAppPath();
    ui->appPathLineEdit->setText(appPath);
}

void PackagingTool::on_savePathPushButton_clicked()
{
    SavePath();
    ui->savePathLineEdit->setText(savePath);
}

void PackagingTool::on_packPushButton_clicked()
{
    if(appPath.isEmpty() && ui->appPathLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this, ("错误"), ("软件路径未选择！"));
        return;
    }
    if(appName.isEmpty() && ui->savePathLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this, ("错误"), ("打包路径未选择！"));
        return;
    }

    // 调用打包程序
    if(PackProcess() < 0)
    {
        qDebug()<<"PackProcess func failed";
        return;
    }
    QMessageBox::information(this,("提示"),("软件打包成功！"),QMessageBox::Ok);
}

void PackagingTool::on_qtPathPushButton_clicked()
{
    //返回桌面路径
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
//    qDebug()<<defaultPath;
    QString selectPath = QFileDialog::getExistingDirectory(this, ("选中Qt安装路径"), defaultPath);
    if(selectPath.isEmpty())
        return;
    ui->qtPathLineEdit->setText(selectPath);
    Config::instance().SetQtInstallPath(selectPath);
    qtPath.SetQtPath(selectPath);

    SetComboBox();
}

