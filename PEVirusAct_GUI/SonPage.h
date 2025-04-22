#pragma once

#include <QWidget>
#include "ui_SonPage.h"
#include"PETamper.h"

typedef struct highlight {
	int HexPos = -1;
	int HexLength = -1;
	int CharPos = -1;
	int CharLength = -1;
}highlight;

class SonPage : public QWidget
{
	Q_OBJECT

public:
	QString FilePath;
	PETamper *ppt;
	struct highlight SourceHighLight;
	struct highlight TargetHighLight;
	QString refield;

	SonPage(QWidget* parent = nullptr, QString FileName = NULL);

	~SonPage();

	//int WindowInitial();      //窗口初始化
	int MakeTree(QTreeView* tree);
	int SetTreeEdit(QTreeView* tree);
	int SourceFieldTreeInitial(); //字段树初始化 放到构造函数
	int TargetFieldTreeInitial();
	int FieldTreeAdjust();

	int ShowSource();
	int ShowTarget();
	int ShellcodeTamper();
	//int SlotsInitial();
	int ScrollInitial();
	//int FieldHighlight(int position, int address, int size);
	int FieldModify(int position, int address, int size, std::string buffer);
	int MakeAssembly(HANDLE hFile);

private:
	Ui::SonPageClass ui;
};
