#include "softkeyboarddlg.h"
#include "ui_softkeyboarddlg.h"
#include <QMouseEvent>
#include <iostream>

#define MAX_SHOW_PY_NUM 6
#define MAX_SHOW_ZH_NUM 6
#define MAX_PY_LENGTH   6
#define MAX_CHAR_BUTTON_CNT 16

QString SoftKeyBoardDlg::sCurrentResult = NULL;
const QString zhLanguage = "中";
const QString enLanguage = "En";
const QString enChars = ",.?!':@;\"/\\()_-+"
                        "=~^#*%&|[]<>{}$￥`";

SoftKeyBoardDlg::SoftKeyBoardDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoftKeyBoardDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(240, 320);
    initSpellKeyControl();
    initDigitKeyControl();
    initCharKeyControl();
    connect(ui->pushButton_back, SIGNAL(clicked()), this, SLOT(onKeyCloseClicked()));
    connect(ui->pushButton_backSpace, SIGNAL(clicked()), this, SLOT(onKeyBaskspaceClicked()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(onOkClicked()));
    setCharButtonText(1);

    QPalette palette;
    palette.setBrush(QPalette::Background, QPixmap(":/style/background.jpg"));
    this->setPalette(palette);
}

SoftKeyBoardDlg::~SoftKeyBoardDlg()
{
    if(mPyButtonGroup) {
        delete mPyButtonGroup; mPyButtonGroup = nullptr;
    }
    if(mKeyButtonGroup) {
        delete mKeyButtonGroup; mKeyButtonGroup = nullptr;
    }
    if(mDigitKeyButtonGroup) {
        delete mDigitKeyButtonGroup; mDigitKeyButtonGroup = nullptr;
    }
    if(mCharKeyButtonGroup) {
        delete mCharKeyButtonGroup; mCharKeyButtonGroup = nullptr;
    }
    delete ui;
}

/******************** 中英文键盘 ********************************/

void SoftKeyBoardDlg::initSpellKeyControl()
{
    mPyButtonGroup = new QButtonGroup;
    mPyButtonGroup->addButton(ui->pushButton_py1, 1);
    mPyButtonGroup->addButton(ui->pushButton_py2, 2);
    mPyButtonGroup->addButton(ui->pushButton_py3, 3);
    mPyButtonGroup->addButton(ui->pushButton_py4, 4);
    mPyButtonGroup->addButton(ui->pushButton_py5, 5);
    mPyButtonGroup->addButton(ui->pushButton_py6, 6);
    connect(mPyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onPyButtonClicked(int)));

    mKeyButtonGroup = new QButtonGroup;
    mKeyButtonGroup->addButton(ui->key_1, 1);
    mKeyButtonGroup->addButton(ui->key_2, 2);
    mKeyButtonGroup->addButton(ui->key_3, 3);
    mKeyButtonGroup->addButton(ui->key_4, 4);
    mKeyButtonGroup->addButton(ui->key_5, 5);
    mKeyButtonGroup->addButton(ui->key_6, 6);
    mKeyButtonGroup->addButton(ui->key_7, 7);
    mKeyButtonGroup->addButton(ui->key_8, 8);
    mKeyButtonGroup->addButton(ui->key_9, 9);
    connect(mKeyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onKeyButtonClicked(int)));

    ui->label_1->installEventFilter(this);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    ui->label_4->installEventFilter(this);
    ui->label_5->installEventFilter(this);
    ui->label_6->installEventFilter(this);
    ui->key_front->installEventFilter(this);
    ui->key_next->installEventFilter(this);

    connect(ui->pushButton_change, SIGNAL(clicked()), this, SLOT(onPushButtonFontChang()));
    connect(ui->pushButton_null, SIGNAL(clicked()), this, SLOT(onPushButtonNull()));
}


//改变结果栏对应页数
void SoftKeyBoardDlg::changResultPage(int page)
{
    clearLabelText();
    QString showText = getLabelResults();
    if(showText.isEmpty()) {
        return;
    }
    ui->key_page->setText(QString("%1/%2").arg(page).arg(nPagesCount));
    int startMBArray = (page - 1) * MAX_SHOW_ZH_NUM;
    int showCount = page < nPagesCount ? MAX_SHOW_ZH_NUM : nMBCount - (page - 1) * MAX_SHOW_ZH_NUM;
   // qDebug() << currPage << showCount;
    for(int i = 0; i < showCount; i++) {
        this->findChild<QLabel* >(QString("label_%1").arg(i+1))->setText(showText.at(startMBArray + i));
    }
}

bool SoftKeyBoardDlg::eventFilter(QObject *obj, QEvent *event)
{
    int labelIndex = 0;
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent=static_cast<QMouseEvent *>(event);
        if(mouseEvent->buttons() & Qt::LeftButton) {
            do {
                //当前结果个数
                if(nMBCount > 0 && obj == ui->key_front && (nCurrentPage > 1)) {
                    changResultPage(--nCurrentPage); break;
                }
                else if (nMBCount > 0 && obj == ui->key_next && (nCurrentPage < nPagesCount)) {
                    changResultPage(++nCurrentPage); break;
                }
                for(int i = 1; i <= MAX_SHOW_ZH_NUM; i++) {
                    QLabel* tlabel = this->findChild<QLabel* >(QString("label_%1").arg(i));
                    if(obj == tlabel) {labelIndex = i; break;}
                }
            } while(0);
        }
    }
    //选中label中的结果
    if(labelIndex > 0) {
        QLabel* tlabel = this->findChild<QLabel* >(QString("label_%1").arg(labelIndex));
        if(!tlabel->text().isEmpty()) {
            nMBCount = 0; //清除当前拼音的结果
            ui->lineEdit_inputBox->insert(tlabel->text());
            clearPyButtonText();
            clearLabelText();
            ui->key_page->setText(QString("0/0"));
            lInputT9Values.clear();
        }
    }
}

//清除拼音按键的text
void SoftKeyBoardDlg::clearPyButtonText()
{
    for(int i = 1; i <= MAX_SHOW_PY_NUM; i++) {
        this->findChild<QPushButton* >(QString("pushButton_py%1").arg(i))->setText("");
    }
}
//设置拼音按键的text
void SoftKeyBoardDlg::setPyButtonText(int conut, T9PY_IDX **t9Py)
{
    clearPyButtonText();
    for(int i = 0; i < conut; i++) {
        this->findChild<QPushButton* >(QString("pushButton_py%1").arg(i+1))->setText(t9Py[i]->PY);
    }
}
//清除结果栏字符
void SoftKeyBoardDlg::clearLabelText()
{
    for(int i = 1; i <= MAX_SHOW_ZH_NUM; i++) {
        this->findChild<QLabel* >(QString("label_%1").arg(i))->setText("");
    }
}
//选择拼音时，更新结果栏显示

void SoftKeyBoardDlg::updateLabelResult()
{
    nMBCount = getLabelResults().length();
    nPagesCount = (nMBCount % MAX_SHOW_ZH_NUM) ? (nMBCount / MAX_SHOW_ZH_NUM + 1) : nMBCount / MAX_SHOW_ZH_NUM;
    nCurrentPage = 1;
    changResultPage(nCurrentPage);
}

//选择拼音按钮
void SoftKeyBoardDlg::onPyButtonClicked(int id)
{
    if(this->findChild<QPushButton* >(QString("pushButton_py%1").arg(id))->text().isEmpty()) {
        return;
    }
    setLabelResults(QString::fromUtf8((const char*)mT9PyChinese[id - 1]->MB));
    updateLabelResult();
}

/******************** 数字键盘 ********************************/
void SoftKeyBoardDlg::initDigitKeyControl()
{
    mDigitKeyButtonGroup = new QButtonGroup;
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_1, 1);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_2, 2);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_3, 3);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_4, 4);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_5, 5);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_6, 6);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_7, 7);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_8, 8);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_9, 9);
    mDigitKeyButtonGroup->addButton(ui->pushButton_key_10, 10);
    connect(mDigitKeyButtonGroup, SIGNAL(buttonClicked(int)), this,  SLOT(onDigitKeyButtonClicked(int)));
    connect(ui->pushButton_key_back, SIGNAL(clicked()), this, SLOT(onGoBackClicked()));
    connect(ui->pushButton_key_null, SIGNAL(clicked()), this, SLOT(onPushButtonNull()));
}

void SoftKeyBoardDlg::onDigitKeyButtonClicked(int id)
{
    QPushButton* tButton = this->findChild<QPushButton* >(QString("pushButton_key_%1").arg(id));
    ui->lineEdit_inputBox->insert(tButton->text());
}

/******************** 符号键盘 ********************************/
void SoftKeyBoardDlg::initCharKeyControl()
{
    mCharKeyButtonGroup = new QButtonGroup;
    mCharKeyButtonGroup->addButton(ui->pushButton_char_1, 1);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_2, 2);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_3, 3);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_4, 4);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_5, 5);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_6, 6);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_7, 7);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_8, 8);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_9, 9);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_10, 10);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_11, 11);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_12, 12);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_13, 13);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_14, 14);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_15, 15);
    mCharKeyButtonGroup->addButton(ui->pushButton_char_16, 16);
    connect(mCharKeyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onCharKeyButtonClicked(int)));
    connect(ui->pushButton_char_back, SIGNAL(clicked()), this, SLOT(onGoBackClicked()));
    connect(ui->pushButton_char_null, SIGNAL(clicked()), this, SLOT(onPushButtonNull()));
    connect(ui->pushButton_char_front, SIGNAL(clicked()), this, SLOT(onPushButtonCharFrontClicked()));
    connect(ui->pushButton_char_next, SIGNAL(clicked()), this, SLOT(onPushButtonCharNextClicked()));
}

void SoftKeyBoardDlg::onPushButtonCharFrontClicked()
{
    if(m_pCurrentCharPage > 1) {
        this->clearCharButtonText();
        this->setCharButtonText(--m_pCurrentCharPage);
    }
}

void SoftKeyBoardDlg::onPushButtonCharNextClicked()
{
    if(m_pCurrentCharPage < 3) {
        this->clearCharButtonText();
        this->setCharButtonText(++m_pCurrentCharPage);
    }
}

void SoftKeyBoardDlg::clearCharButtonText()
{
    for(int i = 1; i <= MAX_CHAR_BUTTON_CNT; i++) {
        this->findChild<QPushButton* >(QString("pushButton_char_%1").arg(i))->setText("");
    }
}

void SoftKeyBoardDlg::setCharButtonText(int page)
{
    int showCount = (enChars.length() - MAX_CHAR_BUTTON_CNT * (page - 1)) > MAX_CHAR_BUTTON_CNT
            ? MAX_CHAR_BUTTON_CNT : enChars.length() - MAX_CHAR_BUTTON_CNT * (page - 1);

    int startArray = (page - 1) * MAX_CHAR_BUTTON_CNT;
    for(int i = 1; i <= showCount; i++) {
        if(enChars.at(startArray + i - 1) == '&') {
            this->findChild<QPushButton* >(QString("pushButton_char_%1").arg(i))->setText("&&");
        }
        else {
            this->findChild<QPushButton* >(QString("pushButton_char_%1").arg(i))->setText(
                        enChars.at(startArray + i - 1));
        }
    }
}

void SoftKeyBoardDlg::onCharKeyButtonClicked(int id)
{
    QString text = this->findChild<QPushButton* >(QString("pushButton_char_%1").arg(id))->text();
    if(!text.isEmpty()) {
        if(text == "&&") {
             ui->lineEdit_inputBox->insert("&");
        }
        else {
             ui->lineEdit_inputBox->insert(text);
        }
    }
}

/***********************************************************************/
void SoftKeyBoardDlg::release()
{
    if(lInputT9Values.size() > 0) {
        lInputT9Values.clear();
    }
    setLabelResults("");
    ui->key_page->setText(QString("0/0"));
    clearLabelText();
    clearPyButtonText();
}


//更新拼音字库
void SoftKeyBoardDlg::updatePyAndZh()
{
    QString pymbStr;
    for(int i = 0; i < lInputT9Values.size(); i++) {
        pymbStr += lInputT9Values[i];
    }
    nResultCounts = getMatchedPymb((unsigned char*)pymbStr.toStdString().c_str(), mT9PyChinese);
    if(nResultCounts > 0 && nResultCounts <= MAX_SHOW_PY_NUM) {
        setPyButtonText(nResultCounts, mT9PyChinese);
        onPyButtonClicked(1);
    }
    else {
        lInputT9Values.pop_back();
    }
}

//点击按键
void SoftKeyBoardDlg::onKeyButtonClicked(int id)
{
    if(id >= 2  && id <= 9) {
        if(ui->pushButton_change->text() == enLanguage) {
            char str[8];
            if(getMatchedPyEn(id, str)) {
                setLabelResults(QString::fromUtf8(str));
                updateLabelResult();
            }

        }
        else {
            if(lInputT9Values.size() < MAX_PY_LENGTH) {
                lInputT9Values.push_back(QString::number(id));
                updatePyAndZh();
            }
        }
    }
}


//手动返回close
void SoftKeyBoardDlg::onKeyCloseClicked()
{
    *softKeyValue = "";
    ui->lineEdit_inputBox->clear();
    release();
    this->close();
    //这里添加一个手动关闭信号, 为了让父窗口重新启动自动关闭
    emit closeDialogManual();
}

/**需要s启动父窗口的自动关闭定时器**/
//点击确定
void SoftKeyBoardDlg::onOkClicked()
{
    release();
    *softKeyValue = ui->lineEdit_inputBox->text();
    ui->lineEdit_inputBox->clear();
    this->close();
}

//点击回退
void SoftKeyBoardDlg::onKeyBaskspaceClicked()
{
    if(lInputT9Values.size() > 0) {
        lInputT9Values.pop_back();
        if(lInputT9Values.size() == 0) {
            clearPyButtonText();
            clearLabelText();
            ui->key_page->setText(QString("0/0"));
        }
        else {
            updatePyAndZh();
        }
    }
    else if(ui->lineEdit_inputBox->text().size() > 0) {
        ui->lineEdit_inputBox->backspace();
        ui->lineEdit_inputBox->setEnabled(true);
        ui->lineEdit_inputBox->setFocus();
    }
}

//中英切换
void SoftKeyBoardDlg::onPushButtonFontChang()
{
    if(bIsEnglishMode) {
        return;
    }
    release();
    if(ui->pushButton_change->text() == zhLanguage) {
        ui->pushButton_change->setText(enLanguage);
    }
    else {
         ui->pushButton_change->setText(zhLanguage);
    }
}

//空格
void SoftKeyBoardDlg::onPushButtonNull()
{
    ui->lineEdit_inputBox->insert(" ");
    ui->lineEdit_inputBox->setEnabled(true);
    ui->lineEdit_inputBox->setFocus();
}

void SoftKeyBoardDlg::onGoBackClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_spell);
    release();
}

void SoftKeyBoardDlg::setSoftKeyBoardLanguage()
{
//    if(SystemSetting::Language == SYSTEM_LANGUAGE_EN) {
//        bIsEnglishMode = true;
//        ui->pushButton_change->setText(enLanguage);
//    }
//    else {
//        bIsEnglishMode = false;
//        ui->pushButton_change->setText(zhLanguage);
//    }
}

void SoftKeyBoardDlg::on_pushButton_char_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_chars);
}

void SoftKeyBoardDlg::on_pushButton_digit_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_digit);
}


int SoftKeyBoardDlg::startExec(QString *value)
{
    softKeyValue = value;
    //setSoftKeyBoardLanguage();
    ui->stackedWidget->setCurrentWidget(ui->page_spell);
    return this->exec();
}


