#include "SonPage.h"

// DOS头字段信息（名称、大小、偏移量)
const std::vector< FieldInfo>IMAGE_DOS_HEADER_INFO = {
	{"e_magic",2, 0x0},
	{"e_cblp",2,0x2} ,                 // Bytes on last page of file
	{ "e_cp",2, 0x4},                       // Pages in file
	{ "e_crlc", 2,0x6},                      // Relocations
	{ "e_cparhdr", 2,0x8},                  // Size of header in paragraphs
	{ "e_minalloc",2, 0xA},                  // Minimum extra paragraphs needed
	{ "e_maxalloc",2,0xC },                  // Maximum extra paragraphs needed
	{ "e_ss",2, 0xE},                       // Initial (relative) SS value
	{ "e_sp",2, 0x10},                        // Initial SP value
	{ "e_csum",2, 0x12},                     // Checksum
	{ "e_ip",2,0x14 },                      // Initial IP value
	{ "e_cs",2, 0x16},                     // Initial (relative) CS value
	{ "e_lfarlc",2,0x18 },                  // File address of relocation table
	{ "e_ovno",2,0x1A },                    // Overlay number
	{ "e_res", 8,0x1C},                // Reserved words
	{ "e_oemid",2,0x24 },                 // OEM identifier (for e_oeminfo)
	{ "e_oeminfo",2, 0x26},                // OEM information; e_oemid specific
	{ "e_res2",20,0x28 },               // Reserved words
	{ "e_lfanew",4,0x3C }
};

// NT头字段信息
const std::vector<FieldInfo>Signature;
const std::vector<FieldInfo>IMAGE_FILE_HEADER_INFO;
const std::vector<FieldInfo>IMAGE_OPTIONAL_HEADER_INFO;

const std::vector<std::vector< FieldInfo>>IMAGE_NT_HEADERS_INFO = { Signature,IMAGE_FILE_HEADER_INFO,IMAGE_OPTIONAL_HEADER_INFO };


const std::vector< FieldInfo> IMAGE_SECTION_HEADERS_INFO = {
	{"Name",8,},
	{""},
};





// 添加行同步功能
int SonPage::syncLineScrolling()
{
    // 计算16进制窗口每行高度
    int hexLineHeight = ui.SourceTextHex->fontMetrics().lineSpacing();
    
    // 计算字符窗口每行高度
    int charLineHeight = ui.SourceTextChar->fontMetrics().lineSpacing();
    
    // 16进制窗口滚动时
	connect(ui.SourceTextHex->verticalScrollBar(), &QScrollBar::valueChanged, this,
		[=](int value) {
			// 计算对应的行号
			int lineNumber = value / hexLineHeight;
			// 设置字符窗口的滚动位置
			ui.SourceTextChar->verticalScrollBar()->setValue(lineNumber * charLineHeight);
		});
    
    // 字符窗口滚动时
	connect(ui.SourceTextChar->verticalScrollBar(), &QScrollBar::valueChanged,
		[=](int value) {
			// 计算对应的行号
			int lineNumber = value / charLineHeight;
			// 设置16进制窗口的滚动位置
			ui.SourceTextHex->verticalScrollBar()->setValue(lineNumber * hexLineHeight);
		});

	return 0;
}




QString StringTransform(std::string str,int )
{
	return 0;
}

QString StdStringToStructHexQString(std::string str)
{
	QByteArray byteArray(str.data(), str.size());


	QString hexString = byteArray.toHex(' ').toUpper();

	QString StructHexString;
	for (int i = 0; i < hexString.size(); i++)
	{
		//StructHexString += hexString[i];
		if ((i + 1) % 48 == 0)
			StructHexString += '\n';
		else
			StructHexString += hexString[i];

	}
	return StructHexString;
}

int SonPage::ScrollInitial()
{
	connect(ui.SourceTextHex->verticalScrollBar(), &QScrollBar::valueChanged, ui.SourceTextChar->verticalScrollBar(), &QScrollBar::setValue);
	connect(ui.SourceTextChar->verticalScrollBar(), &QScrollBar::valueChanged, ui.SourceTextHex->verticalScrollBar(), &QScrollBar::setValue);

	connect(ui.TargetTextHex->verticalScrollBar(), &QScrollBar::valueChanged, ui.TargetTextChar->verticalScrollBar(), &QScrollBar::setValue);
	connect(ui.TargetTextChar->verticalScrollBar(), &QScrollBar::valueChanged, ui.TargetTextHex->verticalScrollBar(), &QScrollBar::setValue);
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
	ShowSource();
	ShowTarget();
	ScrollInitial();
	//SlotsInitial();


	// 确保两个窗口使用相同的字体和行高
    //Font monoFont("Courier New", 10);
    //ui.SourceTextHex->setFont(monoFont);
    //ui.SourceTextChar->setFont(monoFont);


}

SonPage::~SonPage()
{
	delete ppt;
}

int SonPage::FieldModify(int position, int address, int size, std::string buffer)
{
	if (buffer.size() > size)
	{
		qDebug() << "Buffer size is larger than the specified size.";
		return 1;
	}

	buffer.resize(size);
	
	QTextCursor cursor(ui.SourceTextHex->textCursor());
	cursor.setPosition(address);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, size);
	cursor.insertText(QString::fromStdString(buffer));
	//cursor.selectedText();


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
		/*
		if (idh_Field[i][0]->text() == "e_magic" || idh_Field[i][0]->text() == "e_lfanew")
			idh_Field[i][1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<int*>(buffer.data()), 0, 16).toUpper() + QString("h"));
		else
			idh_Field[i][1] = new QStandardItem(QString::number(*reinterpret_cast<int*>(buffer.data())));
		*/
		idh_Field[i][1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<int*>(buffer.data()), 0, 16).toUpper() + QString("h"));
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
		inh_Field[i][2] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].offset, 0, 16).toUpper() + QString("h"));
		inh_Field[i][3] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].size, 0, 16).toUpper() + QString("h"));

		inh_Header[0]->appendRow(inh_Field[i]);
	}
	

	

	
	

	ui.SourceFieldTree->setModel(qmodel);
	
	//设置点击后高亮显示区域
	//
	//性能需要优化

	connect(ui.SourceFieldTree->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection& selected, const QItemSelection& deselected) {
		Q_UNUSED(selected);
		Q_UNUSED(deselected);

		QTextCursor HexCursor(ui.SourceTextHex->textCursor());
		QTextCursor CharCursor(ui.SourceTextChar->textCursor());
		QTextCharFormat format;
		QModelIndexList IndexList = ui.SourceFieldTree->selectionModel()->selectedIndexes();

		format.setBackground(QColor("white"));
		HexCursor.select(QTextCursor::Document);
		HexCursor.mergeCharFormat(format);
		CharCursor.select(QTextCursor::Document);
		CharCursor.mergeCharFormat(format);


		if (!IndexList.isEmpty())
		{
			//HexText
			QModelIndex index = IndexList.first();
			QModelIndex tempindex = IndexList.first();
			int position = 0;
			while (tempindex.isValid() && tempindex.parent().isValid())
				tempindex = tempindex.parent();
			QString ParentName = qmodel->itemFromIndex(tempindex)->text();
			if (ParentName == "IMAGE_DOS_HEADER")
				position = 0;
			else if (ParentName == "IMAGE_NT_HEADERS")
				position = ppt->idh.e_lfanew;
			//else if()
			QString temp;
			temp = qmodel->itemFromIndex(index.siblingAtColumn(2))->text();
			int StartPos = position + temp.left(temp.size() - 1).toInt(NULL, 16);
			temp = qmodel->itemFromIndex(index.siblingAtColumn(3))->text();
			int Length = temp.left(temp.size() - 1).toInt(NULL, 16);

			HexCursor.setPosition(3 * StartPos);
			HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 3 * Length - 1);
			format.setBackground(QColor("yellow"));
			HexCursor.mergeCharFormat(format);

			//CharText

			CharCursor.setPosition(StartPos);
			CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, Length);
			//CharCursor.setPosition(StartPos + Length, QTextCursor::KeepAnchor);
			format.setBackground(QColor("yellow"));
			CharCursor.mergeCharFormat(format);

		}
		});

	//字段修改，这个也要优化
	connect(qmodel, &QStandardItemModel::dataChanged, this, [=](const QModelIndex& index, const QModelIndex&) {

		QTextCursor HexCursor(ui.SourceTextHex->textCursor());
		QTextCursor CharCursor(ui.SourceTextChar->textCursor());
		QTextCharFormat format;

		QModelIndex tempindex = index;
		QString temp;
		int position = 0;

		temp = qmodel->itemFromIndex(index.siblingAtColumn(3))->text();
		int Length = temp.left(temp.size() - 1).toInt(NULL, 16);
		

		temp = qmodel->itemFromIndex(index)->text();
		string tempbuffer = "";
		if (temp[temp.size() - 1] == 'h')
		{
			temp = temp.left(temp.size() - 1);
			if (temp.size() % 2 == 1)
				temp = "0" + temp;
			tempbuffer.resize(temp.size() / 2);
			for (int i = 0; i < temp.size() / 2; i++)
			{
				if (!(temp[i] > '0' && temp[i] < '9') && !(temp[i] > 'A' && temp[i] < 'F') && !(temp[i] > 'a' && temp[i] < 'f'))
				{
					qDebug() << "invalid char";
					return;
				}
				tempbuffer[i] = (char)(temp.mid(i, 2).toInt(NULL, 16));
			}
			//string buffer[];

		}
		else
		{

			for (int i = 0; i < temp.size(); i++)
			{
				if (!(temp[i] > '0' && temp[i] < '9'))
				{
					qDebug() << "invalid char";
					return;
				}

			}
			unsigned long long ull = temp.toULongLong();
			for (int i = 0; ull > 0; i++)
			{
				tempbuffer += (unsigned char)(ull & 0xFF);
				ull >>= 8;
			}
			
		}

		if (Length < tempbuffer.size())
		{
			qDebug() << "out of size";
			return;
		}
		//buffer.resize(Length);
		temp = qmodel->itemFromIndex(index.siblingAtColumn(2))->text();
		int StartPos = position + temp.left(temp.size() - 1).toInt(NULL, 16);

		while (tempindex.isValid() && tempindex.parent().isValid())
			tempindex = tempindex.parent();
		QString ParentName = qmodel->itemFromIndex(tempindex)->text();
		if (ParentName == "IMAGE_DOS_HEADER")
		{
			position = 0;
			ppt->FieldTamper(&ppt->idh, temp.toInt(NULL, 16), (char*)tempbuffer.c_str());
		}
		else if (ParentName == "IMAGE_NT_HEADERS")
		{
			position = ppt->idh.e_lfanew;
			ppt->FieldTamper(&ppt->inh, temp.toInt(NULL, 16), (char*)tempbuffer.c_str());
		}
		

		HexCursor.setPosition(3 * StartPos);
		HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 3 * Length - 1);
		HexCursor.insertText(StdStringToStructHexQString(tempbuffer));

		CharCursor.setPosition(StartPos);
		CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, Length);
		CharCursor.insertText(QString::fromStdString(tempbuffer));
		});


	
	return 0;
}






int SonPage::ShowSource()
{
    // 获取PE文件的全部二进制数据
    string pptAllBinString = ppt->AllBin();
    
    // 转换为结构化十六进制字符串并显示
    ui.SourceTextHex->setText(StdStringToStructHexQString(pptAllBinString));
    
    // 处理字符显示部分
    QString CharStr;
    for (size_t i = 0; i < pptAllBinString.size(); i++) 
	{
        
        // 处理0-31和127的不可打印字符
		if (pptAllBinString[i] < 32 || pptAllBinString[i] == 127)
			CharStr += '.';
        else 
			CharStr += QChar((unsigned char)pptAllBinString[i]);

		if ((i + 1) % 16 == 0)
			CharStr += '\n';
    }
    
    // 显示处理后的字符
    ui.SourceTextChar->setText(CharStr);

    return 0;
}

int SonPage::ShowTarget()
{
	std::string temp = ppt->Assembly();
	ui.TargetTextHex->setText(StdStringToStructHexQString(temp));

	QString CharStr;;
	for (size_t i = 0; i < temp.size(); i++)
	{

		// 处理0-31和127的不可打印字符
		if (temp[i] < 32 || temp[i] == 127)
			CharStr += '.';
		else
			CharStr += temp[i];

		if ((i + 1) % 16 == 0)
			CharStr += '\n';
	}

	// 显示处理后的字符
	ui.TargetTextChar->setText(CharStr);
	
	return 0;
}

int SonPage::MakeAssembly(HANDLE hFile)
{
	ppt->AssemblyA(hFile);
	return 0;
}