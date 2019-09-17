#include "playmodeform.h"
#include "ui_playmodeform.h"
#include<QListWidgetItem>
PlayModeForm::PlayModeForm(QWidget *parent) :
    QFrame(parent),
    ui( new Ui::PlayModeForm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->LastIndex = 4;

    ui->lstWid_playMode->installEventFilter( this );
    ui->lstWid_playMode->setStyleSheet(QString( "QListWidget{border-top:0px solid white;background:  transparent;}"));
   ui->lstWid_playMode->setCurrentRow( 3);   //默认为循环播放模式
    ui->lstWid_playMode->item( 3)->setTextColor( QColor(Qt::blue));
    ui->lstWid_playMode->item( 3)->setToolTip(ui->lstWid_playMode->currentItem()->text() );

    connect(ui->lstWid_playMode,&QListWidget::currentRowChanged, [=]( int index ){
              emit sig_playModeChanged( index );
               ui->lstWid_playMode->item( LastIndex )->setTextColor(QColor(Qt::black));
                  QListWidgetItem *item =  ui->lstWid_playMode->item(index);
                  if( item != nullptr ){
                      item->setSelected( true );
                      item->setTextColor( QColor(Qt::blue ));
                  }
                  this->LastIndex = index;
            });

    connect(ui->lstWid_playMode,&QListWidget::currentTextChanged, [=](QString toolTip){
                emit sig_modeTipChanged( toolTip );
                });
}


PlayModeForm::~PlayModeForm()
{
    delete ui;
}


