#include "Config.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QSettings>

Config::Config(QObject *parent)
    : QObject{parent} {
    QString path = QCoreApplication::applicationDirPath();
    QString filePath = path + "/config.ini";
    QFile file(filePath);
    if (!file.exists()) {
        //        file.open(QIODevice::WriteOnly);
        qWarning() << "file not exists";
        file.close();
        WritePath();
    }

    ReadPath();
}

bool Config::ReadPath() {
    QString path = QCoreApplication::applicationDirPath();
    QSettings settings(path + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("Path");
    qtInstallPath = settings.value("Qt Install Path").toString();
    appPath = settings.value("EXE Path").toString();
    savePath = settings.value("Save Path").toString();
    settings.endGroup();

    return true;
}

bool Config::WritePath() {
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
//    qDebug() << path;
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Path");
    settings.setValue("Qt Install Path", qtInstallPath);
    settings.setValue("EXE Path", appPath);
    settings.setValue("Save Path", savePath);
    settings.endGroup();
    return true;
}

Config &Config::instance() {
    static Config c;
    return c;
}

void Config::SetQtInstallPath(const QString &path) {
    qtInstallPath = path;
    WritePath();
}

void Config::SetAppPath(const QString &path) {
    appPath = path;
    WritePath();
}

void Config::SetSavePath(const QString &path) {
    savePath = path;
    WritePath();
}

void Config::WriteConfig(const QString &key, const QString &value,const QString &group/* ="" */)
{
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue(key,value);
    settings.endGroup();
}

QString Config::GetConfig(const QString &key,const QString &group/* ="" */)
{
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup(group);
    auto value = settings.value(key).toString();
    settings.endGroup();
    return value;
}

//void Config::SetComplierList(const QVector<QString> &list)
//{
//    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
//    QSettings settings(path, QSettings::IniFormat);
//    settings.beginGroup("ComplierList");
//    QString str("complier");
//    size_t i = 0;
//    for(auto it = list.begin(); it!=list.end(); it++)
//    {
//        settings.setValue(str.arg(i), *it);
//        i++;
//    }
//    settings.endGroup();
//}
