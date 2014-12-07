#include "music.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Music M;
//    QDesktopWidget *desktop=QApplication::desktop();

//    int w=desktop->width();//获取桌面宽度
//    int h=desktop->height();//获取桌面高度

//    M.move((w-M.width())/2,(h-M.height())/2);//居中显示
    M.show();
    a.setWindowIcon(QIcon("icon.ico"));

    return a.exec();
}
