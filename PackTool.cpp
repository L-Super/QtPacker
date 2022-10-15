#include "PackTool.h"
#include "ui_PackTool.h"

#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QString>
#include <QRegExp>
#include <QThread>
#include <QDebug>
#include <QMovie>

#include "CmdProcess.h"
#include "Version.h"
#include "ZipTool.h"
#include "Config.h"
#include "Log.h"

PackTool::PackTool(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PackTool),
      versionLabel(new QLabel()),
      instructionLabel(new QLabel()),
      maskLayer(new ProgressWidget(this)),
      zip(new ZipTool()),
      workerThread(new QThread()),
      cmd(new CmdProcess())
{
    ui->setupUi(this);
    setWindowTitle("QtPacker");

    //状态栏添加版本信息
    versionLabel->setText(GetAppVersion());
    ui->statusbar->addPermanentWidget(versionLabel);
    instructionLabel->setText("使用说明");
    ui->statusbar->addWidget(instructionLabel);

    if (!Config::instance().GetQtInstallPath().isEmpty())
    {
        qtPath.SetQtPath(Config::instance().GetQtInstallPath());
        SetComboBox();
    }
    ui->qtPathLineEdit->setText(Config::instance().GetQtInstallPath());
    ui->appPathLineEdit->setText(Config::instance().GetAppPath());
    ui->savePathLineEdit->setText(Config::instance().GetSavePath());

    //进度遮罩层
    maskLayer->setFixedSize(this->size()); //设置窗口大小
    maskLayer->setVisible(false);
    stackUnder(maskLayer);

    instructionLabel->installEventFilter(this);

    connect(cmd, &CmdProcess::readStandardOutput, this, [=](const QByteArray &byte) {
        ui->textEdit->append(QString::fromLocal8Bit(byte));
    });
    connect(cmd, &CmdProcess::readStandardError, this, [=](const QByteArray &byte) {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(QString::fromLocal8Bit(byte));
        ui->textEdit->setTextColor(Qt::black);
    });

    zip->moveToThread(workerThread);
    //信号发射后启动线程工作
    connect(this, &PackTool::zipSignal, zip, &ZipTool::Zip);
    //该线程结束时销毁
    connect(workerThread, &QThread::finished, zip, &QObject::deleteLater);
    //线程 执行函数结束后发送信号，对结果进行处理
    connect(zip, &ZipTool::zipSuccess, this, [this]()
            {
        maskLayer->Stop();
        ui->textEdit->setTextColor(QColor("#3377FF"));
        ui->textEdit->append("制作zip压缩包成功");
        QMessageBox::information(this,"提示","制作zip压缩包成功！"); });
    connect(zip, &ZipTool::zipFailed, this, [this](const QString &errorStr)
            {
        maskLayer->Stop();
        ui->textEdit->append(errorStr);
        QMessageBox::information(this, "提示", QString("制作压缩包失败！\n%1").arg(errorStr)); });
    //创建线程（此时未启动）
    workerThread->start();
}

PackTool::~PackTool()
{
    delete ui;
    maskLayer->deleteLater();
    workerThread->quit();
    workerThread->wait();
}

void PackTool::OpenAppPath()
{
    QString str("选择待打包程序");
    filePathAndName = QFileDialog::getOpenFileName(this, str, ".", tr("*.exe"));
    //    qDebug()<< "filePathAndName " << filePathAndName;

    // 通过文件对话框打开的路径形式为“D:/Git_Project/”，故匹配“/”即可
    appName = filePathAndName.section(QRegExp("[/]"), -1);
    Config::instance().WriteConfig("app name", appName);
    //    qDebug()<< "appName " << appName;

    appPath = filePathAndName.left(filePathAndName.lastIndexOf("/"));
    Config::instance().SetAppPath(appPath);
    //    qDebug() << "filePath " << appPath;
}

void PackTool::SavePath()
{
    savePath = QFileDialog::getExistingDirectory(this, ("保存路径"), appPath);
    Config::instance().SetSavePath(savePath);
}

// 从搜索结果获取下拉框选项
void PackTool::SetComboBox()
{
    compilers = qtPath.GetComplierResult();

    for (const auto &it : compilers)
    {
        ui->comboBox->addItem(it);
    }
}

bool PackTool::CopyApp()
{
    QString decApp;
    if (appName.isEmpty())
    {
        appName = Config::instance().GetConfig("app name");
        savePath = Config::instance().GetSavePath();
        auto srcPath = Config::instance().GetAppPath();
        decApp = savePath + "/" + appName;
        filePathAndName = srcPath + "/" + appName;
        qcout << decApp << filePathAndName;
    }
    else
        decApp = savePath + "/" + appName;
    // 删除待打包路径已存在的app
    if (QFile::exists(decApp))
    {
        qcout << "exe is exist";
        if (decApp != filePathAndName)
        {
            qcout << "path is different, so delete it";
            QFile::remove(decApp);
        }
    }
    return QFile::copy(filePathAndName, decApp);
}

int PackTool::LaunchProcess()
{
    if (!CopyApp())
    {
        qcout << "maybe path is same or failed";
        //        return -1;
    }
    QString selectCompiler = ui->comboBox->currentText();
    QString envBatPath = qtPath.GetEnvBatPath(selectCompiler);
    if(envBatPath.isEmpty())
    {
        qcout<<"env bat path is empty";
        return -1;
    }
    qcout << "env bat path is" << envBatPath;

    QString packApp = savePath + "/" + appName;

    cmd->StartProcess(envBatPath, packApp);

    return 0;
}

void PackTool::SelectionAfterProcessDone()
{
    QMessageBox box(QMessageBox::Information, "提示", "软件打包成功！\n是否打开文件夹或制作压缩包？");
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Yes, QString("制作压缩包"));
    box.setButtonText(QMessageBox::Ok, QString("打开文件夹"));
    box.setButtonText(QMessageBox::Cancel, QString("关闭"));
    int button = box.exec();
    if (button == QMessageBox::Ok)
    {
        QString url = "file:///" + Config::instance().GetSavePath();
        // QUrl::TolerantMode：QUrl 将尝试纠正 URL 中的一些常见错误。这种模式对于解析来自严格符合标准的来源的 URL 非常有用。
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
    if (button == QMessageBox::Yes)
    {
        maskLayer->Start();
        ZipTool zip;
        auto appName = Config::instance().GetConfig("app name");
        auto zipName = appName.left(appName.lastIndexOf(".")) + ".zip";
        qcout << zipName;
        auto path = Config::instance().GetSavePath();
        //发射信号，开始执行线程
        emit zipSignal(zipName, path);
        //        if(zip.Zip(zipName, path) == false)
        //        {
        //            qcout<<"zip error";
        //            return;
        //        }
        //        ui->textEdit->append("制作压缩包成功");
        //        QMessageBox::information(this,"提示","制作压缩包成功！");
    }
}

//添加事件过滤器，实现状态栏的QLabel能够点击触发
bool PackTool::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == instructionLabel)
    {
        if (event->type() == QEvent::MouseButtonPress) //鼠标点击
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event); // 事件转换

            if (mouseEvent->button() == Qt::LeftButton)
            {
                QDialog dlg;
                dlg.setFixedSize(QSize(480, 200));
                auto layout = new QHBoxLayout(&dlg);
                auto label = new QLabel(&dlg);
                label->setText("<h3>使用说明</h3>"
                               "1. 选择Qt 的安装路径"
                               "<p style=\"font-size:14px\"><b>不同版本路径有区别，未识别到编译器，请尝试上下一级路径</b></p>"
                               "2. 编译器选择对应的工程所使用的<br>"
                               "3. 软件路径即生成的可执行文件路径<br>"
                               "4. 切勿保存到软件路径！选择新的存放路径<br>");
                layout->addWidget(label);
                dlg.exec();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
}

void PackTool::on_appPathPushButton_clicked()
{
    OpenAppPath();
    ui->appPathLineEdit->setText(appPath);
}

void PackTool::on_savePathPushButton_clicked()
{
    SavePath();
    ui->savePathLineEdit->setText(savePath);
}

void PackTool::on_packPushButton_clicked()
{
    if (ui->qtPathLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this, ("错误"), ("Qt路径未选择！"));
        return;
    }
    if (appPath.isEmpty() && ui->appPathLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this, ("错误"), ("软件路径未选择！"));
        return;
    }
    if (appName.isEmpty() && ui->savePathLineEdit->text().isEmpty())
    {
        QMessageBox::critical(this, ("错误"), ("打包路径未选择！"));
        return;
    }

    ui->textEdit->clear();

    ui->statusbar->showMessage("开始打包...", 1000);

    //    maskLayer->Start();

    // 调用打包程序
    if (LaunchProcess() < 0)
    {
        qcout << "PackProcess func failed";
        return;
    }

    ui->statusbar->showMessage("打包完成", 2000);

    SelectionAfterProcessDone();

    //    maskLayer->Stop();
}

void PackTool::on_qtPathPushButton_clicked()
{
    //返回桌面路径
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    //    qDebug()<<defaultPath;
    QString selectPath = QFileDialog::getExistingDirectory(this, ("选中Qt安装路径"), defaultPath);
    if (selectPath.isEmpty())
        return;
    ui->qtPathLineEdit->setText(selectPath);
    Config::instance().SetQtInstallPath(selectPath);
    qtPath.SetQtPath(selectPath);

    SetComboBox();
}

ProgressWidget::ProgressWidget(QWidget *parent) : QWidget(parent),
                                                  label(new QLabel()),
                                                  movie(new QMovie(":/images/resources/images/loading.gif")),
                                                  layout(new QVBoxLayout(this))
{
    setWindowFlags(/*Qt::CustomizeWindowHint |*/ Qt::FramelessWindowHint);
    setAutoFillBackground(true);

    auto pal = this->palette();
    pal.setColor(QPalette::Background, QColor(255, 255, 255, 0));
    setPalette(pal);

    movie->setScaledSize(QSize(50, 50));
    label->setMovie(movie);
    label->setAlignment(Qt::AlignCenter);

    label->setStyleSheet("background-color: transparent;");
    movie->start();
    layout->addWidget(label);
}

void ProgressWidget::Start()
{
    this->setVisible(true);
    this->raise();
}

void ProgressWidget::Stop()
{
    this->setVisible(false);
    this->lower();
}
