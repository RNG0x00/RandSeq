#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    #ifdef WIN64
        QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    #endif
    QApplication a(argc, argv);
    QSplashScreen s;
    QPixmap p(":/new/pics/pic.jpg");
    s.setPixmap(p);
    s.show();
    MainWindow w;
    w.show();
    s.hide();
    return a.exec();
}
