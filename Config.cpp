#include "Config.h"
#include <QFile>
#include <QSettings>
#include <QCoreApplication>
#include<QDebug>

Config::Config(QObject *parent)
    : QObject{parent}
{
    QString path = QCoreApplication::applicationDirPath();
    QString filePath = path + "/config.ini";
    QFile file(filePath);
    if(!file.exists())
    {
//        file.open(QIODevice::WriteOnly);
        qWarning()<<"file not exists";
        file.close();
        WriteFile();
    }

    ReadFile();
}

bool Config::ReadFile()
{
    QString path = QCoreApplication::applicationDirPath();
    QSettings settings(path + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("Path");
    qtInstallPath = settings.value("Qt Install Path").toString();
    appPath = settings.value("EXE Path").toString();
    savePath = settings.value("Save Path").toString();
    settings.endGroup();
    return true;
}

bool Config::WriteFile()
{
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    qDebug()<<path;
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Path");
    settings.setValue("Qt Install Path", qtInstallPath);
    settings.setValue("EXE Path", appPath);
    settings.setValue("Save Path", savePath);
    settings.endGroup();
    return true;
}

Config &Config::instance()
{
    static Config c;
    return c;
}

void Config::SetQtInstallPath(const QString &path)
{
    qtInstallPath = path;
    WriteFile();
}

void Config::SetAppPath(const QString &path)
{
    appPath = path;
    WriteFile();
}

void Config::SetSavePath(const QString &path)
{
    savePath = path;
    WriteFile();
}

