#include "SonPage.h"

QString StringTransform(std::string str,int )
{
	return 0;
}

SonPage::SonPage(QWidget *parent,QString FileName)
	: QWidget(parent)
{
	//qDebug() << FileName;
	//qDebug() << FileName.toStdString().c_str();
	HANDLE hFile = CreateFileA((LPCSTR)(FileName.toStdString().c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ppt = new PETamper(hFile);
	FilePath = FileName;
	ui.setupUi(this);
	FieldTreeInitial();
	ShowSourceHex();
	ShowTargetHex();

}

SonPage::~SonPage()
{
	delete ppt;
}

int SonPage::FieldHighlight(int position, int address, int size)
{
	QTextCursor cursor(ui.SourceTextHex->textCursor());
	QTextCharFormat format;
	cursor.setPosition(address);
	cursor.setPosition(address + size, QTextCursor::KeepAnchor);
	format.setBackground(QColor("yellow"));
	cursor.mergeCharFormat(format);
	return 0;
}

int SonPage::FieldModify(int position, int address, int size, std::string buffer)
{
	QTextCursor cursor(ui.SourceTextHex->textCursor());
	cursor.setPosition(address);
	cursor.setPosition(address + size, QTextCursor::KeepAnchor);


	return 0;
}

int SonPage::FieldTreeInitial()
{
	//
	//	由于目前C++没有原生反射机制，而搜索到实现反射的代码大概“不到一百行”，最后决定在不适用第三方库的前提下采取硬编码的方式遍历结构体，所以接下来的代码会非常丑陋
	//
	// 
	// 
	// 
	// 
	// 
	// 


	//设置头
	QStandardItemModel* qmodel = new QStandardItemModel(ui.SourceFieldTree);
	//qmodel->setHorizontalHeaderLabels(QStringList() << "名称" << "值" << "地址" << "大小");
	qmodel->setHorizontalHeaderLabels(QStringList() << "Name" << "Value" << "Address" << "Size");//输入中文有问题

	//QVector<BYTE>buffer;
	std::string buffer;

	//IMAGE_DOS_HEADER
	QList<QStandardItem*> idh_Header(4);
	idh_Header[0] = new QStandardItem("IMAGE_DOS_HEADER");
	idh_Header[1] = new QStandardItem("");
	idh_Header[2] = new QStandardItem(QString("%1").arg(0, 0, 16).toUpper() + QString("h"));
	idh_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_DOS_HEADER), 0, 16).toUpper() + QString("h"));
	qmodel->appendRow(idh_Header);

	QVector<QList<QStandardItem*>> idh_Field(IMAGE_DOS_HEADER_INFO.size());
	for (int i = 0; i < IMAGE_DOS_HEADER_INFO.size(); i++)
	{
		idh_Field[i].resize(4);
		idh_Field[i][0] = new QStandardItem(QString::fromStdString(IMAGE_DOS_HEADER_INFO[i].field));
		buffer.resize(IMAGE_DOS_HEADER_INFO[i].size);
		SetFilePointer(ppt->hFile, 0 + IMAGE_DOS_HEADER_INFO[i].offset, 0, 0);
		if (!ReadFile(ppt->hFile, buffer.data(), IMAGE_DOS_HEADER_INFO[i].size, NULL, NULL))
		{
			qDebug() << "ReadFile failed at IMAGE_DOS_HEADER ";
			qDebug() << "GetLastError: " << GetLastError();
		}
		if (idh_Field[i][0]->text() == "e_magic" || idh_Field[i][0]->text() == "e_lfanew")
			idh_Field[i][1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<int*>(buffer.data()), 0, 16).toUpper() + QString("h"));
		else
			idh_Field[i][1] = new QStandardItem(QString::number(*reinterpret_cast<int*>(buffer.data())));
		idh_Field[i][2] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].offset, 0, 16).toUpper() + QString("h"));
		idh_Field[i][3] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].size, 0, 16).toUpper() + QString("h"));

		idh_Header[0]->appendRow(idh_Field[i]);

		/*
		
		for (int j = 0; j < 4; j++)
		{
			connect(idh_Field[i][j],QStandardItem::)
		}
		
		
		*/
		
	}



	
	//IMAGE_NT_HEADERS
	QList<QStandardItem*> inh_Header(4);
	inh_Header[0] = new QStandardItem("IMAGE_NT_HEADERS");
	inh_Header[1] = new QStandardItem("");
	inh_Header[2] = new QStandardItem(QString("%1").arg(0 + sizeof(IMAGE_DOS_HEADER), 0, 16).toUpper() + QString("h"));
	inh_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_NT_HEADERS), 0, 16).toUpper() + QString("h"));
	qmodel->appendRow(inh_Header);

	QVector<QList<QStandardItem*>> inh_Field(IMAGE_DOS_HEADER_INFO.size());
	for (int i = 0; i < IMAGE_DOS_HEADER_INFO.size(); i++)
	{
		inh_Field[i].resize(4);
		inh_Field[i][0] = new QStandardItem(QString::fromStdString(IMAGE_DOS_HEADER_INFO[i].field));
		inh_Field[i][1] = new QStandardItem("");
		inh_Field[i][2] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].offset, 16).toUpper() + QString("h"));
		inh_Field[i][3] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].size, 0, 16).toUpper() + QString("h"));

		inh_Header[0]->appendRow(inh_Field[i]);
	}
	

	

	
	//设置点击后高亮显示区域

	ui.SourceFieldTree->setModel(qmodel);
	//QTreeView::
	//ui.SourceFieldTree->selectionModel();
	/*
	connect(ui.SourceFieldTree, &QTreeView::clicked, this, [=](const QModelIndex& index) {
		//QStandardItem* item = qmodel->itemFromIndex(index);
		QModelIndex tempindex = index;
		int position = 0;
		while (tempindex.isValid() && tempindex.parent().isValid())
			tempindex = tempindex.parent();
			//tempindex = index.parent();
		QString ParentName = qmodel->itemFromIndex(tempindex)->text();
		if (ParentName == "IMAGE_DOS_HEADER")
			position = 0;
		else if (ParentName == "IMAGE_NT_HEADERS")
			position = ppt->idh.e_lfanew;
		//else if()
		QTextCursor cursor(ui.SourceTextHex->textCursor());
		QTextCharFormat format;
		cursor.setPosition(position + (qmodel->itemFromIndex(index.siblingAtColumn(2)))->text().left(qmodel->itemFromIndex(index.siblingAtColumn(2))->text().size() - 1).toInt());
		cursor.setPosition(position + (qmodel->itemFromIndex(index.siblingAtColumn(2)))->text().left(qmodel->itemFromIndex(index.siblingAtColumn(2))->text().size() - 1).toInt() + (qmodel->itemFromIndex(index.siblingAtColumn(3)))->text().left(qmodel->itemFromIndex(index.siblingAtColumn(3))->text().size() - 1).toInt(), QTextCursor::KeepAnchor);
		format.setBackground(QColor("yellow"));
		cursor.mergeCharFormat(format);
		});
	
	
	*/
	

	connect(ui.SourceFieldTree->selectionModel(),&QItemSelectionModel::selectionChanged,this, [=](const QItemSelection& selected, const QItemSelection& deselected) {
		//QStandardItem* item = qmodel->itemFromIndex(index);
		Q_UNUSED(deselected);
		QModelIndex index = selected.indexes().first();
		QModelIndex tempindex = selected.indexes().first();
		int position = 0;
		while (tempindex.isValid() && tempindex.parent().isValid())
			tempindex = tempindex.parent();
		//tempindex = index.parent();
		QString ParentName = qmodel->itemFromIndex(tempindex)->text();
		if (ParentName == "IMAGE_DOS_HEADER")
			position = 0;
		else if (ParentName == "IMAGE_NT_HEADERS")
			position = ppt->idh.e_lfanew;
		//else if()
		QTextCursor cursor(ui.SourceTextHex->textCursor());
		QTextCharFormat format;
		cursor.setPosition(position + (qmodel->itemFromIndex(index.siblingAtColumn(2)))->text().left(qmodel->itemFromIndex(index.siblingAtColumn(2))->text().size() - 1).toInt());
		cursor.setPosition(position + (qmodel->itemFromIndex(index.siblingAtColumn(2)))->text().left(qmodel->itemFromIndex(index.siblingAtColumn(2))->text().size() - 1).toInt() + (qmodel->itemFromIndex(index.siblingAtColumn(3)))->text().left(qmodel->itemFromIndex(index.siblingAtColumn(3))->text().size() - 1).toInt(), QTextCursor::KeepAnchor);
		format.setBackground(QColor("yellow"));
		cursor.mergeCharFormat(format);
		});
	return 0;
}

int SonPage::ShowSourceHex()
{

	//cerr << "TESTTEST" << endl;

	/*
    // 获取原始二进制数据（假设返回的是 std::string）
    std::string binaryData = ppt->AllBin();

    // 转换为16进制字符串（每字节2位，大写，空格分隔）

	
	QByteArray byteArray(binaryData.data(), binaryData.size());


	QString hexString = byteArray.toHex(' ').toUpper();

	QString StructHexString;
	for (int i = 0; i < hexString.size(); i++)
	{
		StructHexString += hexString[i];
		if ((i + 1) % 48 == 0)
			StructHexString += '\n';

	}
	
	*/
    

    // 显示在 QTextEdit 中
	string pptAllBinString = ppt->AllBin();
	ui.SourceTextHex->setText(StdStringToStructHexQString(pptAllBinString));
	string temp;
	/*
	for (int i = 0; i < pptAllBinString.size(); i++)
	{
		if(pptAllBinString[i])
	}
	*/
	
	ui.SourceTextChar->setText(QString::fromStdString(ppt->AllBin()));

	//ui.textEdit->setText(StructHexString);
    return 0;

	/*
	//ui.textEdit->setText(QString::fromStdString(ppt->ShowSourceHex()));
	cerr << "TESTTEST" << endl;

	ui.textEdit->setText(QString::fromStdString(ppt->AllBin()));
	return 0;
	*/

}

int SonPage::ShowTargetHex()
{
	std::string temp;
	ui.TargetTextHex->append("IMAGE_DOS_HEADER");
	temp.assign(reinterpret_cast<char*>(&ppt->idh), sizeof(ppt->idh));
	ui.TargetTextHex->append(StdStringToStructHexQString(temp));
	ui.TargetTextHex->append("\n");

	ui.TargetTextHex->append("IMAGE_NT_HEADERS");
	temp.assign(reinterpret_cast<char*>(&ppt->inh), sizeof(ppt->inh));
	ui.TargetTextHex->append(StdStringToStructHexQString(temp));
	ui.TargetTextHex->append("\n");


	
	qDebug() << temp;
	//ui.textEdit_2->setText(QString::fromStdString(ppt->ShowTargetHex()));
	return 0;
}

int SonPage::MakeAssembly(HANDLE hFile)
{
	ppt->Assembly(hFile);
	return 0;
}