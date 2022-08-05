#include "PackagingTool.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile style(":/qss/resources/style.qss");
    if( style.open(QFile::ReadOnly))
    {
        qApp->setStyleSheet(style.readAll());
        style.close();
    }

    PackagingTool w;
    w.show();
    return a.exec();
}
