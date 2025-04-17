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

    int OpenFileAndInitial();     //��Ӱ�ť
	int WindowInitial();      //���ڳ�ʼ��
	int FieldTreeInitial(); //�ֶ�����ʼ��

    int ShowSourceHex();
    int ShowTargetHex();


    int ShellcodeTamper();

    int MakeAssembly(HANDLE hFile);

private:
    Ui::PEVirusAct_GUIClass ui;
};
