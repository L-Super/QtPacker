#pragma once
#include <QDebug>
// 宏定义打印log支持打印文件名-所在行号
#define qcout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"
