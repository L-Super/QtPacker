#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

class Config : public QObject {
    Q_OBJECT
private:
    explicit Config(QObject* parent = nullptr);
    bool ReadPath();
    bool WritePath();

public:
    static Config& instance();
    void SetQtInstallPath(const QString& path);
    void SetAppPath(const QString& path);
    void SetSavePath(const QString& path);
    void WriteConfig(const QString& key, const QString& value,const QString& group = "");
    QString GetConfig(const QString& key,const QString& group = "");
    QString GetQtInstallPath() { return qtInstallPath; }
    QString GetAppPath() { return appPath; }
    QString GetSavePath() { return savePath; }
//    QVector<QString> GetComplierList() { return complierList; }

private:
    QString qtInstallPath;
    QString appPath;
    QString savePath;
//    QVector<QString> complierList;
};

#endif  // CONFIG_H
