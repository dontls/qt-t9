#ifndef SOFTKEYBOARDDLG_H
#define SOFTKEYBOARDDLG_H

#include "fontlib_zh.h"

#include <QDialog>
#include <QButtonGroup>
#include <QLabel>
#include <QDialog>

namespace Ui {
class SoftKeyBoardDlg;
}

class SoftKeyBoardDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SoftKeyBoardDlg(QWidget *parent = 0);
    ~SoftKeyBoardDlg();
    int startExec(QString* value = 0);

private:
    Ui::SoftKeyBoardDlg *ui;

    QList<QString> lInputT9Values;
    T9PY_IDX* mT9PyChinese[6];
    QButtonGroup* mKeyButtonGroup;
    QButtonGroup* mPyButtonGroup;
    QButtonGroup* mDigitKeyButtonGroup;
    QButtonGroup* mCharKeyButtonGroup;

    static QString sCurrentResult;
    int nResultCounts = 0;
    int nCurrentPyIndex = 0;

    int nCurrentPage = 0;
    int nPagesCount = 0;
    int nMBCount = 0;

    QString* softKeyValue;
    bool bIsEnglishMode = false;

    int m_pCurrentCharPage = 1;
private:

    void initSpellKeyControl();
    void initDigitKeyControl();
    void initCharKeyControl();

    void clearPyButtonText();
    void clearLabelText();
    void setPyButtonText(int conut, T9PY_IDX **t9Py);
    void changResultPage(int page);
    void updateLabelResult();
    void release();

    static void setLabelResults(QString result) { sCurrentResult = result;}
    static QString getLabelResults() { return sCurrentResult;}
    void updatePyAndZh();

    void setSoftKeyBoardLanguage();

    void clearCharButtonText();
    void setCharButtonText(int page);

signals:
    void closeDialogManual();
private slots:
    void onKeyCloseClicked();
    void onKeyBaskspaceClicked();
    void onPushButtonNull();
    void onGoBackClicked();
    void onOkClicked();


    /** Spell Mode **/
    void onKeyButtonClicked(int id);
    void onPyButtonClicked(int id);
    void onPushButtonFontChang();
    /** Digit Mode **/
    void onDigitKeyButtonClicked(int id);
    /** Char Mode **/
    void onCharKeyButtonClicked(int id);

    void onPushButtonCharFrontClicked();
    void onPushButtonCharNextClicked();

    void on_pushButton_char_clicked();
    void on_pushButton_digit_clicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

#endif // SOFTKEYBOARDDLG_H
