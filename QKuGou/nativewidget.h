#ifndef NATIVEWIDGET_H
#define NATIVEWIDGET_H

#include <QWidget>
#include<QList>
#include<QString>
#include<QListWidgetItem>
#include<QUrl>
#include <QMediaContent>
namespace Ui {
class NativeWidget;
}

class NativeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NativeWidget(QWidget *parent = 0);
    ~NativeWidget();
     void Load_nativeMusic(const QString &scanDir);
      void setListwidCurrentItem(const int & index );
private slots:

    void on_chBox_native_clicked(bool checked);
    void slot_nativeListItem_DoubleClicked(QListWidgetItem *item);
    void on_tBtn_scan_clicked();

signals:
    void sig_sendPlayHash(const  int &row, const  QList< QString > & nativePlayList );

private:
    Ui::NativeWidget *ui;
   QList< QString > nativePlayList;
};

#endif // NATIVEWIDGET_H
