#include "lrclabel.h"


#include<QStringLiteral>
#include<QPainter>
#include<QTimer>

LrcLabel::LrcLabel(QWidget * parent )
    :QLabel( parent )
{

   this->setMinimumSize(800,60);
   this->setMaximumSize(800,60);

    //设置歌词的线性渐变填充
    linearGradient.setStart(0,10);
    linearGradient.setFinalStop(0,40);
    linearGradient.setColorAt(0.1, QColor(14, 179, 255));
    linearGradient.setColorAt(0.5, QColor(114, 232, 255));
    linearGradient.setColorAt(0.9, QColor(14, 179, 255));
    //设置歌词的遮盖填充
    maskLinearGradient.setStart(0,10);
    maskLinearGradient.setFinalStop(0,10);
    maskLinearGradient.setColorAt(0.1, QColor(Qt::yellow));
    maskLinearGradient.setColorAt(0.5, QColor(Qt::yellow));
    maskLinearGradient.setColorAt(0.9, QColor(Qt::yellow));
    //设置字体
    lrcFont.setFamily(QStringLiteral("Times New Roman"));
    lrcFont.setBold(true);
    lrcFont.setPointSize(30);
    //设置定时器
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    //设置遮盖的宽度和时间间隔
    lrc_maskwidth = 0;
    lrc_maskwidthInterval = 0;
}

void LrcLabel::startLrcMask(qint64 Intervaltime)
{
    // 这里设置每隔30毫秒更新一次遮罩的宽度，因为如果更新太频繁
    // 会增加CPU占用率，而如果时间间隔太大，则动画效果就不流畅了
    qreal count = Intervaltime / 50 ;
   // 获取遮罩每次需要增加的宽度，这里的800是部件的固定宽度
    lrc_maskwidthInterval = 800 /  count;

    lrc_maskwidth = 0;
    timer->start( 50 );   //30
}

void LrcLabel::stopLrcMask()
{
    timer->stop();
    lrc_maskwidth = 0 ;
    update();
   /* 更新小部件，除非禁用了更新或隐藏了小部件。

    此函数不会导致立即重绘;相反，当Qt返回到主事件循环时，它为处理调度一个paint事件。这允许Qt进行优化，以获得比调用repaint()更快的速度和更少的闪烁。

    多次调用update()通常只会导致一次paintEvent()调用。

    Qt通常在paintEvent()调用之前擦除小部件的区域。如果设置了Qt::WA_OpaquePaintEvent小部件属性，小部件将负责用不透明的颜色绘制其所有像素。*/

}

void LrcLabel::setLrcText(QString textStr)
{
     QString::SectionFlag flag = QString::SectionSkipEmpty;
      textStr = textStr.section('\n',0,0,flag);    //按照指定字符将字符串分为多个部分，下标由0开始，后面的标志位标识如果分割的部分为空格则跳出。
      this->setText( textStr);
      this->adjustSize();
}


void LrcLabel::timeout()
{
   lrc_maskwidth += lrc_maskwidthInterval;
     update();
}


void LrcLabel::paintEvent(QPaintEvent *paintEvent)
{
    QPainter painter(this);
    painter.setFont( lrcFont );
    // 先绘制底层文字，作为阴影，这样会使显示效果更加清晰，且更有质感
    painter.setPen(QColor(0,0,0,200));
    painter.drawText(1, 1, 800, 60, Qt::AlignLeft, this->text());  //左上角，右下角确定一个矩形
    //再来绘制渐变文字颜色
    painter.setPen(QPen(linearGradient,0));
    painter.drawText(0,0,800,60,Qt::AlignLeft,this->text() );
    //最后绘制遮盖文字渐变颜色
    painter.setPen(QPen(maskLinearGradient,0));
    painter.drawText(0,0,lrc_maskwidth,60,Qt::AlignLeft,this->text());
}

LrcLabel::~LrcLabel()
{
     timer->deleteLater();
}




