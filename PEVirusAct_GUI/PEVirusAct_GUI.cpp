#include "PEVirusAct_GUI.h"
#include"SonPage.h"

// ���캯������ʼ��������
PEVirusAct_GUI::PEVirusAct_GUI(QWidget *parent)
    : QMainWindow(parent)           // ���ø���QMainWindow�Ĺ��캯��
{
    ui.setupUi(this);               // ʹ��UI�ļ����ý���

    // ���Ӳ˵���"OpenFile"�Ĵ����źŵ��ۺ���OpenFileAndCreateSonPage
    connect(ui.OpenFile, &QAction::triggered, this, &PEVirusAct_GUI::OpenFileAndCreateSonPage);

}

// ��������
PEVirusAct_GUI::~PEVirusAct_GUI()
{}

// ���ļ���������ҳ��Ĳۺ���
int PEVirusAct_GUI::OpenFileAndCreateSonPage()
{
    // ���ļ��Ի���
    //QString temp = u8"ѡ���ļ�";            // ʹ��u8ǰ׺ȷ��UTF-8����

    QString temp = QStringLiteral("ѡ���ļ�");

    // ����˵���������ڡ��Ի�����⡢Ĭ��·�����ļ�������
    QString FileName = QFileDialog::getOpenFileName(this, temp, "", "All Files (*)");
    //QString FileName = QFileDialog::getOpenFileName(this, "ѡ���ļ�", "", "All Files (*)");

    // ʹ���ļ���������ҳ�棬this��ʾ�������ǵ�ǰ������
    SonPage* sonPage = new SonPage(this, FileName);


    // �����ļ�����������·������ȡ�ļ�������
    // ���ַ���ĩβ��ǰ�������ҵ����һ��'/'�ַ�
	for (int i = FileName.size() - 1; i >= 0; i--)
	{
		if (FileName[i] == '/')
		{

            // ʹ��mid��ȡ'/'֮��Ĳ�����Ϊ�ļ���
            FileName = FileName.mid(i + 1);
			break;
		}
	}

    // ����ҳ����ӵ�tabWidget�У�ʹ���ļ�����Ϊ��ǩ
    ui.tabWidget->addTab(sonPage, FileName);
    //qDebug() << "test";

    return 0;
}
