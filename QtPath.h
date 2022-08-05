#ifndef QTPATH_H
#define QTPATH_H

#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <unordered_map>

class QtPath : public QObject {
    Q_OBJECT
public:
    explicit QtPath(QObject* parent = nullptr);
    void SetQtPath(const QString& path);
    QString GetSelectComplierPath(const QString& str);
    QVector<QString> GetComplierResult() { return complierList; }

private:
    bool FindCompiler(const QString& path);
    QString RegexMatch(QRegExp ex, const QVector<QString>& v);

private:
    QString installPath;            //保存Qt安装路径
    QDir qPath;                     //保存Qt/5.15.2/路径
    QRegExp versionReg;             //匹配5.15.2
    QVector<QString> complierList;  //编译器列表
};

#endif  // QTPATH_H
