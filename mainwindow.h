#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "softkeyboarddlg.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SoftKeyBoardDlg* m_pSoftKeyBoardDlg;

private slots:

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
