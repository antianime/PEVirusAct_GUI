#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PEVirusAct_GUI.h"

class PEVirusAct_GUI : public QMainWindow
{
    Q_OBJECT

public:
    PEVirusAct_GUI(QWidget *parent = nullptr);
    ~PEVirusAct_GUI();

private:
    Ui::PEVirusAct_GUIClass ui;
};
