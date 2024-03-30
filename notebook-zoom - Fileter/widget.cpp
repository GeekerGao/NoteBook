#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QShortcut>
#include <QTextEdit>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->textEdit->installEventFilter(this);
    //虽然上面一行代码进行widget和ui的窗口关联，但是如果发生窗口大小变化时，里面的布局不会随之改变
    //通过下面这行代码进行显示说明，让窗口变化时，布局及其子控件随之调整
    this->setLayout(ui->verticalLayout);
    //快捷键(打开、保存、放大、缩小)设置
    QShortcut *shortcutOpen = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")),this);
    QShortcut *shortcutSave = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")),this);
    QShortcut *shortcutZoomIn = new QShortcut(QKeySequence(tr("Ctrl+Shift+=", "File|Save")),this);
    QShortcut *shortcutZoomOut = new QShortcut(QKeySequence(tr("Ctrl+Shift+-", "File|Save")),this);
    //绑定信号与槽
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(oncurrentIndexChanged(int)));
    connect(ui->textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(oncursorPositionChanged()));
    connect(shortcutOpen,&QShortcut::activated,[=](){
        on_btnOpen_clicked();
    });
    connect(shortcutSave,&QShortcut::activated,[=](){
        on_btnSave_clicked();
    });
    connect(shortcutZoomIn,&QShortcut::activated,[=](){
        zoomIn();
    });
    connect(shortcutZoomOut,&QShortcut::activated,[=](){
        zoomOut();
    });
}

Widget::~Widget()
{
    delete ui;
}
//快捷键放大字体
void Widget::zoomIn()
{
    //获得TestEdit的当前字体信息
    QFont font = ui->textEdit->font();
    //获得当前字体大小
    int fontSize = font.pointSize();
    if(fontSize == -1) return;
    //改变大小，并设置字体大小
    int newFontSize = fontSize + 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
}
//快捷键缩小字体
void Widget::zoomOut()
{
    //获得TestEdit的当前字体信息
    QFont font = ui->textEdit->font();
    //获得当前字体大小
    int fontSize = font.pointSize();
    if(fontSize == -1) return;
    //改变大小，并设置字体大小
    int newFontSize = fontSize - 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Wheel){
        if(QGuiApplication::keyboardModifiers() == Qt::ControlModifier){
            QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
            if(wheelEvent->angleDelta().y() > 0){
                zoomIn();
            }else if(wheelEvent->angleDelta().y() < 0){
                zoomOut();
            }
            return true;
        }
        return false;
    }
}
//打开
void Widget::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),
                                                    "D:/Study/",
                                                    tr("Text(*.txt)"));
    ui->textEdit->clear();
    file.setFileName(fileName);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "file open error!";
    }
    this->setWindowTitle(fileName + "-高英杰记事本");
    QTextStream in(&file);
    //in.setCodec("UTF-8");
    QString str = ui->comboBox->currentText(); //把QString转化成char *
    const char* c_str = str.toStdString().c_str();
    in.setCodec(c_str);

    while(!in.atEnd()){
        QString context = in.readLine();
        //qDebug() << qPrintable(context);
        //ui->textEdit->setText(context);
        ui->textEdit->append(context);
    }
}
//保存
void Widget::on_btnSave_clicked()
{
    //判断文件没有打开，执行下面代码
    if(!file.isOpen()){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        "D:/Study/untitled.txt",
                                                        tr("Text (*.txt *.doc)"));
        file.setFileName(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            qDebug() << "file open error!";
        }
        this->setWindowTitle(fileName + "-高英杰记事本");
    }
    //文件写入
    QTextStream out(&file);
    out.setCodec(ui->comboBox->currentText().toStdString().c_str());
    QString context = ui->textEdit->toPlainText();
    out << context;
}
//关闭
void Widget::on_btnClose_clicked()
{
    QMessageBox msgBox;
    int ret = QMessageBox::warning(this, tr("高英杰记事本 注意："),
                                   tr("当前文件未保存，请问是否保存？"),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel,
                                   QMessageBox::Save);
    switch (ret) {
    case QMessageBox::Save:
        on_btnSave_clicked();
        break;
    case QMessageBox::Discard:
        ui->textEdit->clear();
        if(file.isOpen()){
            file.close();
            this->setWindowTitle("高英杰记事本");
        }
        break;
    case QMessageBox::Cancel:
        break;
    default:
        break;
    }
}
//在下拉框索引改变时，从一个已经打开的文件中读取内容，并将其显示在用户界面中的一个文本编辑器中。
void Widget::oncurrentIndexChanged(int)
{
    ui->textEdit->clear();
    if(file.isOpen()){
        QTextStream in(&file);
        //确保文件以正确的编码方式读取
        in.setCodec(ui->comboBox->currentText().toStdString().c_str());
        //将文件指针移动到文件的开头
        file.seek(0);
        //逐行读取文件中的内容，直到到达文件末尾
        while(!in.atEnd()){
            QString context = in.readLine();
            ui->textEdit->append(context);
        }
    }
}
//显示光标位置行列序号
void Widget::oncursorPositionChanged()
{
    QTextCursor cursor =  ui->textEdit->textCursor();
    //qDebug() << cursor.blockNumber() + 1 << cursor.columnNumber() + 1;
    QString blockNum = QString::number(cursor.blockNumber() + 1);
    QString columnNum = QString::number(cursor.columnNumber() + 1);
    const QString labelMes = "第" + blockNum + "行" + "第" + columnNum  + "列";
    ui->labelPosition->setText(labelMes);
    //设置当前行高亮
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection ext;
    //1.知道当前行
    ext.cursor = ui->textEdit->textCursor();
    //2.颜色
    QBrush qBrush(Qt::yellow);
    ext.format.setBackground(qBrush);
    //配置段属性：整行显示，没有这句话不行
    ext.format.setProperty(QTextFormat::FullWidthSelection,true);
    //3.设置
    //把ext加入到ext容器中
    extraSelections.append(ext);
    ui->textEdit->setExtraSelections(extraSelections);
}
