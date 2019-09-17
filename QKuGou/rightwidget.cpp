#include "rightwidget.h"
#include "ui_rightwidget.h"

#include "favorwidget.h"
#include "nativewidget.h"

#include <QLabel>
#include<QMessageBox>
#include<QStringLiteral>
#include<QHBoxLayout>
#include<QDebug>
#include<QToolButton>

RightWidget::RightWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::RightWidget)
{
    ui->setupUi(this);
    favorWidget = nullptr;
    nativeWidget = nullptr;

    connect(ui->listWid_histMusic,SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT( slot_listItemDoubleClicked(QListWidgetItem*)));
    connect(this,SIGNAL(tabBarClicked(int)),this,SLOT(slotHandle_tabBarClicked(int)));

    init_favorWidget();
    inItialListWidget();

}

void RightWidget::init_favorWidget()
{
    show_FavorWidget();
}

void RightWidget::inItialListWidget()
{

    QFile qssFile(":/RightWidStyle.qss");
    if( qssFile.open(QFile::ReadOnly)){
        this->setStyleSheet( qssFile.readAll() );
        qssFile.close();
    }

    ui->listWid_histMusic->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWid_histMusic->setWindowFlags(Qt::FramelessWindowHint);

}

void RightWidget::sethistQhash(const QHash<QString, SongItem *> &hash)
{
    this->histQhash = hash;
}

void RightWidget::setListwidCurrentItem(const int &index)
{
    ui->listWid_histMusic->setCurrentRow( index );
}


QWidget* RightWidget::customeWidgetItem(QWidget * const itemWidget , SongItem *const  songItem )  //const SongItem *songItem
{
    itemWidget->setObjectName( songItem->fileName );   //在遍历歌曲列表时需要用到
    //设置歌曲名
    QLabel *lbl_audioName =  new QLabel(itemWidget);
    lbl_audioName->setParent( itemWidget );
    lbl_audioName->setText( songItem->fileName );
    lbl_audioName->setToolTip( songItem->fileName );
    lbl_audioName->adjustSize();
    lbl_audioName->setAlignment(Qt::AlignCenter);
    QFontMetrics fontWidth(lbl_audioName->font());
    //得到每个字符的宽度，当要显示的字符超过label能显示字符个数的最大值时，用省略号显示后面部分
    QString elideNote = fontWidth.elidedText( lbl_audioName->text(), Qt::ElideRight, 150);//最大宽度150像素
    lbl_audioName->setText( elideNote );
    lbl_audioName->setObjectName(QStringLiteral( "lbl_audioName")); //在双击行的处理事件中要用到该值
    //设置收藏按钮
    QToolButton *btn_collect =  new QToolButton(itemWidget);
    btn_collect->setFixedSize(20,20);
    btn_collect->setStyleSheet(QString("QToolButton{ border:none; background: transparent; }"));
    btn_collect->setIcon(QIcon(":/rightImage/Ilike.png"));
    btn_collect->setIconSize(QSize(20,20));
    btn_collect->setToolTip("我喜欢");
    btn_collect->setObjectName( songItem->fileName );
    connect(btn_collect,SIGNAL(clicked(bool)),this,SLOT(slotHandle_favBtnClicked()));
    //设置删除按钮
    QToolButton *btn_delete = new QToolButton(itemWidget);
    btn_delete->setFixedSize(20,20);
    btn_delete->setStyleSheet(QString("QToolButton{ border:none; background: transparent; }"));
    btn_delete->setIcon(QIcon(":/rightImage/del.png"));
    btn_delete->setIconSize(QSize(20,20));
    btn_delete->setToolTip(QStringLiteral("删除"));
    btn_delete->setObjectName( songItem->fileName );   //在删除一个歌曲时要用到该值。
    connect(btn_delete,SIGNAL(clicked(bool)),this,SLOT(slotHandle_delBtnClicked()));
    //设置歌曲时间
    QLabel *lbl_duration = new QLabel(itemWidget);
    lbl_duration->setText( songItem->playTime );
    lbl_duration->adjustSize();
    lbl_duration->setObjectName( songItem->fileName );    //在添加一首歌曲时，查找是否存在，要用到该值。
    QHBoxLayout *horLayout = new QHBoxLayout(itemWidget);
    horLayout->setMargin(0);
    horLayout->setSpacing(0);
    horLayout->addWidget(lbl_audioName,4,Qt::AlignLeft);
    horLayout->addWidget(btn_collect,1,Qt::AlignRight);
    horLayout->addWidget(btn_delete,1,Qt::AlignRight);
    horLayout->addWidget(lbl_duration,2,Qt::AlignRight);
    itemWidget->setLayout( horLayout );
    return itemWidget;
}

bool RightWidget::songIsExist(const SongItem * const songItem, const QListWidget * const listWidget, int &row)
{

     QLabel * label = listWidget->findChild<QLabel *>( songItem->fileName );
    //不存在就要添加
     if( nullptr == label ) {  return false ;}

    //存在就要定位
     int x = label->geometry().x();
     int y = label->geometry().y();
     QModelIndex modeIndex =  listWidget->indexAt( QPoint(x ,y));
     row = modeIndex.row();
    return  true;
}



//接收中心widget  传过来的数据
void RightWidget::slot_addSongToPlayList(QHash<QString, SongItem *> &hash)
{

           if(  hash.isEmpty() ){ return ;}
           int row = 0;
           QHash<QString, SongItem *>::const_iterator iterHash = hash.constBegin();

           while( iterHash != hash.constEnd() )
           {
                histQhash.insert(  iterHash.key(), iterHash.value() );  //如果已经存在则不在重新插入，而insertMutilp 则继续插入
                bool  exist = songIsExist( iterHash.value() , ui->listWid_histMusic ,row);
              if(  exist ) {
                          //存在就定位到具体的行数,并设置其图片
                          ui->listWid_histMusic->setCurrentRow(row);
                     }
              else{
                  //如果列表中不包含则添加到播放列表
                  QListWidgetItem*  listItem=  new QListWidgetItem( ui->listWid_histMusic );
                  QWidget * itemWidget = new QWidget( ui->listWid_histMusic );
                  listItem->setSizeHint(QSize(320, 30));
                  itemWidget->setFixedSize(listItem->sizeHint());

                  SongItem  * songItem = iterHash.value();

                  ui->listWid_histMusic->addItem( listItem );
                  itemWidget = customeWidgetItem( itemWidget , songItem );
                  ui->listWid_histMusic->setItemWidget( listItem,itemWidget);
                 }
              ++iterHash;    //无论是否包含都迭代到下一个数据
            }

}

void RightWidget::slot_setListWidCurItem(const ListWidgetFlags &listWidgetFlags, const int &index)
{
    switch ( listWidgetFlags ) {
    case update_nativeListWid:
         nativeWidget->setListwidCurrentItem( index );
        break;
    case update_favorListwid:
        favorWidget->setListwidCurrentItem( index );
        break;
   case update_histListwid:
        this->setListwidCurrentItem( index );
        break;
   default:
        break;
    }
}

QString RightWidget::getAudioName_FromBtnClicked(QToolButton *tBtn, QListWidget *listWidget, int &row)
{
   QString text_fileName =tBtn->objectName();
    QWidget *ItemWidget = (QWidget *)tBtn->parent();
    int x = ItemWidget->geometry().x();
    int y = ItemWidget->geometry().y();
    QModelIndex modelIndex = ui->listWid_histMusic->indexAt(QPoint(x,y));
    row = modelIndex.row() ;
   return text_fileName;
}

void RightWidget::slotHandle_favBtnClicked()
{
    QToolButton *favBtn =  qobject_cast<QToolButton *>( sender() );
    if( ! favBtn ){ return ;}
    int row = 0;
    QString text = getAudioName_FromBtnClicked(favBtn,ui->listWid_histMusic,row);
    SongItem * songItem = histQhash.value( text );
    if(  nullptr ==  songItem ) {  return ;}
    if( favBtn->toolTip() == QStringLiteral("我喜欢")){
        favBtn->setIcon(QIcon(":/rightImage/disklike.png")); //images: 喜欢图片
        favBtn->setIconSize(QSize(20,20));
        favBtn->setToolTip(QStringLiteral("取消喜欢"));
        emit sig_adFavListWidItem( songItem );    //发松信号告知添加一个喜欢的歌曲
        return ;
    }else if( favBtn->toolTip() == QStringLiteral("取消喜欢")){
        favBtn->setIcon(QIcon(":/rightImage/Ilike.png")); //images: 默认收藏图片
        favBtn->setIconSize(QSize(20,20));
        favBtn->setToolTip(QStringLiteral("我喜欢"));
        emit  sig_rvFavListWidItem( songItem );
        return;
    }
}

void RightWidget::slotHandle_delBtnClicked()
{
    QToolButton *delBtn  = qobject_cast<QToolButton *>( sender() );
    if(  ! delBtn ) { return ;}
    int row = 0;
    QString text = getAudioName_FromBtnClicked(delBtn, ui->listWid_histMusic,row);
    int  ret  =  QMessageBox::information(this,tr("温馨提示"),tr("是否确认删除歌曲\" %1 \" ").arg(text),tr("确认"),tr("取消"));   //需要设定为对应的重载项
    if( 0 == ret ){
        QListWidgetItem * listItem = ui->listWid_histMusic->takeItem( row );
        QWidget * itemWidget = ui->listWid_histMusic->itemWidget( listItem );
        delete itemWidget;
        itemWidget = nullptr;
        delete  listItem;     //使用delete 而不是delete [] ,对于类类型是有区别的。
        listItem = nullptr;
        this->histPlaylist.clear(); //再次更新历史播放列表
        getHistPlaylist( ui->listWid_histMusic, histPlaylist );
    }
}


//双击每一行执行播放操作
void RightWidget::slot_listItemDoubleClicked(QListWidgetItem *listItem)
{
    int row = ui->listWid_histMusic->row( listItem );
    if( histPlaylist.isEmpty() ){
        getHistPlaylist(ui->listWid_histMusic, histPlaylist );
    }
    emit sig_sendHistPlaylist(update_histListwid,row, histPlaylist);
}


void RightWidget::slotHandle_tabBarClicked(int index)
{
    if( 2 == index ) {
        show_NativeWidget();
    }
}

void RightWidget::show_FavorWidget()
{
    if(  nullptr == favorWidget   ){
        favorWidget =  new FavorWidget( ui->tab_favor );
        QVBoxLayout  * vBoxLayout =  new QVBoxLayout;
        vBoxLayout->setMargin(0);
       vBoxLayout->SetFixedSize;
       vBoxLayout->setSpacing(0);
       vBoxLayout->addWidget( favorWidget );
       ui->tab_favor->setLayout( vBoxLayout );
       connect(this,SIGNAL(sig_adFavListWidItem(SongItem*)),favorWidget, SLOT(slot_addFavorSong(SongItem*)) );
       connect(this,SIGNAL(sig_rvFavListWidItem(SongItem*const)), favorWidget, SLOT(slot_removeFavorSong(SongItem*const)));
       connect( favorWidget,SIGNAL( sig_updateFavBtnStatus(QString)), this,SLOT( slot_setFavBtnStatus(QString)));
      connect( favorWidget,&FavorWidget::sig_sendFavorPlaylist, this,&RightWidget::sig_sendHistPlaylist);
    }
}

void RightWidget::show_NativeWidget()
{
    if(  nullptr == nativeWidget   ){
        nativeWidget =  new NativeWidget;
        nativeWidget->setParent( ui->tab_native );
        QVBoxLayout  * vBoxLayout =  new QVBoxLayout;
        vBoxLayout->setMargin(0);
       vBoxLayout->SetFixedSize;
       vBoxLayout->setSpacing(0);
       vBoxLayout->addWidget( nativeWidget );
       ui->tab_native->setLayout( vBoxLayout ); 
       connect(nativeWidget, SIGNAL(sig_sendPlayHash(const int &,  const  QList< QString > & )), this,SIGNAL(sig_sendPlayHash(const int &,  const  QList< QString > & )));
    }

}

void RightWidget::getHistPlaylist(const QListWidget * const listWidget, QList<SongItem *> &HistPlaylist)
{
    int rowCnt = listWidget->count();
    int i = 0;
    QListWidgetItem *listItem = nullptr;
    QWidget * itemWidget = nullptr;
    SongItem * songItem = nullptr;
    for( i = 0 ; i< rowCnt; i++){
        listItem = listWidget->item( i );
        if( listItem != nullptr ){
            itemWidget = listWidget->itemWidget( listItem );
            songItem = histQhash.value( itemWidget->objectName() );
            if( songItem == nullptr ) { continue ;}
            HistPlaylist.append( songItem );
        }
    }
}


void RightWidget::slot_setFavBtnStatus(const QString objectName)
{
    QToolButton * tBtn_collect = ui->listWid_histMusic->findChild<QToolButton *>( objectName );
    if( nullptr == tBtn_collect) { return ;}
    tBtn_collect->setToolTip("我喜欢");
    tBtn_collect->setIcon( QIcon(":/rightImage/Ilike.png"));
    tBtn_collect->setIconSize(QSize(20,20));
}


RightWidget::~RightWidget()
{
    delete nativeWidget;
    nativeWidget = nullptr;
    delete favorWidget;
    favorWidget = nullptr;
     delete ui;
}
