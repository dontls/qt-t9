#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pSoftKeyBoardDlg = new SoftKeyBoardDlg;
    ui->lineEdit_inputbox->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent=static_cast<QMouseEvent *>(event);
        if(mouseEvent->buttons() & Qt::LeftButton) {
            if(obj == ui->lineEdit_inputbox) {
                QString value;
                m_pSoftKeyBoardDlg->startExec(&value);
                if(!value.isEmpty())
                    ui->lineEdit_inputbox->setText(value);
            }
        }
    }
}
