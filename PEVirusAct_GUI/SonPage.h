#pragma once

#include <QWidget>
#include "ui_SonPage.h"
#include"PETamper.h"

class SonPage : public QWidget
{
	Q_OBJECT

public:
	QString FilePath;
	PETamper *ppt;
	SonPage(QWidget* parent = nullptr, QString FileName = NULL);

	~SonPage();

	//int WindowInitial();      //窗口初始化
	int FieldTreeInitial(); //字段树初始化 放到构造函数

	int ShowSource();
	int ShowTarget();
	int syncLineScrolling();
	int ShellcodeTamper();
	int SlotsInitial();
	int ScrollInitial();
	//int FieldHighlight(int position, int address, int size);
	int FieldModify(int position, int address, int size, std::string buffer);
	int MakeAssembly(HANDLE hFile);

private:
	Ui::SonPageClass ui;
};
