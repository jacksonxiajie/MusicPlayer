#include "deskiyrics.h"
#include "ui_lrcwidget.h"

#include<QMouseEvent>
#include<QStringLiteral>
#include<QRect>
DeskIyrics::DeskIyrics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose); //继承Qwidget 窗体关闭时自动销毁
    this->setWindowFlags( Qt::FramelessWindowHint );

    initInterface();
}

void DeskIyrics::initInterface()
{
    imgNumer = 0;
    palette = this->palette;

   ui->lbl_firstRow->adjustSize();
   ui->lbl_firstRow->setFrameShape(QFrame::NoFrame);

   ui->lbl_secondRow->adjustSize();
   ui->lbl_secondRow->setFrameShape(QFrame::NoFrame);

}

//歌词的遮盖和渐变填充
void DeskIyrics::lrcLabel_startMask(const int &flag, const qreal  &interval)
{
    if( 1 == flag){
         ui->lbl_firstRow->startLrcMask( interval );
    }else if ( 2 == flag ){
        ui->lbl_secondRow->startLrcMask( interval );
    }

}

void DeskIyrics::lrcLabel_stopMask(const int &flag)
{
    if( 1 == flag){
          ui->lbl_firstRow->stopLrcMask();
    }else if ( 2 == flag ){
        ui->lbl_secondRow->stopLrcMask();
    }
}

void DeskIyrics::lrcLabel_changText(QString firstRowStr , QString secondRowStr)
{
      ui->lbl_firstRow->setLrcText( firstRowStr );
      ui->lbl_secondRow->setLrcText( secondRowStr );

}

void DeskIyrics::lrcLabel_setVisible(bool IsVisible)
{
     ui->lbl_firstRow->setVisible( IsVisible );
     ui->lbl_secondRow->setVisible( IsVisible);
}

void DeskIyrics::showPicTimer_start(const int &interval)
{
    imgNumer = 0 ;
    timer_showPic.start( interval );
}

void DeskIyrics::showPicTimer_stop()
{
    imgNumer = 0;
    timer_showPic.stop();
}

void DeskIyrics::setBackground(const QPixmap &pixmap)
{
    QPalette palette = this->palette;
    palette.setBrush(QPalette::Background,QBrush( QPixmap( pixmap)));
    this->setPalette( palette );
}



DeskIyrics::~DeskIyrics()
{
    delete ui;
}


