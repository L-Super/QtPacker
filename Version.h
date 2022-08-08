#ifndef VERSION_H
#define VERSION_H
//Mingw编译器下，无法读取Version.h头文件数据
//#define FILE_VERSION           1,3,2,0   // 文件版本
//#define FILE_VERSION_STR       "1.3.2.0"
//#define PRODUCT_VERSION        1,3,2,0   // 产品版本
//#define PRODUCT_VERSION_STR    "1.3.2.0"
//#define COMPANY_NAME           "LIM"
//#define FILE_DESCRIPTION       "QtPacker"  // 文件说明
//#define LEGAL_COPYRIGHT        "Copyright(C) 2021-2077 LIM Ltd. All rights reserved." // 版权
//#define ORIGINAL_FILE_NAME     "QtPacker"    // 原始文件名
//#define PRODUCT_NAME           "QtPacker"        // 产品名称

#include <QCoreApplication>
QString  GetAppVersion()
{
    return QCoreApplication::applicationVersion();
}
#endif // VERSION_H