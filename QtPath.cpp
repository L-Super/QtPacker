#include "QtPath.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include "Log.h"

QtPath::QtPath(QObject *parent)
    : QObject{parent},
      versionReg(R"(\d{1}.\d{1,}.[0-9])")
{
}

void QtPath::SetQtPath(const QString &path)
{
    FindCompiler(std::forward<const QString &>(path));
}

//path = D:\Qt
//D:\Qt\5.15.2\mingw81_64
bool QtPath::FindCompiler(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
        qcout << "dir Cannot find the directory";

    qPath = QDir(path);
    //    qPath.setPath(path);
    if (!qPath.exists())
        qcout << "Cannot find the directory";

    installPath = path;

    qPath.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    auto vDirList = qPath.entryList().toVector();

    //匹配5.15.2文件夹
    QString result = RegexMatch(versionReg, vDirList);

    if (result.isEmpty())
    {
        qWarning("Cannot find the qt directory");
        QMessageBox::critical(nullptr, "", "请检查选择的Qt 安装路径是否正确");
        return false;
    }

    //进入后即所含编译器的文件夹，D:\Qt\5.15.2
    qPath.cd(result);
    complierList = qPath.entryList().toVector();
    //    for(auto it:complierList)
    //    {
    //        qcout<<it;
    //    }

    return true;
}

// 通过正则表达式匹配编译器
QString QtPath::RegexMatch(QRegExp ex, const QVector<QString> &v)
{
    if (!ex.isValid() || v.isEmpty())
        return QString();
    for (auto it : v)
    {
        bool isMatched = ex.exactMatch(it);
        if (isMatched)
            return it;
    }
    return QString();
}

QString QtPath::GetEnvBatPath(const QString &str)
{
    QDir dir(qPath.path());
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    qcout << "选择的路径及编译器：" << dir.path() << str;
    dir.cd(str);
    if(!dir.cd("bin"))
    {
        qcout<<"cd bin folder failed";
        return QString();
    }
    auto list = dir.entryInfoList();
    for (auto it = list.begin(); it != list.end(); it++)
    {
        if (it->baseName() == "qtenv2")
            return it->absoluteFilePath();
    }
    return QString();
}
