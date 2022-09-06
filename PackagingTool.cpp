#include "PackagingTool.h"
#include "ui_PackagingTool.h"

#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QMovie>

#include "Config.h"
#include "Version.h"

// 宏定义打印log支持打印文件名-所在行号
#define qcout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

// TODO:
//添加进度条显示
//遮罩层失败，原因都属于gui，不能多线程，也会阻塞
//后期换成进度条对话框

PackagingTool::PackagingTool(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::PackagingTool),
      versionLabel(new QLabel()),
      instructionLabel(new QLabel()),
      p(new QProcess()),
      maskLayer(new ProgressWidget(this))
{
    ui->setupUi(this);
    setWindowTitle("QtPacker");

    //状态栏添加版本信息
    versionLabel->setText(GetAppVersion());
    ui->statusbar->addPermanentWidget(versionLabel);
    instructionLabel->setText("使用说明");
    ui->statusbar->addWidget(instructionLabel);

    if(!Config::instance().GetQtInstallPath().isEmpty())
    {
        qtPath.SetQtPath(Config::instance().GetQtInstallPath());
        SetComboBox();
    }
    ui->qtPathLineEdit->setText(Config::instance().GetQtInstallPath());
    ui->appPathLineEdit->setText(Config::instance().GetAppPath());
    ui->savePathLineEdit->setText(Config::instance().GetSavePath());

//    //进度遮罩层
//    maskLayer->setFixedSize(this->size());//设置窗口大小
//    maskLayer->setVisible(false);
//    stackUnder(maskLayer);

    instructionLabel->installEventFilter(this);
    // QProcess连接信号输出信息
    //2022.08.07 之前放在槽函数中关联，导致每次点击，就会关联一次，造成点击一次会执行n+1次
    connect(p, &QProcess::readyReadStandardOutput, this, [=] {
        auto output = p->readAllStandardOutput();
        ui->textEdit->clear();
        qcout<<"clear";
        ui->textEdit->append(output);
    });
    connect(p, &QProcess::readyReadStandardError, this, [=] {
        auto output = p->readAllStandardError();
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append(output);
    });
}

PackagingTool::~PackagingTool() {
    delete ui;
    maskLayer->deleteLater();
}

void PackagingTool::OpenAppPath() {
    QString str("选择待打包程序");
    filePathAndName = QFileDialog::getOpenFileName(this, str, ".", tr("*.exe"));
    //    qDebug()<< "filePathAndName " << filePathAndName;

    // 通过文件对话框打开的路径形式为“D:/Git_Project/”，故匹配“/”即可
    appName = filePathAndName.section(QRegExp("[/]"), -1);
    Config::instance().WriteConfig("app name",appName);
    //    qDebug()<< "appName " << appName;

    appPath = filePathAndName.left(filePathAndName.lastIndexOf("/"));
    Config::instance().SetAppPath(appPath);
    //    qDebug() << "filePath " << appPath;
}

void PackagingTool::SavePath() {
    savePath = QFileDialog::getExistingDirectory(this, ("保存路径"), appPath);
    Config::instance().SetSavePath(savePath);
}

// 从搜索结果获取下拉框选项
void PackagingTool::SetComboBox() {
    compilers = qtPath.GetComplierResult();

    for (const auto& it : compilers) {
        ui->comboBox->addItem(it);
    }
}

bool PackagingTool::CopyApp() {
    QString decApp;
    if(appName.isEmpty())
    {
        appName = Config::instance().GetConfig("app name");
        savePath = Config::instance().GetSavePath();
        auto srcPath = Config::instance().GetAppPath();
        decApp = savePath + "/" + appName;
        filePathAndName = srcPath + "/" + appName;
        qcout<<decApp<<filePathAndName;
    }
    else
        decApp = savePath + "/" + appName;
    // 删除待打包路径已存在的app
    if (QFile::exists(decApp))
    {
        qcout<<"exe is exist";
        if(decApp != filePathAndName)
        {
            qcout<<"path is different, so delete it";
            QFile::remove(decApp);
        }
    }
    return QFile::copy(filePathAndName, decApp);
}

int PackagingTool::PackProcess() {
    if (!CopyApp())
    {
        qcout<<"maybe path is same or failed";
//        return -1;
    }
    QString selectCompiler = ui->comboBox->currentText();
    QString programPath = qtPath.GetSelectComplierPath(selectCompiler);
    qcout << "windeployqt.exe path is" << programPath;

    QStringList arguments;
    //    arguments << "--dir"
    //              << savePath
    //              << copiedAppName;

    QString packedApp = savePath + "/" + appName;
    qcout << "打包路径：" << packedApp;
    arguments << packedApp;

    p->start(programPath, arguments);
//    p->startDetached(programPath, arguments);

    if (!p->waitForFinished()) {
        qcout << "Package failed:" << p->errorString();
        QMessageBox::critical(this, ("错误"), ("编译器选择错误！"));
    }

//    maskLayer->Stop();
    return 0;
}

//添加事件过滤器，实现状态栏的QLabel能够点击触发
bool PackagingTool::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == instructionLabel)
    {
        if (event->type() == QEvent::MouseButtonPress) //鼠标点击
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换

            if(mouseEvent->button() == Qt::LeftButton)
            {
                QDialog dlg;
                dlg.setFixedSize(QSize(480,200));
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

void PackagingTool::on_appPathPushButton_clicked() {
    OpenAppPath();
    ui->appPathLineEdit->setText(appPath);
}

void PackagingTool::on_savePathPushButton_clicked() {
    SavePath();
    ui->savePathLineEdit->setText(savePath);
}

void PackagingTool::on_packPushButton_clicked() {
    if (appPath.isEmpty() && ui->appPathLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, ("错误"), ("软件路径未选择！"));
        return;
    }
    if (appName.isEmpty() && ui->savePathLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, ("错误"), ("打包路径未选择！"));
        return;
    }

    ui->statusbar->showMessage("开始打包...", 1000);
//    _sleep(5000);
//    maskLayer->Start();
//    return;

    // 调用打包程序
    if (PackProcess() < 0) {
        qcout << "PackProcess func failed";
        return;
    }
    else
    {
        ui->statusbar->showMessage("打包完成", 2000);
        QMessageBox box(QMessageBox::Information, "提示", "软件打包成功！\n是否打开文件夹？");
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok, QString("是"));
        box.setButtonText(QMessageBox::Cancel, QString("否"));
        int button = box.exec();
        if (button == QMessageBox::Ok)
        {
            QString url = "file:///" + Config::instance().GetSavePath();
            QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));//QUrl::TolerantMode：QUrl 将尝试纠正 URL 中的一些常见错误。这种模式对于解析来自严格符合标准的来源的 URL 非常有用。
        }
    }
}

void PackagingTool::on_qtPathPushButton_clicked() {
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

ProgressWidget::ProgressWidget(QWidget* parent):QWidget(parent),
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
    qcout<<"start";
}

void ProgressWidget::Stop()
{
    this->setVisible(false);
    this->lower();
}

