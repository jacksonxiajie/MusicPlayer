#ifndef LRCLABEL_H
#define LRCLABEL_H

#include<QLabel>
#include<QLinearGradient>
#include<QFont>
#include <QPaintEvent>

class QTimer;

class LrcLabel :  public QLabel
{
     Q_OBJECT
public:
    explicit  LrcLabel( QWidget * parent  );
    ~LrcLabel();

    //开始歌词遮罩
    void startLrcMask(qint64 Intervaltime);
    //停止歌词遮罩
    void stopLrcMask();
   //设置文本
    void setLrcText(QString textStr);

protected:
    void paintEvent(QPaintEvent  *paintEvent);

private slots:
    void timeout();

private:

    QLinearGradient  linearGradient;
    QLinearGradient   maskLinearGradient;
    QFont lrcFont;

    qreal  lrc_maskwidth;
    qreal  lrc_maskwidthInterval;

    QTimer *timer;
};

#endif // LRCLABEL_H
