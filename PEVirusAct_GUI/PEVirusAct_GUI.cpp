#include "PEVirusAct_GUI.h"

PEVirusAct_GUI::PEVirusAct_GUI(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.OpenFile, &QAction::triggered, this, &PEVirusAct_GUI::OpenFileAndInitial);
}

PEVirusAct_GUI::~PEVirusAct_GUI()
{}

int PEVirusAct_GUI::OpenFileAndInitial()
{
    //Open File
    QString temp = u8"选择文件";
    //QString temp = QString::fromUtf8("选择文件");
    QString FileName = QFileDialog::getOpenFileName(this, temp, "", "All Files (*)");
    //QString FileName = QFileDialog::getOpenFileName(this, "选择文件", "", "All Files (*)");
    //ui.TestLine->setText(FileName);

    //std::cout << FileName.toStdString() << std::endl;

    //initial : Read File

    PETamper pt(FileName.toStdString());
    //temp.fromLocal8Bit(reinterpret_cast<const char*>(&pt.idh.e_magic), 2);
    //ui.TestBlock->setText(QString::number(pt.idh.e_magic));
    //ui.TestLine->setText(temp);
    qDebug() << "test";

    return 0;
}

int PEVirusAct_GUI::MakeAssembly(HANDLE hFile)
{
    pt.Assembly(hFile);
    return 0;
}