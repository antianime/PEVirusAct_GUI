#pragma once


#include<iostream>
#include "ui_PEVirusAct_GUI.h"
#include"fh.h"

class PEVirusAct_GUI : public QMainWindow
{
    Q_OBJECT

public:
    PEVirusAct_GUI(QWidget *parent = nullptr);
    ~PEVirusAct_GUI();

    int OpenFileAndCreateSonPage();     //Ìí¼Ó°´Å¥
	


    

private:
    Ui::PEVirusAct_GUIClass ui;
};
