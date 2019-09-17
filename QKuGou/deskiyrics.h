#ifndef DESKIYRICS_H
#define DESKIYRICS_H


#include <QDialog>
#include <QPoint>
#include<QTimer>
#include<QList>
#include<QPalette>
class QMouseEvent;
class  QLabel;


namespace Ui {
class Form;
}

class DeskIyrics : public QDialog
{
    Q_OBJECT

public:
    explicit DeskIyrics(QWidget *parent = 0);
    ~DeskIyrics();

    //歌词的渐变控制
   void lrcLabel_startMask(const int &flag, const qreal  &interval);
   void lrcLabel_stopMask(const int &flag);

   //歌词的文本更改
   void lrcLabel_changText(QString firstRowStr , QString secondRowStr);
   void lrcLabel_setVisible( bool IsVisible);

   //歌手写真轮播效果
   void showPicTimer_start(const int &interval);
   void showPicTimer_stop();
   void setBackground(const QPixmap & pixmap);


private:
    void initInterface();
private:
    Ui::Form *ui;
    QPoint mMousePoint;
    bool mMoveEnable;
    QTimer timer_showPic;
//    QList<QPixmap> picVarArray;
    short imgNumer;
    QPalette palette;
    QPixmap pixmap;

};

#endif // DESKIYRICS_H
