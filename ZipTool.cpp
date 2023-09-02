#include "ZipTool.h"
#include <QMessageBox>
#include <QDir>
#include <QtGui/private/qzipwriter_p.h>

#include "Log.h"

ZipTool::ZipTool(QObject *parent)
    : QObject{parent}
{
}

auto ZipTool::RecursiveFiles(const QString &folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists())
    {
        return -1;
    }

    //过滤文件和文件名，去掉.和..的文件夹
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    //获取文件夹和文件信息
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() < 1)
    {
        return -2;
    }

    for (const auto &it : fileList)
    {
        //如果是文件夹就继续递归
        if (it.isDir())
        {
            RecursiveFiles(it.filePath());
        }
        else
        {
            files.push_back(it);
        }
    }
    return 0;
}

bool ZipTool::Zip(const QString &zipName, const QString &saveFolderPath)
{
    QDir dir(saveFolderPath);
    if (!dir.exists())
    {
        QMessageBox::warning(nullptr, "Warn", "选择的路径不存在");
        emit zipFailed("选择的路径不存在");
        return false;
    }

    // 分割路径
    // fixed 截取错误问题
    // "E:/Code/qt-packer/Debug/1/libgcc_s_seh-1.dll"
    // 原先截取错误，结果成了1.dll
    fatherDir = "/" + dir.path().split("/").last() + "/";

    files.clear();
    auto err = RecursiveFiles(dir.path());
    if (err < 0)
    {
        qcout << "Recursive files failed.Error code:" << err;
    }

    qcout<<"zipName"<<zipName<<"folderPath"<<saveFolderPath;
    StartZip(zipName, saveFolderPath);

    emit zipSuccess();
    return true;
}

void ZipTool::StartZip(const QString &zipName, const QString &sourceSavePath)
{
    auto zipPath = sourceSavePath + "/" + zipName;
    qcout<<zipPath;

    QZipWriter *writer = new QZipWriter(zipPath);

    for (const auto &it : qAsConst(files))
    {
        auto filePath = it.filePath();
        if(filePath == zipPath)
            continue;

        // 分割出相对路径。原路径是绝对路径，若打包则会是绝对路径的文件夹结构。
        // 目的：E:\Code\test\a.txt -> test\a.txt
        auto relativePath = filePath.mid(filePath.lastIndexOf(fatherDir));
        qcout<<"relative path"<<relativePath;

        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        writer->addFile(relativePath, file.readAll());
        file.close();
    }

    writer->close();
}
