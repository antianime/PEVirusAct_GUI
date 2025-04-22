#include "PEVirusAct_GUI.h"
#include"SonPage.h"

// 构造函数，初始化主窗口
PEVirusAct_GUI::PEVirusAct_GUI(QWidget *parent)
    : QMainWindow(parent)           // 调用父类QMainWindow的构造函数
{
    ui.setupUi(this);               // 使用UI文件设置界面

    // 连接菜单项"OpenFile"的触发信号到槽函数OpenFileAndCreateSonPage
    connect(ui.OpenFile, &QAction::triggered, this, &PEVirusAct_GUI::OpenFileAndCreateSonPage);
	connect(ui.tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
		QWidget* widget = ui.tabWidget->widget(index);
		if (widget)
		{
			SonPage* sonPage = qobject_cast<SonPage*>(widget);
			if (sonPage)
			{
				sonPage->ppt->~PETamper();
				delete sonPage;
			}
		}
		ui.tabWidget->removeTab(index);
		});

}

// 析构函数
PEVirusAct_GUI::~PEVirusAct_GUI()
{
    for (int index = ui.tabWidget->count(); index > 0; index--)
    {
        ui.tabWidget->removeTab(index);
    }
}

// 打开文件并创建子页面的槽函数
int PEVirusAct_GUI::OpenFileAndCreateSonPage()
{
    // 打开文件对话框
    //QString temp = u8"选择文件";            // 使用u8前缀确保UTF-8编码

    QString temp = QStringLiteral("选择文件");

    // 参数说明：父窗口、对话框标题、默认路径、文件过滤器
    QString FileName = QFileDialog::getOpenFileName(this, temp, "", "All Files (*)");
    //QString FileName = QFileDialog::getOpenFileName(this, "选择文件", "", "All Files (*)");
    if (FileName.isEmpty())
        return 0;

    // 使用文件名创建子页面，this表示父窗口是当前主窗口
    SonPage* sonPage;
    try {
        sonPage = new SonPage(this, FileName);
    }
    catch (std::exception& e) {
        QMessageBox::critical(this, "错误", QString::fromStdString(e.what()));
        return 1;
    };


    // 处理文件名：从完整路径中提取文件名部分
    // 从字符串末尾向前遍历，找到最后一个'/'字符
	for (int i = FileName.size() - 1; i >= 0; i--)
	{
		if (FileName[i] == '/')
		{

            // 使用mid截取'/'之后的部分作为文件名
            FileName = FileName.mid(i + 1);
			break;
		}
	}

    // 将子页面添加到tabWidget中，使用文件名作为标签
    ui.tabWidget->addTab(sonPage, FileName);
    //qDebug() << "test";

    return 0;
}
