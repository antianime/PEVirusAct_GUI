#include "PEVirusAct_GUI.h"
#include <QtWidgets/QApplication>           // ����QtӦ�ó�����ͷ�ļ�
#include <QFont>

// ǿ��ʹ��UTF-8����ִ���ַ�����MSVC������ר��ָ�
#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


    // ����QtӦ�ó�����󣬹�������Ӧ�ó������������
    QApplication a(argc, argv);


    // ����֧�����ĵ�����
    QFont font;
    font.setFamily("Microsoft YaHei"); // ΢���ź�
    a.setFont(font);


    // ���������ڶ���
    PEVirusAct_GUI w;

    // ��ʾ������
    w.show();

    // ����Qt�¼�ѭ�����ȴ��û�����
    // ��Ӧ�ó����˳�ʱ��exec()����һ������ֵ
    return a.exec();
}
