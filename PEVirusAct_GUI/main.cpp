#include "PEVirusAct_GUI.h"
#include <QtWidgets/QApplication>           // 包含Qt应用程序类头文件
#include <QFont>

// 强制使用UTF-8编码执行字符集（MSVC编译器专用指令）
#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


    // 创建Qt应用程序对象，管理整个应用程序的生命周期
    QApplication a(argc, argv);


    // 设置支持中文的字体
    QFont font;
    font.setFamily("Microsoft YaHei"); // 微软雅黑
    a.setFont(font);


    // 创建主窗口对象
    PEVirusAct_GUI w;

    // 显示主窗口
    w.show();

    // 进入Qt事件循环，等待用户交互
    // 当应用程序退出时，exec()返回一个整数值
    return a.exec();
}
