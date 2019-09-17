#include "favorwidget.h"
#include "ui_favorwidget.h"

#include<QToolButton>
#include<QVBoxLayout>
#include<QDebug>
#include<QLabel>
#include<QListWidget>
#include<QListWidgetItem>
#include<QMessageBox>
#include<QModelIndex>
#include<QFontMetrics>

FavorWidget::FavorWidget( QWidget *parent):
    QWidget(parent),
    ui(new Ui::favorWidget)
{

   ui->setupUi(this);
  connect(ui->lstWid_favorMusic,SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT( slot_listItemDoubleClicked(QListWidgetItem*)));

}

bool FavorWidget::songIsExist(const SongItem * const songItem, const QListWidget * const listWidget)
{
     QWidget * itemWidget = listWidget->findChild<QWidget *>( songItem->fileName );
    //不存在就要添加
     if( nullptr == itemWidget ) {  return false ;}
    return  true;
}

QWidget *FavorWidget::customeWidgetItem(QWidget * itemWidget ,  SongItem *   songItem)   //增一个形参 songitem
{
     itemWidget->setObjectName( songItem->fileName );

    //设置歌曲名
    QLabel *lbl_audioName =  new QLabel(itemWidget);
    lbl_audioName->setParent( itemWidget );
   lbl_audioName->setText( songItem->fileName );
   lbl_audioName->setToolTip( songItem->fileName );
   lbl_audioName->setAlignment(Qt::AlignCenter);
   QFontMetrics fontWidth(lbl_audioName->font());//得到每个字符的宽度
   QString elideNote = fontWidth.elidedText( lbl_audioName->text(), Qt::ElideRight, 150);//最大宽度150像素

   lbl_audioName->setText(elideNote);//显示省略好的字符串
   lbl_audioName->adjustSize();
   lbl_audioName->setObjectName(QStringLiteral("lbl_audioName")); //在双击行中不可变

   QSpacerItem* hSpacer1 = new QSpacerItem(5, itemWidget->height(), QSizePolicy::Expanding, QSizePolicy::Minimum);

   //设置收藏按钮
   QToolButton *btn_collect =  new QToolButton(itemWidget);
   btn_collect->setParent( itemWidget );
   btn_collect->setFixedSize(20,20);
   btn_collect->setIcon(QIcon(":/rightImage/disklike.png"));
    btn_collect->setStyleSheet(QString("QToolButton{ border:none; background: transparent; }"));
   btn_collect->setIconSize(QSize(20,20));
   btn_collect->setToolTip("取消喜欢");
   btn_collect->setObjectName(QStringLiteral("btn_collect"));
   connect(btn_collect,SIGNAL(clicked(bool)),this,SLOT(slotHandle_favBtnClicked()));

   QSpacerItem* hSpacer2 = new QSpacerItem(5, itemWidget->height(), QSizePolicy::Expanding, QSizePolicy::Minimum);

    //设置删除按钮
    QToolButton *btn_delete = new QToolButton(itemWidget);
    btn_delete->setParent( itemWidget );
    btn_delete->setFixedSize(20,20);
    btn_delete->setIcon(QIcon(":/rightImage/del.png"));
    btn_delete->setStyleSheet(QString("QToolButton{ border:none; background: transparent; }"));
    btn_delete->setIconSize(QSize(20,20));
    btn_delete->setToolTip(QStringLiteral("删除"));
    connect(btn_delete,SIGNAL(clicked(bool)),this,SLOT(slotHandle_delBtnClicked()));
    btn_delete->setObjectName(QStringLiteral("btn_delete"));

    QSpacerItem* hSpacer3 = new QSpacerItem(5, itemWidget->height(), QSizePolicy::Expanding, QSizePolicy::Minimum);
    //设置歌曲时间
    QLabel *lbl_duration = new QLabel(itemWidget);
    lbl_duration->setParent( itemWidget );
    lbl_duration->setText( songItem->playTime );
    lbl_duration->adjustSize();
    lbl_duration->setObjectName( songItem->fileName );
    QSpacerItem* hSpacer4= new QSpacerItem(5, itemWidget->height(), QSizePolicy::Expanding, QSizePolicy::Minimum);


    QHBoxLayout *horLayout = new QHBoxLayout(itemWidget);
    horLayout->setMargin(0);
    horLayout->setSpacing(0);
    horLayout->addWidget(lbl_audioName,4,Qt::AlignLeft);
    horLayout->addSpacerItem(hSpacer1 );
    horLayout->addWidget(btn_collect,1,Qt::AlignRight);
    horLayout->addSpacerItem(hSpacer2);
    horLayout->addWidget(btn_delete,1,Qt::AlignRight);
   horLayout->addSpacerItem(hSpacer3);
    horLayout->addWidget(lbl_duration,2,Qt::AlignRight);
    horLayout->addSpacerItem(hSpacer4);
    itemWidget->setLayout( horLayout );
    return itemWidget;
}

void FavorWidget::slot_addFavorSong( SongItem *  songItem)
{
    if(  nullptr == songItem  ){  return ;}
    bool exist = songIsExist( songItem, ui->lstWid_favorMusic);
    if( exist ) { return ;}

    QListWidgetItem *listItem = new QListWidgetItem( ui->lstWid_favorMusic );
     QWidget  *itemWidget = new QWidget( ui->lstWid_favorMusic );
     listItem->setSizeHint(QSize(320, 30));
     itemWidget->setFixedSize(listItem->sizeHint());
     ui->lstWid_favorMusic->addItem( listItem );

      itemWidget = customeWidgetItem( itemWidget, songItem );
     ui->lstWid_favorMusic->setItemWidget(listItem, itemWidget);
     favorQHash.insert(songItem->fileName, songItem);
     favorPlaylist.clear();
     getFavorPlaylist( ui->lstWid_favorMusic, favorPlaylist);
     //更新数据库中的喜欢列表，增加一个数据
}

void FavorWidget::slot_removeFavorSong(const SongItem * const songItem)
{
    //本表中移除，数据库中也移除
    if( nullptr == songItem ){  return ; }

        QWidget * itemWidget = ui->lstWid_favorMusic->findChild<QWidget *>( songItem->fileName );
        if( nullptr == itemWidget ){ return ;}
       int x = itemWidget->geometry().x();
       int y = itemWidget->geometry().y();
      QModelIndex modelIndex = ui->lstWid_favorMusic->indexAt( QPoint(x,y));
      int row = modelIndex.row();
      QListWidgetItem *listItem = ui->lstWid_favorMusic->takeItem( row );
      if( nullptr == listItem ){ return ;}

      delete itemWidget;
      itemWidget = nullptr;
      delete listItem;
      listItem = nullptr;

     favorPlaylist.clear();
     getFavorPlaylist( ui->lstWid_favorMusic, favorPlaylist);
      //数据库中移除
}

void FavorWidget::setListwidCurrentItem( const int & index)
{
    ui->lstWid_favorMusic->setCurrentRow( index );
}

QString FavorWidget::getAudioName_FromBtnClicked(QToolButton *tBtn, QListWidget *listWidget, int &row)
{
    QWidget *itemWidget = (QWidget*)tBtn->parent();
    int x =  tBtn->geometry().x();
    int y = tBtn->geometry().y();
    QModelIndex modelIndex = listWidget->indexAt(QPoint(x,y));
    row = modelIndex.row() ;
   return itemWidget->objectName();

}

void FavorWidget::getFavorPlaylist(const QListWidget * const listWidget, QList<SongItem *> &favorPlaylist)
{
    int rowCnt = listWidget->count();
    int i = 0;
    QListWidgetItem *listItem = nullptr;
    QWidget * itemWidget = nullptr;
     SongItem *  songItem = nullptr;
    for( i = 0 ; i< rowCnt; i++){
        listItem = listWidget->item( i );
        if( listItem != nullptr ){
            itemWidget = listWidget->itemWidget( listItem );
            if( itemWidget == nullptr ){ return ;}
            songItem =  favorQHash.value( itemWidget->objectName() );
            if( songItem == nullptr ) { continue ;}
            favorPlaylist.append( songItem );
        }
    }
}

void FavorWidget::slotHandle_favBtnClicked()
{
    QToolButton *favBtn =  qobject_cast<QToolButton *>( sender() );   //本表中的都是红心： 表示喜欢的，点击一下则删除数据，并删除数据库里的数据
    if( ! favBtn ){ return ;}

         int row = 0;
        QString text = getAudioName_FromBtnClicked(favBtn,ui->lstWid_favorMusic,row);
         //1.找到要删除的歌曲名去数据库里面删除即可

           //2.从本列表中删除该行数据并释放空间
         QListWidgetItem * listItem = ui->lstWid_favorMusic->takeItem( row );
         if( nullptr == listItem ){ return ;}
         QWidget  *itemWidget = ui->lstWid_favorMusic->itemWidget( listItem );
           delete listItem;
           listItem = nullptr;
           delete itemWidget;
           itemWidget = nullptr;

           //3.告诉历史表已经取消收藏，改变红心图片，修改tooltip,将 histlistwidget 中的favbtn  的objectName 设为 AudioName 方便查找
           emit  sig_updateFavBtnStatus( text );
           favorPlaylist.clear();
           getFavorPlaylist( ui->lstWid_favorMusic, favorPlaylist );
}

void FavorWidget::slotHandle_delBtnClicked()
{
    QToolButton *delBtn =  qobject_cast<QToolButton *>( sender() );
    if( ! delBtn ){ return ;}
    int row = 0;
   QString text = getAudioName_FromBtnClicked(delBtn,ui->lstWid_favorMusic,row);
   int  ret  =  QMessageBox::information(this,tr("温馨提示"),tr("是否确认删除歌曲\" %1 \" ").arg(text),tr("确认"),tr("取消"));   //需要设定为对应的重载项
    if( 0 == ret ){
        //1.从列表中移除
             QListWidgetItem * listItem = ui->lstWid_favorMusic->takeItem( row );
             if( nullptr == listItem ){ return ;}
             QWidget  *itemWidget = ui->lstWid_favorMusic->itemWidget( listItem );
               delete listItem;
               listItem = nullptr;
               delete itemWidget;
               itemWidget = nullptr;
         //2.从数据库中移除( 从喜欢表中移除即可)
    }
}


void FavorWidget::slot_listItemDoubleClicked(QListWidgetItem *listItem)
{
     int row = ui->lstWid_favorMusic->row( listItem );
     if( favorPlaylist.isEmpty() ){
         getFavorPlaylist(ui->lstWid_favorMusic, favorPlaylist );
     }
     emit sig_sendFavorPlaylist(update_favorListwid,row,favorPlaylist);
}

FavorWidget::~FavorWidget()
{
    delete ui;
    ui =nullptr;
}
