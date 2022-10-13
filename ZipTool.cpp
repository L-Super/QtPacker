#include "ZipTool.h"
#include <QMessageBox>
#include <QDir>

#include "Log.h"
#include "zip.h"

ZipTool::ZipTool(QObject *parent)
    : QObject{parent}
{

}

auto ZipTool::RecursiveFiles(const QString &folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        return -1;
    }

    //过滤文件和文件名，去掉.和..的文件夹
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    //获取文件夹和文件信息
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.size() < 1) {
        return -2;
    }

    for (const auto& it : fileList)
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

bool ZipTool::Zip(const QString &zipName, const QString &folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        QMessageBox::warning(nullptr, "Warn","选择的路径不存在");
        return false;
    }

    //分割路径
    fatherDir = dir.path().split("/").last();

    auto err = RecursiveFiles(dir.path());
    if(err < 0)
    {
        qcout<<"Recursive files failed.Error code:"<<err;
    }

    int error = 0;
    auto z = zip_open(zipName.toUtf8().constData(), ZIP_CREATE, &error);
    if ((error != ZIP_ER_OK) && (z == nullptr))
    {
        qcout<<"zip_open failed";
        return false;
    }

    for (const auto& it : qAsConst(files)) {
        zip_source* src = zip_source_file(z, it.absoluteFilePath().toUtf8(), 0, 0);
        if (src)
        {
            auto filePath = it.filePath();

            //分割出相对路径。原路径是绝对路径，若打包则会是绝对路径的文件夹结构。
            //目的：E:\Code\test\a.txt -> test\a.txt
            auto relativePath = filePath.mid(filePath.lastIndexOf(fatherDir));

            error = zip_file_add(z, relativePath.toUtf8(), src, 0);
            if (error == -1)
            {
                qcout << QString("Error adding file '%1': %2")
                    .arg(it.fileName(), zip_strerror(z));
                zip_source_free(src);
                zip_close(z);
                return false;
            }
        }
    }

    zip_close(z);
    return true;
}
