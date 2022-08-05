#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

class Config : public QObject
{
    Q_OBJECT
private:
    explicit Config(QObject *parent = nullptr);
    bool ReadFile();
    bool WriteFile();
public:
    static Config& instance();
    void SetQtInstallPath(const QString& path);
    void SetAppPath(const QString& path);
    void SetSavePath(const QString& path);
    QString GetQtInstallPath() {return qtInstallPath;}
    QString GetAppPath() {return appPath;}
    QString GetSavePath() {return savePath;}
private:
    QString qtInstallPath;
    QString appPath;
    QString savePath;
};

#endif // CONFIG_H
