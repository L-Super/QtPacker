#ifndef ZIPTOOL_H
#define ZIPTOOL_H

#include <QObject>
#include <QFileInfo>

class ZipTool : public QObject
{
    Q_OBJECT
public:
    explicit ZipTool(QObject *parent = nullptr);
    /**
     * @brief RecursiveFiles 递归遍历文件夹下的文件，包含子文件夹
     * @param folderPath 文件夹路径
     */
    auto RecursiveFiles(const QString& folderPath);
    /**
     * @brief Zip 制作为压缩包
     * @param zipName 压缩包名字，格式为：name.zip /name.7z其他格式未测试
     * @param folderPath 文件夹路径
     * @return
     */
    bool Zip(const QString& zipName, const QString& folderPath);

private:
    QVector<QFileInfo> files;//保存待压缩文件夹里的所有文件信息
    QString fatherDir;//记录压缩包的根文件夹
};

#endif // ZIPTOOL_H
