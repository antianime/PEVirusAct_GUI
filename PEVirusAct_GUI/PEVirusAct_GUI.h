#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include<iostream>
#include "ui_PEVirusAct_GUI.h"
#include"fh.h"

class PEVirusAct_GUI : public QMainWindow
{
    Q_OBJECT

public:
    PEVirusAct_GUI(QWidget *parent = nullptr);
    ~PEVirusAct_GUI();

    int OpenFileAndInitial();     //添加按钮
	int WindowInitial();      //窗口初始化
	int FieldTreeInitial(); //字段树初始化

    int ShowSourceHex();
    int ShowTargetHex();


    int ShellcodeTamper();

    int MakeAssembly(HANDLE hFile);

private:
    Ui::PEVirusAct_GUIClass ui;
};
