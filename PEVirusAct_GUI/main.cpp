#include "PEVirusAct_GUI.h"
#include <QtWidgets/QApplication>
//#include <QTextCodec>

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    PEVirusAct_GUI w;
    w.show();
    return a.exec();
}
