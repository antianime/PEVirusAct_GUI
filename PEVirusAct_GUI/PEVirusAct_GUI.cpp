#include "PEVirusAct_GUI.h"
#include"SonPage.h"

PEVirusAct_GUI::PEVirusAct_GUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.OpenFile, &QAction::triggered, this, &PEVirusAct_GUI::OpenFileAndCreateSonPage);

}

PEVirusAct_GUI::~PEVirusAct_GUI()
{}

int PEVirusAct_GUI::OpenFileAndCreateSonPage()
{
    //Open File
    QString temp = u8"选择文件";
    QString FileName = QFileDialog::getOpenFileName(this, temp, "", "All Files (*)");
    //QString FileName = QFileDialog::getOpenFileName(this, "选择文件", "", "All Files (*)");

    SonPage* sonPage = new SonPage(this, FileName);

	for (int i = FileName.size() - 1; i >= 0; i--)
	{
		if (FileName[i] == '/')
		{
            FileName = FileName.mid(i + 1);
			break;
		}
	}
    ui.tabWidget->addTab(sonPage, FileName);
    //qDebug() << "test";

    return 0;
}
