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



// NT头签名信息
const std::vector<FieldInfo> Signature = {
    {"Signature", 4, 0x0}
};

// 文件头字段信息
const std::vector<FieldInfo> IMAGE_FILE_HEADER_INFO = {
    {"Machine", 2, 0x0},
    {"NumberOfSections", 2, 0x2},
    {"TimeDateStamp", 4, 0x4},
    {"PointerToSymbolTable", 4, 0x8},
    {"NumberOfSymbols", 4, 0xC},
    {"SizeOfOptionalHeader", 2, 0x10},
    {"Characteristics", 2, 0x12}
};


// 可选头字段信息 (64位)
const std::vector<FieldInfo> IMAGE_OPTIONAL_HEADER_INFO = {
    {"Magic", 2, 0x0},
    {"MajorLinkerVersion", 1, 0x2},
    {"MinorLinkerVersion", 1, 0x3},
    {"SizeOfCode", 4, 0x4},
    {"SizeOfInitializedData", 4, 0x8},
    {"SizeOfUninitializedData", 4, 0xC},
    {"AddressOfEntryPoint", 4, 0x10},
    {"BaseOfCode", 4, 0x14},
    {"ImageBase", 8, 0x18},
    {"SectionAlignment", 4, 0x20},
    {"FileAlignment", 4, 0x24},
    {"MajorOperatingSystemVersion", 2, 0x28},
    {"MinorOperatingSystemVersion", 2, 0x2A},
    {"MajorImageVersion", 2, 0x2C},
    {"MinorImageVersion", 2, 0x2E},
    {"MajorSubsystemVersion", 2, 0x30},
    {"MinorSubsystemVersion", 2, 0x32},
    {"Win32VersionValue", 4, 0x34},
    {"SizeOfImage", 4, 0x38},
    {"SizeOfHeaders", 4, 0x3C},
    {"CheckSum", 4, 0x40},
    {"Subsystem", 2, 0x44},
    {"DllCharacteristics", 2, 0x46},
    {"SizeOfStackReserve", 8, 0x48},
    {"SizeOfStackCommit", 8, 0x50},
    {"SizeOfHeapReserve", 8, 0x58},
    {"SizeOfHeapCommit", 8, 0x60},
    {"LoaderFlags", 4, 0x68},
    {"NumberOfRvaAndSizes", 4, 0x6C}
};




// 数据目录字段信息
const std::vector<FieldInfo> IMAGE_DATA_DIRECTORY_INFO = {
	{"Export Table", 8, 0x0},
	{"Import Table", 8, 0x8},
	{"Resource Table", 8, 0x10},
	{"Exception Table", 8, 0x18},
	{"Certificate Table", 8, 0x20},
	{"Base Relocation Table", 8, 0x28},
	{"Debug", 8, 0x30},
	{"Architecture", 8, 0x38},
	{"Global Ptr", 8, 0x40},
	{"TLS Table", 8, 0x48},
	{"Load Config Table", 8, 0x50},
	{"Bound Import", 8, 0x58},
	{"IAT", 8, 0x60},
	{"Delay Import Descriptor", 8, 0x68},
	{"CLR Runtime Header", 8, 0x70},
	{"Reserved", 8, 0x78}
};

// 节表字段信息
const std::vector<FieldInfo> IMAGE_SECTION_HEADER_INFO = {
    {"Name", 8, 0x0},
    {"VirtualSize", 4, 0x8},
    {"VirtualAddress", 4, 0xC},
    {"SizeOfRawData", 4, 0x10},
    {"PointerToRawData", 4, 0x14},
    {"PointerToRelocations", 4, 0x18},
    {"PointerToLinenumbers", 4, 0x1C},
    {"NumberOfRelocations", 2, 0x20},
    {"NumberOfLinenumbers", 2, 0x22},
    {"Characteristics", 4, 0x24}
};




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

	HANDLE hFile = CreateFileA((LPCSTR)(FileName.toStdString().c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
	{
		QMessageBox::critical(this, "错误", "文件读取失败");
		return;
	}

	ppt = new PETamper(hFile);
	FilePath = FileName;
	ui.setupUi(this);
	MakeTree(ui.SourceFieldTree);
	MakeTree(ui.TargetFieldTree);
	SourceFieldTreeInitial();
	TargetFieldTreeInitial();
	ShowSource();
	ShowTarget();
	ScrollInitial();

	//FieldTreeAdjust();

	connect(ui.ShellcodeTamper, &QPushButton::clicked, this, &SonPage::ShellcodeTamper);



	connect(ui.MakeAssembly, &QPushButton::clicked, this, [this]() {
		QString Title = "选择输出位置";
		QString FileName = QFileDialog::getSaveFileName(this, Title, "", "All Files (*)");
		HANDLE hFile = CreateFileW((LPCWSTR)FileName.data(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		MakeAssembly(hFile);
		CloseHandle(hFile);
		});


}

SonPage::~SonPage()
{

	delete ppt;
}

int SonPage::FieldModify(int position, int address, int size, std::string buffer)
{
	if (buffer.size() > size)
	{
		QMessageBox::critical(this, "错误", "缓冲区过大");
		//qDebug() << "Buffer size is larger than the specified size.";
		return 1;
	}

	buffer.resize(size);
	
	QTextCursor cursor(ui.SourceTextHex->textCursor());
	cursor.setPosition(address);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, size);
	cursor.insertText(QString::fromStdString(buffer));


	return 0;
}

int SonPage::MakeTree(QTreeView* tree)
{
	QChar hc = 'h';
	QString tempqstr;
	int tempaddress = 0;
	tree->setUpdatesEnabled(false);
	tree->setUniformRowHeights(true);

	//设置头
	QStandardItemModel* qmodel = new QStandardItemModel(tree);
	qmodel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("名称") << QStringLiteral("值") << QStringLiteral("地址") << QStringLiteral("大小"));
	std::string buffer;

	//IMAGE_DOS_HEADER
	QList<QStandardItem*> idh_Header(4);
	idh_Header[0] = new QStandardItem(QStringLiteral("IMAGE_DOS_HEADER"));
	idh_Header[1] = new QStandardItem("");
	idh_Header[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
	idh_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_DOS_HEADER), 0, 16).toUpper() + hc);
	qmodel->appendRow(idh_Header);

	for (int i = 0; i < IMAGE_DOS_HEADER_INFO.size(); i++)
	{
		QList<QStandardItem*> idh_Field(4);
		idh_Field[0] = new QStandardItem(QString::fromStdString(IMAGE_DOS_HEADER_INFO[i].field));
		buffer.resize(IMAGE_DOS_HEADER_INFO[i].size);
		memmove(buffer.data(), (PVOID)((unsigned long long) & ppt->idh + IMAGE_DOS_HEADER_INFO[i].offset), IMAGE_DOS_HEADER_INFO[i].size);
		idh_Field[1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<unsigned long long*>(buffer.data()), 0, 16).toUpper() + hc);
		idh_Field[2] = new QStandardItem(QString("%1").arg(IMAGE_DOS_HEADER_INFO[i].offset, 0, 16).toUpper() + hc);
		idh_Field[3] = new QStandardItem(QString("%1").arg(0 + IMAGE_DOS_HEADER_INFO[i].size, 0, 16).toUpper() + hc);
		idh_Header[0]->appendRow(idh_Field);
	}

	//DOS Stub
	QList<QStandardItem*> ids_Header(4);
	ids_Header[0] = new QStandardItem(QStringLiteral("IMAGE_DOS_STUB"));
	ids_Header[1] = new QStandardItem("");
	ids_Header[2] = new QStandardItem(QString("%1").arg(tempaddress = sizeof(IMAGE_DOS_HEADER), 0, 16).toUpper() + hc);
	ids_Header[3] = new QStandardItem(QString("%1").arg(ppt->stubbuffer.size(), 0, 16).toUpper() + hc);
	qmodel->appendRow(ids_Header);


	//IMAGE_NT_HEADERS
	QList<QStandardItem*> inh_Header(4);
	inh_Header[0] = new QStandardItem(QStringLiteral("IMAGE_NT_HEADERS"));
	inh_Header[1] = new QStandardItem("");
	inh_Header[2] = new QStandardItem(QString("%1").arg(tempaddress = ppt->idh.e_lfanew, 0, 16).toUpper() + hc);
	inh_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_NT_HEADERS), 0, 16).toUpper() + hc);
	qmodel->appendRow(inh_Header);

	QList<QStandardItem*>pSignature(4);
	pSignature[0] = new QStandardItem(QStringLiteral("DWORD Signature"));
	pSignature[1] = new QStandardItem(QString("%1").arg(ppt->inh.Signature, 0, 16).toUpper() + hc);
	pSignature[2] = new QStandardItem(QString("%1").arg(tempaddress + Signature[0].offset, 0, 16).toUpper() + hc);
	pSignature[3] = new QStandardItem(QString("%1").arg(Signature[0].size, 0, 16).toUpper() + hc);
	inh_Header[0]->appendRow(pSignature);

	//IMAGE_FILE_HEADER
	QList<QStandardItem*>ifh_Header(4);
	ifh_Header[0] = new QStandardItem(QStringLiteral("IMAGE_FILE_HEADER"));
	ifh_Header[1] = new QStandardItem("");
	ifh_Header[2] = new QStandardItem(QString("%1").arg(tempaddress += Signature[0].size, 0, 16).toUpper() + hc);
	ifh_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_FILE_HEADER), 0, 16).toUpper() + hc);
	inh_Header[0]->appendRow(ifh_Header);

	for (int i = 0; i < IMAGE_FILE_HEADER_INFO.size(); i++)
	{
		QList<QStandardItem*> ifh_Field(4);

		ifh_Field[0] = new QStandardItem(QString::fromStdString(IMAGE_FILE_HEADER_INFO[i].field));
		buffer.resize(IMAGE_FILE_HEADER_INFO[i].size);
		memmove(buffer.data(), (PVOID)((unsigned long long) & ppt->inh.FileHeader + IMAGE_FILE_HEADER_INFO[i].offset), IMAGE_FILE_HEADER_INFO[i].size);
		ifh_Field[1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<unsigned long long*>(buffer.data()), 0, 16).toUpper() + hc);
		ifh_Field[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
		ifh_Field[3] = new QStandardItem(QString("%1").arg(IMAGE_FILE_HEADER_INFO[i].size, 0, 16).toUpper() + hc);
		tempaddress += IMAGE_FILE_HEADER_INFO[i].size;
		ifh_Header[0]->appendRow(ifh_Field);
	}

	//IMAGE_POTIONAL_HEADER
	QList<QStandardItem*>ioh_Header(4);
	ioh_Header[0] = new QStandardItem(QStringLiteral("IMAGE_OPTIONAL_HEADER"));
	ioh_Header[1] = new QStandardItem("");
	ioh_Header[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
	ioh_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_OPTIONAL_HEADER), 0, 16).toUpper() + hc);
	inh_Header[0]->appendRow(ioh_Header);

	for (int i = 0; i < IMAGE_OPTIONAL_HEADER_INFO.size(); i++)
	{
		QList<QStandardItem*> ioh_Field(4);
		ioh_Field[0] = new QStandardItem(QString::fromStdString(IMAGE_OPTIONAL_HEADER_INFO[i].field));
		buffer.resize(IMAGE_OPTIONAL_HEADER_INFO[i].size);
		memmove(buffer.data(), (PVOID)((unsigned long long) & ppt->inh.OptionalHeader + IMAGE_OPTIONAL_HEADER_INFO[i].offset), IMAGE_OPTIONAL_HEADER_INFO[i].size);
		ioh_Field[1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<unsigned long long*>(buffer.data()), 0, 16).toUpper() + hc);
		ioh_Field[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
		ioh_Field[3] = new QStandardItem(QString("%1").arg(IMAGE_OPTIONAL_HEADER_INFO[i].size, 0, 16).toUpper() + hc);
		tempaddress += IMAGE_OPTIONAL_HEADER_INFO[i].size;
		ioh_Header[0]->appendRow(ioh_Field);
	}
	QList<QStandardItem*>idd_Header(4);
	idd_Header[0] = new QStandardItem(QStringLiteral("IMAGE_DATA_DIRECTORY"));
	idd_Header[1] = new QStandardItem("");
	idd_Header[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
	idd_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_DATA_DIRECTORY_INFO.size(), 0, 16).toUpper() + hc);
	ioh_Header[0]->appendRow(idd_Header);
	QVector< QList<QStandardItem*>>idd_Field(IMAGE_DATA_DIRECTORY_INFO.size());
	for (int i = 0; i < IMAGE_DATA_DIRECTORY_INFO.size(); i++)
	{
		QList<QStandardItem*> idd_Field(4);
		idd_Field[0] = new QStandardItem(QString::fromStdString(IMAGE_DATA_DIRECTORY_INFO[i].field));
		idd_Field[1] = new QStandardItem("");
		idd_Field[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
		idd_Field[3] = new QStandardItem(QString("%1").arg(IMAGE_DATA_DIRECTORY_INFO[i].size, 0, 16).toUpper() + hc);
		idd_Header[0]->appendRow(idd_Field);

		QVector<QList<QStandardItem*>>idd_Field_2(2);
		idd_Field_2[0].resize(4);
		idd_Field_2[0][0] = new QStandardItem(QStringLiteral("DWORD VirtualAddress"));
		idd_Field_2[0][1] = new QStandardItem(QString("%1").arg(ppt->inh.OptionalHeader.DataDirectory[i].VirtualAddress, 0, 16).toUpper() + hc);
		idd_Field_2[0][2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
		idd_Field_2[0][3] = new QStandardItem(QString("%1").arg(sizeof(ppt->inh.OptionalHeader.DataDirectory[i].VirtualAddress), 0, 16).toUpper() + hc);
		idd_Field[0]->appendRow(idd_Field_2[0]);
		tempaddress += sizeof(ppt->inh.OptionalHeader.DataDirectory[i].VirtualAddress);

		idd_Field_2[1].resize(4);
		idd_Field_2[1][0] = new QStandardItem(QStringLiteral("DWORD Size"));
		idd_Field_2[1][1] = new QStandardItem(QString("%1").arg(ppt->inh.OptionalHeader.DataDirectory[i].Size, 0, 16).toUpper() + hc);
		idd_Field_2[1][2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
		idd_Field_2[1][3] = new QStandardItem(QString("%1").arg(sizeof(ppt->inh.OptionalHeader.DataDirectory[i].Size), 0, 16).toUpper() + hc);
		idd_Field[0]->appendRow(idd_Field_2[1]);
		tempaddress += sizeof(ppt->inh.OptionalHeader.DataDirectory[i].Size);
	}

	//IMAGE_SECTION_HEADERS
	QList<QStandardItem*> ish_Header(4);
	ish_Header[0] = new QStandardItem(QStringLiteral("IMAGE_SECTION_HEADERS"));
	ish_Header[1] = new QStandardItem("");
	ish_Header[2] = new QStandardItem(QString("%1").arg(ppt->idh.e_lfanew + sizeof(IMAGE_NT_HEADERS), 0, 16).toUpper() + hc);
	ish_Header[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_SECTION_HEADER) * ppt->NumberOfSections, 0, 16).toUpper() + hc);
	qmodel->appendRow(ish_Header);
	for (int i = 0; i < ppt->NumberOfSections; i++)
	{
		QList<QStandardItem*> ish_Field(4);
		ish_Field[0] = new QStandardItem(QString("IMAGE_SECTION_HEADER[%1]").arg(i));
		ish_Field[1] = new QStandardItem(*reinterpret_cast<char*>(ppt->SectionNames[i].data()));
		ish_Field[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
		ish_Field[3] = new QStandardItem(QString("%1").arg(sizeof(IMAGE_SECTION_HEADER), 0, 16).toUpper() + hc);
		ish_Header[0]->appendRow(ish_Field);

		for (int j = 0; j < IMAGE_SECTION_HEADER_INFO.size(); j++)
		{
			QList<QStandardItem*> ish_Field_2(4);
			ish_Field_2[0] = new QStandardItem(QString::fromStdString(IMAGE_SECTION_HEADER_INFO[j].field));
			buffer.resize(IMAGE_SECTION_HEADER_INFO[j].size);
			memmove(buffer.data(), (PVOID)((unsigned long long) (&ppt->SectionHeaders[i]) + IMAGE_SECTION_HEADER_INFO[j].offset), IMAGE_SECTION_HEADER_INFO[j].size);
			if (j == 0)
				ish_Field_2[1] = new QStandardItem(QString::fromStdString(buffer));
			else
				ish_Field_2[1] = new QStandardItem(QString("%1").arg(*reinterpret_cast<unsigned int*>(buffer.data()), 0, 16).toUpper() + hc);
			ish_Field_2[2] = new QStandardItem(QString("%1").arg(tempaddress, 0, 16).toUpper() + hc);
			ish_Field_2[3] = new QStandardItem(QString("%1").arg(IMAGE_SECTION_HEADER_INFO[j].size, 0, 16).toUpper() + hc);
			ish_Field[0]->appendRow(ish_Field_2);
			tempaddress += IMAGE_SECTION_HEADER_INFO[j].size;
		}
	}


	//IMAGE_SECTION_DATAS
	QVector<QList<QStandardItem*>> isd_Header(ppt->NumberOfSections);
	for (int i = 0; i < ppt->NumberOfSections; i++)
	{
		isd_Header[i].resize(4);
		isd_Header[i][0] = new QStandardItem(QString("IMAGE_SECTION_DATA[%1]").arg(i));
		isd_Header[i][1] = new QStandardItem(reinterpret_cast<char*>(ppt->SectionNames[i].data()));
		isd_Header[i][2] = new QStandardItem(QString("%1").arg(ppt->SectionHeaders[i].PointerToRawData, 0, 16).toUpper() + hc);
		isd_Header[i][3] = new QStandardItem(QString("%1").arg(ppt->SectionHeaders[i].SizeOfRawData, 0, 16).toUpper() + hc);
		qmodel->appendRow(isd_Header[i]);
	}

	tree->setModel(qmodel);

	return 0;
}

int SonPage::SourceFieldTreeInitial()
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

	//ui.SourceFieldTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	QStandardItemModel* qmodel = qobject_cast<QStandardItemModel*>(ui.SourceFieldTree->model());
	ui.SourceFieldTree->setUpdatesEnabled(true);
	ui.SourceFieldTree->setEditTriggers(QAbstractItemView::NoEditTriggers);


	
	//设置点击后高亮显示区域
	//
	//性能需要优化

	connect(ui.SourceFieldTree->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, qmodel](const QItemSelection& selected, const QItemSelection& deselected) {
		Q_UNUSED(selected);
		Q_UNUSED(deselected);

		QTextCursor HexCursor(ui.SourceTextHex->textCursor());
		QTextCursor CharCursor(ui.SourceTextChar->textCursor());
		QTextCharFormat format;
		QModelIndexList IndexList = ui.SourceFieldTree->selectionModel()->selectedIndexes();

		if (SourceHighLight.HexPos != -1)
		{
			format.setBackground(Qt::white);

			HexCursor.setPosition(SourceHighLight.HexPos);
			HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, SourceHighLight.HexLength);
			HexCursor.mergeCharFormat(format);

			CharCursor.setPosition(SourceHighLight.CharPos);
			CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, SourceHighLight.CharLength);
			CharCursor.mergeCharFormat(format);
		}


		if (!IndexList.isEmpty())
		{
			QModelIndex index = IndexList.first();
			QString temp;

			temp = qmodel->itemFromIndex(index.siblingAtColumn(2))->text();
			int StartPos = temp.left(temp.size() - 1).toInt(NULL, 16);
			temp = qmodel->itemFromIndex(index.siblingAtColumn(3))->text();
			int Length = temp.left(temp.size() - 1).toInt(NULL, 16);

			format.setBackground(Qt::yellow);

			HexCursor.setPosition(SourceHighLight.HexPos = 3 * StartPos);
			HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, SourceHighLight.HexLength = 3 * Length - 1);
			HexCursor.mergeCharFormat(format);

			CharCursor.setPosition(SourceHighLight.CharPos = StartPos + StartPos / 16);
			CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, SourceHighLight.CharLength = Length + Length / 16);
			CharCursor.mergeCharFormat(format);
		}
		});

	return 0;
}

int SonPage::TargetFieldTreeInitial()
{
	QStandardItemModel* tqmodel = qobject_cast<QStandardItemModel*>(ui.TargetFieldTree->model());
	ui.TargetFieldTree->setUpdatesEnabled(true);
	SetTreeEdit(ui.TargetFieldTree);

	//设置点击后高亮显示区域

	connect(ui.TargetFieldTree->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this, tqmodel](const QItemSelection& selected, const QItemSelection& deselected) {
		Q_UNUSED(selected);
		Q_UNUSED(deselected);

		QTextCursor HexCursor(ui.TargetTextHex->textCursor());
		QTextCursor CharCursor(ui.TargetTextChar->textCursor());
		QTextCharFormat format;
		QModelIndexList IndexList = ui.TargetFieldTree->selectionModel()->selectedIndexes();

		if (TargetHighLight.HexPos != -1)
		{
			format.setBackground(Qt::white);
			HexCursor.setPosition(TargetHighLight.HexPos);
			HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, TargetHighLight.HexLength);
			HexCursor.mergeCharFormat(format);

			CharCursor.setPosition(TargetHighLight.CharPos);
			CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, TargetHighLight.CharLength);
			CharCursor.mergeCharFormat(format);
		}


		if (!IndexList.isEmpty())
		{
			QModelIndex index = IndexList.first();
			QString temp;

			temp = tqmodel->itemFromIndex(index.siblingAtColumn(2))->text();
			int StartPos = temp.left(temp.size() - 1).toInt(NULL, 16);
			temp = tqmodel->itemFromIndex(index.siblingAtColumn(3))->text();
			int Length = temp.left(temp.size() - 1).toInt(NULL, 16);

			format.setBackground(Qt::yellow);
			HexCursor.setPosition(TargetHighLight.HexPos = 3 * StartPos);
			HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, TargetHighLight.HexLength = 3 * Length - 1);
			HexCursor.mergeCharFormat(format);

			CharCursor.setPosition(TargetHighLight.CharPos = StartPos + StartPos / 16);
			CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, TargetHighLight.CharLength = Length + Length / 16);
			CharCursor.mergeCharFormat(format);

		}
		});

	


	connect(ui.TargetFieldTree, &QTreeView::doubleClicked, this, [=](const QModelIndex& index) {
		refield = tqmodel->itemFromIndex(index)->text();
		});


	//字段修改，这个也要优化
	connect(tqmodel, &QStandardItemModel::dataChanged, this, [=](const QModelIndex& index, const QModelIndex&) {

		QTextCursor HexCursor(ui.TargetTextHex->textCursor());
		QTextCursor CharCursor(ui.TargetTextChar->textCursor());

		QModelIndex tempindex = index;
		QString temp;
		int position = 0;


		temp = tqmodel->itemFromIndex(index)->text();
		string tempbuffer = "";
		if (temp[temp.size() - 1] == 'h')
		{
			temp = temp.left(temp.size() - 1);
			if (temp.size() % 2 == 1)
				temp = "0" + temp;
			tempbuffer.resize(temp.size() / 2);
			for (int i = 0; i < temp.size() / 2; i++)
			{
				if (!(temp[i] >= '0' && temp[i] <= '9') && !(temp[i] >= 'A' && temp[i] <= 'F') && !(temp[i] >= 'a' && temp[i] <= 'f'))
				{
					QMessageBox::critical(this, "错误", "非法字符");
					tqmodel->itemFromIndex(index)->setText(refield);
					return;
				}
				tempbuffer[i] = (char)(temp.mid(2 * (temp.size() / 2 - i - 1), 2).toInt(NULL, 16));
			}
		}
		else
		{

			for (int i = 0; i < temp.size(); i++)
			{
				if (!(temp[i] >= '0' && temp[i] <= '9'))
				{
					QMessageBox::critical(this, "错误", "非法字符");
					tqmodel->itemFromIndex(index)->setText(refield);
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
		temp = tqmodel->itemFromIndex(index.siblingAtColumn(3))->text();
		int Length = temp.left(temp.size() - 1).toInt(NULL, 16);
		if (Length < tempbuffer.size())
		{
			QMessageBox::critical(this, "错误", "字符过长");
			tqmodel->itemFromIndex(index)->setText(refield);
			return;
		}
		tempbuffer.resize(Length);
		temp = tqmodel->itemFromIndex(index.siblingAtColumn(2))->text();
		int StartPos = temp.left(temp.size() - 1).toInt(NULL, 16);

		while (tempindex.isValid() && tempindex.parent().isValid())
			tempindex = tempindex.parent();
		QString ParentName = tqmodel->itemFromIndex(tempindex)->text();
		if (ParentName == "IMAGE_DOS_HEADER")
		{
			position = 0;
			ppt->FieldTamper(&ppt->idh, StartPos - position, (char*)tempbuffer.c_str());
		}
		else if (ParentName == "IMAGE_NT_HEADERS")
		{
			position = ppt->idh.e_lfanew;
			ppt->FieldTamper(&ppt->inh, StartPos - position, (char*)tempbuffer.c_str());
		}
		else if (ParentName == "IMAGE_SECTION_HEADERS")
		{
			QModelIndex ParentIndex = index.parent();
			position = ppt->idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + ParentIndex.row() * sizeof(IMAGE_SECTION_HEADER);	//////////////
			ppt->FieldTamper(&ppt->SectionHeaders[ParentIndex.row()], StartPos - position, (char*)tempbuffer.c_str());
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

int Recursion(QStandardItem* qo, QStandardItemModel* model)
{
	
	model->itemFromIndex(qo->index().siblingAtColumn(0))->setEditable(false);
	model->itemFromIndex(qo->index().siblingAtColumn(2))->setEditable(false);
	model->itemFromIndex(qo->index().siblingAtColumn(3))->setEditable(false);
	if (qo->parent() != nullptr || qo->child(0) == nullptr)
		model->itemFromIndex(qo->index().siblingAtColumn(1))->setEditable(true);
	else
	{
		model->itemFromIndex(qo->index().siblingAtColumn(1))->setEditable(false);
		for (int i = 0; i < qo->rowCount(); i++)
			Recursion(qo->child(i), model);
	}
	return 0;
}

int SonPage::SetTreeEdit(QTreeView *tree)
{
	QStandardItemModel* model = (QStandardItemModel*)tree->model();
	QStandardItem* root = model->invisibleRootItem();
	
	for (int i = 0; i < root->rowCount(); i++)
	{
		Recursion(root->child(i), model);
	}

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
		if ((unsigned int)pptAllBinString[i] < 32 || (unsigned int)pptAllBinString[i] == 127)
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
		if ((unsigned int)temp[i] < 32 || (unsigned int)temp[i] == 127)
			CharStr += '.';
		else
			CharStr += QChar((unsigned char)temp[i]);

		if ((i + 1) % 16 == 0)
			CharStr += '\n';
	}

	ui.TargetTextChar->setText(CharStr);
	return 0;
}

int SonPage::ShellcodeTamper()
{

	QString input = QInputDialog::getText(this, "输入shellcode", "请依照0x00或00h的方式，以','分隔");
	if (input.isEmpty())
	{
		QMessageBox::critical(this, "错误", "shellcode为空");
		throw std::logic_error("shellcode empty");
		return 1;
	}
	QString QShellcode;
	
	QStringList List = input.split(",");
	QString temp;
	for (int i = 0; i < List.size(); i++)
	{
		if (List[i][List[i].size() - 1] == 'h')
			List[i].chop(1);
		while(List[i][0]==' '|| List[i][0] == '\n' || List[i][0] == '\r')
			List[i] = List[i].mid(1);
		if (List[i][0] == '0' && List[i][1] == 'x')
			List[i] = List[i].mid(2);
		for (int j = 0; j < 2; j++)
			if (!(List[i][j] >= '0' && List[i][j] <= '9') && !(List[i][j] >= 'A' && List[i][j] <= 'F') && !(List[i][j] >= 'a' && List[i][j] <= 'f'))
			{
				QMessageBox::critical(this, "错误", "输入有误：存在非法字符");
				return 1;
			}

		temp.append(QChar((unsigned char)List[i].toInt(NULL, 16)));
	}

	std::string shellcodestr = temp.toLatin1().toStdString();	//包含ASCII扩展字符

	ppt->ShellcodeInjection(shellcodestr.data(), shellcodestr.size());

	QString CharStr;
	QTextCursor HexCursor(ui.TargetTextHex->textCursor());
	QTextCursor CharCursor(ui.TargetTextChar->textCursor());
	for (int i = 0; i < ppt->NumberOfSections; i++)
	{
		if (!strcmp((const char*)ppt->SectionNames[i].data(), ".text"))
		{
			HexCursor.setPosition(ppt->SectionHeaders[i].PointerToRawData * 3);
			HexCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, shellcodestr.size() * 3 - 1);
			CharCursor.setPosition(ppt->SectionHeaders[i].PointerToRawData / 16 * 17);
			CharCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, shellcodestr.size() / 16 * 17);

			for (size_t j = 0; j < shellcodestr.size(); j++)
			{

				// 处理0-31和127的不可打印字符
				if ((unsigned int)shellcodestr[j] < 32 || (unsigned int)shellcodestr[j] == 127)
					CharStr += '.';
				else
					CharStr += QChar((unsigned char)shellcodestr[j]);

				if ((j + (ppt->SectionHeaders[i].PointerToRawData / 16 * 17) + 1) % 16 == 0)
					CharStr += '\n';
			}
		}
	}

	HexCursor.insertText(StdStringToStructHexQString(shellcodestr));
	CharCursor.insertText(CharStr);
	

	return 0;
}

int SonPage::MakeAssembly(HANDLE hFile)
{
	ppt->MakeAssembly(hFile);
	return 0;
}