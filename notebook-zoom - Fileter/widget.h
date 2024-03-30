#ifndef WIDGET_H
#define WIDGET_H

#include <QFile>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    QFile file;
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void zoomIn();
    void zoomOut();

    bool eventFilter(QObject *watched,QEvent *event);
private slots:
    void on_btnOpen_clicked();
    void on_btnSave_clicked();
    void on_btnClose_clicked();
    void oncurrentIndexChanged(int index);
    void oncursorPositionChanged();
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
