#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "customfunction.h"
#include "downloadform.h"
#include "deskiyrics.h"
#include "mnetworker.h"
#include "mcentralwidget.h"
#include "rightwidget.h"
#include "playmodeform.h"
#include "msystemtray.h"

#include<QMouseEvent>
#include<QDesktopWidget>
#include<QMessageBox>
#include <QVBoxLayout>
//#include<QMediaMetaData>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   this-> setWindowFlags( Qt::FramelessWindowHint );
   this-> setAttribute(Qt::WA_TranslucentBackground);
    this-> setAttribute( Qt::WA_DeleteOnClose );

    QFile file(":/TopWidStyle.qss");
    if(file.open(QFile::ReadOnly)){
        this->setStyleSheet( file.readAll() );
        file.close();
      }

    inIt_privateData();
    inIt_MediaPlayer();
    inIt_NetWork();
    inIt_BottomWidget();
    inIt_RightWidget();
    inIt_CentralWidget();
}

void MainWindow::inIt_privateData()
{
    mediaPlayer = nullptr;
    musicPlayList = nullptr;
    desklyrics = nullptr;
    mNetWork = nullptr;
    downloadForm = nullptr;
    mCentralWidget =  nullptr;
    rightWidget = nullptr;
     playModeForm = nullptr;
     msystemTray = nullptr;
     centralWidget_VLayout = nullptr;
     rightWidget_VLayout  = nullptr;
     stackedLayout = nullptr;

    qRegisterMetaType<PlayHash>("PlayHash");
    listWidgetFlags = update_Null;
    IsUpdateWidget = false ;
     lyricflag = 0;
}

void MainWindow::inIt_MediaPlayer()
{
    mediaPlayer =  new QMediaPlayer(this);
    musicPlayList = new QMediaPlaylist(this);

    mediaPlayer->setPlaylist( musicPlayList );
    mediaPlayer->setNotifyInterval(1);
    musicPlayList->setPlaybackMode(QMediaPlaylist::Loop);

    connect(mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)), this,SLOT(slot_mediaStateChanged(QMediaPlayer::State)) );
    connect(mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(slot_changeHsbProgress(qint64)));
    connect(mediaPlayer,SIGNAL( positionChanged(qint64)) ,this,SLOT( slot_showPlayTime(qint64)));
    connect(mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(slot_durationChanged(qint64)));
    connect(mediaPlayer,SIGNAL(volumeChanged(int)),  this ,SLOT(slot_setVolSliderPos(int)));
    connect(mediaPlayer,SIGNAL(mutedChanged(bool)), this,SLOT( slot_mutedChanged(bool)));
    connect(mediaPlayer,SIGNAL( metaDataChanged()), this,SLOT(slot_MetaDataAvailableChanged()));
    connect( mediaPlayer,SIGNAL( currentMediaChanged(QMediaContent)),this,SLOT( set_lyricsBackground( QMediaContent)) );
    connect(musicPlayList,SIGNAL( currentIndexChanged(int)), this,SLOT(slot_changeListitemPos(const int &)));

}

void MainWindow::inIt_NetWork()
{

      //网络模块
    mNetWork = new MNetWorker();
    mNetWork->moveToThread( &childThread );

   connect(&childThread,SIGNAL( started()) , this,SLOT( slot_startSearch()));
   connect( mNetWork,SIGNAL( sig_searchFinished(SongInfo ,const QString)),this,SLOT(replySearchFinished(SongInfo ,const QString)));
   connect( mNetWork,SIGNAL( sig_taskFinished()), &childThread,SLOT(quit()));
   connect( mNetWork,SIGNAL( sig_findError(QString)),this,SLOT(slot_HandleNetError(QString)));

}


void MainWindow::inIt_BottomWidget()
{

    //设置按钮的悬浮提示
    ui->btn_play->setToolTip("播放");
    ui->toolBtn_preSong->setToolTip("上一首");
    ui->toolBtn_nextSong->setToolTip("下一首");
    ui->lbl_audioName->setText("");
    ui->tBtn_download->setToolTip("下载");
    ui->tBtn_cyclePlay->setToolTip("列表循环");
    ui->tBtn_volumn->setToolTip("静音");
    ui->tBtn_desktopIrics->setToolTip("打开桌面歌词");

    QFile qssFile(":/BottomWidStyle.qss");
    if( qssFile.open( QFile::ReadOnly)){
        ui->frame_bottom->setStyleSheet( qssFile.readAll() );
        qssFile.close();
    }
    ui->tBtn_volumn->installEventFilter( this );
    ui->verSlider_volumn->installEventFilter( this );
    ui->verSlider_volumn->hide();
    ui->verSlider_volumn->setValue( mediaPlayer->volume() );

    saveVolume = 0;

    connect(ui->btn_play,SIGNAL(clicked(bool)), this, SLOT(slot_btn_playClicked()));
    connect( ui->toolBtn_nextSong,SIGNAL(clicked(bool)), this, SLOT(slot_tBtn_nextSong()));
    connect(ui->toolBtn_preSong,SIGNAL(clicked(bool)), this, SLOT(slot_tBtn_preSong()));
    connect(ui->hSlider_progrss,SIGNAL(sliderMoved(int)), this,SLOT(slot_progress_sliderMoved(int)));
    connect(ui->verSlider_volumn,SIGNAL(sliderMoved(int)), this,SLOT(slot_volumn_sliderMoved(int)));
    connect(ui->tBtn_volumn,SIGNAL(clicked(bool)), this,SLOT(slot_tBtn_volumn_clicked()));
    connect(ui->tBtn_desktopIrics,SIGNAL(clicked(bool)), this,SLOT(slot_tBtn_desktopIricsClicked()));
    connect(ui->tBtn_download,SIGNAL(clicked(bool)), this,SLOT(slot_tBtn_downloadClicked()));

}

void MainWindow::inIt_RightWidget()
{
    if( (rightWidget != nullptr) && ( rightWidget_VLayout != nullptr)  ){
         delete rightWidget;
        rightWidget = nullptr;
        delete rightWidget_VLayout ;
        rightWidget_VLayout = nullptr;
    }
    rightWidget_VLayout  = new QVBoxLayout;
    rightWidget =  new RightWidget(this);
    rightWidget_VLayout->addWidget(rightWidget);
    rightWidget_VLayout->setSpacing(0);
    rightWidget_VLayout->setContentsMargins(0,0,0,0);
    ui->frame_rightWidget->setLayout( rightWidget_VLayout );
    connect( rightWidget, SIGNAL( sig_sendHistPlaylist(ListWidgetFlags ,const int &,  QList<SongItem *> &)), this,SLOT( slot_recvPlayList(ListWidgetFlags , const int & ,  QList<SongItem *> &)));
    connect( rightWidget, SIGNAL( sig_sendPlayHash(const int &, const  QList< QString > & )), this,SLOT(slot_recvPlaySong(const int &,  const  QList< QString > & )));
    connect( this, SIGNAL( sig_changeListWidCurItem(const ListWidgetFlags & ,const int &)), rightWidget,SLOT(slot_setListWidCurItem( const ListWidgetFlags &,const int &)));
}

void MainWindow::inIt_CentralWidget()
{
    if(  (nullptr == desklyrics )&&( nullptr == centralWidget_VLayout )){
        desklyrics = new DeskIyrics;
        centralWidget_VLayout = new QVBoxLayout( ui->frame_centralWidget );
        stackedLayout =  new QStackedLayout;

        stackedLayout->addWidget( desklyrics );
        centralWidget_VLayout->addLayout( stackedLayout);
        centralWidget_VLayout->setSpacing(0);
        centralWidget_VLayout->setMargin(0);
    }

    ui->frame_centralWidget->setLayout( centralWidget_VLayout );
   desklyrics->lrcLabel_setVisible( false );
   desklyrics->show();
   //设置默认背景图片
}

//网络搜索
void MainWindow::on_tBtn_search_clicked()
{
    if( ui->lineEdit_search->text().isEmpty() ){ return ;}

    if( childThread.isRunning() )
    {
        childThread.wait();
        childThread.terminate();
    }
      childThread.start();
}

void MainWindow::slot_startSearch()
{
    QString songStr = ui->lineEdit_search->text();
        songInfo.clearAllItems();  //仅仅是释放item 还不够，还有清除Qhash 中的数据才可以使用。
        mNetWork->searchSongInfo( songInfo,songStr );
}


void MainWindow::replySearchFinished(SongInfo songInfo ,const QString keyword)
{
    //每一次搜索完毕时，先释放上一次的空间再重新绘制窗口
    if(  nullptr != mCentralWidget  ){
        delete mCentralWidget;
        mCentralWidget = nullptr;
    }

    mCentralWidget = new MCentralWidget;
    connect( mCentralWidget ,SIGNAL(sig_sendAudioName(  QHash<QString,SongItem*> &)),  rightWidget  ,SLOT( slot_addSongToPlayList(QHash<QString,SongItem*> &)) );
    connect( mCentralWidget,SIGNAL(sig_playInstantly(ListWidgetFlags ,const int & ,QList<SongItem*>&)), this,SLOT( slot_recvPlayList(ListWidgetFlags ,const int & ,QList<SongItem*>&)));

    stackedLayout->addWidget( mCentralWidget );
    slot_tBtn_desktopIricsClicked();
    mCentralWidget->Show_SearchedSong( songInfo );
    mCentralWidget->setLabelText( keyword );
    mCentralWidget->show();
}


void MainWindow::slot_btn_playClicked()
{
    ui->verSlider_volumn->setValue( mediaPlayer->volume() );
    switch( mediaPlayer->state() )
    {
    case  QMediaPlayer::PlayingState :
        mediaPlayer->pause();
        break;
    default:
        mediaPlayer->play();
        break;
    }
}

void MainWindow::slot_mediaStateChanged(QMediaPlayer::State state)
{
    if( state == QMediaPlayer::PlayingState ){
        ui->btn_play->setToolTip( QStringLiteral("暂停"));
        ui->lbl_playTime->setText("00:00/00:00");
        ui->btn_play->setIcon(QIcon(":/bottomImage/pause.png"));
        ui->btn_play->setIconSize(QSize(36,36));
    }else{
        ui->btn_play->setToolTip( QStringLiteral("播放"));
        ui->btn_play->setIcon(QIcon(":/bottomImage/play.png"));
        ui->btn_play->setIconSize(QSize(36,36));
    }
}

void MainWindow::slot_tBtn_nextSong()
{
    musicPlayList->next();
    mediaPlayer->play();
}

void MainWindow::slot_tBtn_preSong()
{
    musicPlayList->previous();
    mediaPlayer->play();
}

void MainWindow:: slot_progress_sliderMoved(int position)
{
    mediaPlayer->setPosition(position);    //当进度条被拖动时，播放位置也能跟着走。
}

void MainWindow::slot_changeHsbProgress(qint64 progress)
{
    ui->hSlider_progrss->setValue( progress  );
    if( nullptr == desklyrics ){ return;}

    if(  lrcMap.contains( progress) ){
        desklyrics->lrcLabel_setVisible( true );
        QMap<qint64,QString>::const_iterator mapIter;
        mapIter = lrcMap.find( progress );
        lyricflag++;
        switch(lyricflag ){
        case 1:
            ++mapIter;
            desklyrics->lrcLabel_stopMask( 2 );
            desklyrics->lrcLabel_changText( lrcMap.value(progress) ,  mapIter.value() );
            desklyrics->lrcLabel_startMask(1, mapIter.key() - progress );
            break;
        case 2:
            ++mapIter;
            desklyrics->lrcLabel_stopMask(1);
            desklyrics->lrcLabel_changText( mapIter.value() , lrcMap.value( progress ) );
            desklyrics->lrcLabel_startMask(2,mapIter.key() - progress );
            break;
        default:
            ++mapIter;
            lyricflag = 1 ;
            desklyrics->lrcLabel_stopMask(2);
            desklyrics->lrcLabel_changText(  lrcMap.value(progress ) ,  mapIter.value());
            desklyrics->lrcLabel_startMask(1,mapIter.key() - progress );
            break;
        }
    }
}

void MainWindow::slot_durationChanged(qint64 duration)
{
    ui->hSlider_progrss->setRange(0,duration);
}

void MainWindow::slot_showPlayTime(qint64 playTime)
{
   QString time = formatTime( playTime/1000 );   //将毫秒转为秒。
   QString totalTime = formatTime( mediaPlayer->duration() / 1000 );
   ui->lbl_playTime->setText( time + "/" + totalTime ) ;
}

void MainWindow::slot_volumn_sliderMoved(int position)
{
    mediaPlayer->setVolume(position);
}

void MainWindow::slot_tBtn_volumn_clicked()
{
    //判断是否为静音，若为是则取消静音
    if( mediaPlayer->isMuted() ){
        mediaPlayer->setMuted(false);       //设为非静音
    }else{
        mediaPlayer->setMuted(true);   //设为静音
    }
}

void MainWindow::slot_setVolSliderPos(int position)
{
    ui->verSlider_volumn->setValue( position );
}

void MainWindow::slot_mutedChanged(bool muted)
{
    if( muted ){
        saveVolume = ui->verSlider_volumn->value();
        ui->verSlider_volumn->setValue(0);
        ui->tBtn_volumn->setToolTip("取消静音");
        ui->tBtn_volumn->setIcon(QIcon(":/bottomImage/muteVoice.png"));
        ui->tBtn_volumn->setIconSize(QSize(36,36));
    }else {
        ui->verSlider_volumn->setValue( saveVolume );
        ui->tBtn_volumn->setToolTip("静音");
        ui->tBtn_volumn->setIcon(QIcon(":/bottomImage/voice.png"));
        ui->tBtn_volumn->setIconSize(QSize(36,36));
    }
}

void MainWindow::slot_tBtn_downloadClicked()
{
    if( ui->lbl_audioName->text().isEmpty() ) {  return ;}
    QUrl curPlayUrl = mediaPlayer->currentMedia().canonicalUrl();
    if( curPlayUrl.isLocalFile() ) { return ;}
    if( playlistHash.isEmpty() ){ return ;}

    SongItem *curSongItem = playlistHash.value( curPlayUrl.toString() );
    if(nullptr == curSongItem ) { return ;}
    //每次关闭按钮时都自动释放对象的内存空间
    downloadForm  = new DownloadForm( this);
    connect(this,SIGNAL(sig_initDownForm(QString)),downloadForm,SLOT(slot_receiveAudioName(QString)));
    emit sig_initDownForm(  curSongItem->fileName  );
    downloadForm->show();    //改动：该句子在上一句
}


void MainWindow::slot_changePlayMode(int index)
{
    if(  (0 <= index) && (index <= 4))
    {
        musicPlayList->setPlaybackMode( QMediaPlaylist::PlaybackMode(index));
    }
}

void MainWindow::slot_setModeTip(const QString &toolTip)
{
    ui->tBtn_cyclePlay->setToolTip( toolTip );
}

void MainWindow::on_tBtn_cyclePlay_clicked()
{
    if( nullptr == playModeForm ){
        playModeForm = new PlayModeForm( this);  //播放模式选择类
        connect(playModeForm,&PlayModeForm::sig_playModeChanged, this, &MainWindow::slot_changePlayMode);
        connect(playModeForm, &PlayModeForm::sig_modeTipChanged, this,&MainWindow::slot_setModeTip);
        playModeForm->installEventFilter( this );
        playModeForm->setGeometry(820, 529,80,121);
    };
    playModeForm->show();
}

void MainWindow::slot_tBtn_desktopIricsClicked()
{
    if( nullptr ==  mCentralWidget ){ return ;}
    int nCount = stackedLayout->count();
    int nIndex = stackedLayout->currentIndex();
    // 获取下一个需要显示的页面索引
    ++nIndex;
    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount){
        nIndex = 0;
    }
    stackedLayout->setCurrentIndex(nIndex);
}

void MainWindow::slot_recvPlayList(ListWidgetFlags listWidgetFlags , const int & row,  QList<SongItem *> &playlist)  //来自历史列表的数据，来自中心部件播放的数据
{
    this->savePlayRow =  row;
    this->listWidgetFlags = listWidgetFlags;
    MNetWorker  *network =  new MNetWorker;
    connect( network,&MNetWorker::sig_taskFinished, [network]() mutable {   delete network; network = nullptr;    });
    connect( network,SIGNAL( sig_getUrlFinished( const QList<SongItem *> & )), this, SLOT( slot_setMediaPlayList(  const QList<SongItem *> &  )));
    network->slot_getPlayUrl( playlist );

}

void MainWindow::slot_setMediaPlayList(    const QList<SongItem *> & playlist )  //来自历史数据表格的数据,来自中心部件播放的数据
{

    if( playlist.isEmpty() ) { return ;}

    if( mediaPlayer->state() == QMediaPlayer::PlayingState){
        mediaPlayer->stop();
    }

    musicPlayList->clear();
    playlistHash.clear();
    foreach (SongItem *songItem, playlist) {

       if( ! songItem->play_url.isEmpty() ){
                playlistHash.insert( songItem->play_url, songItem);   //改动
                 musicPlayList->addMedia(QMediaContent( QUrl( songItem->play_url)));
       }
    }
    musicPlayList->setCurrentIndex( this->savePlayRow );
    mediaPlayer->play();
}


void MainWindow::slot_MediaStatusChange(QMediaPlayer::MediaStatus status)
{
    if( status == QMediaPlayer::EndOfMedia ){
        desklyrics->showPicTimer_stop();
    }
}

void MainWindow::slot_recvPlaySong(const int & row,  const  QList< QString > &nativePlayList ) //来自本地音乐的数据列表
{
    if( nativePlayList.isEmpty() ){ return ;}
    if( mediaPlayer->state() == QMediaPlayer::PlayingState){
        mediaPlayer->stop();
    }

   musicPlayList->clear();
   playlistHash.clear();
   foreach (QString fileName, nativePlayList) {
       if( ! fileName.isEmpty() ){
           musicPlayList->addMedia(QUrl::fromLocalFile( fileName));
       }
   }

    this->listWidgetFlags = update_nativeListWid;
    musicPlayList->setCurrentIndex( row );
    mediaPlayer->play();

}

void MainWindow::slot_MetaDataAvailableChanged()
{

    QString title =" ";
    QUrl  curPlayUrl = mediaPlayer->currentMedia().canonicalUrl();
    if( curPlayUrl.isLocalFile() ){
        title = curPlayUrl.fileName();
        title = title.left( title.length() -4 );
        lrcMap.clear();
    }else {
        if( playlistHash.isEmpty() ){ return ;}
        SongItem *curSongItem = playlistHash.value( curPlayUrl.toString() );
        if(nullptr == curSongItem ) { return ;}
        title = curSongItem->fileName;    
        lrcMap.clear();  //获取歌词
        splitString_Iric( curSongItem->lyrics,lrcMap);
    }
    desklyrics->lrcLabel_setVisible( false );
    ui->lbl_audioName->setText( title );
}


void MainWindow::set_lyricsBackground(const QMediaContent &media)
{
    QUrl currentPlayUrl =  media.canonicalUrl();
    if( currentPlayUrl.isLocalFile() ){ return ;}
    if( playlistHash.isEmpty() ){ return ;}
    SongItem *curSongItem = playlistHash.value( currentPlayUrl.toString() );
    if(nullptr == curSongItem ) { return ;}
    if( curSongItem->singerName.isEmpty() ) { return;}
    QByteArray tempByteArray = mNetWork->reuqestBgPic( curSongItem->singerName );
    QPixmap pixmap;
    if ( pixmap.loadFromData( tempByteArray, "JPG"))
    {
        pixmap.scaled(ui->frame->width(), ui->frame->height(),Qt::IgnoreAspectRatio);
        QPalette palette = ui->frame->palette();
        ui->frame->setAutoFillBackground( true );
        palette.setBrush(QPalette::Background,QBrush( QPixmap( pixmap)));
        ui->frame->setPalette( palette );
    }
}

void MainWindow::slot_changeListitemPos(const int  &index)
{
   emit sig_changeListWidCurItem( listWidgetFlags, index );
}


void MainWindow::slot_HandleNetError(QString NetError)
{
    if( NetError.trimmed() != QString("NoError").trimmed() ){
          QMessageBox::information( this,QStringLiteral("网络似乎出现了错误哦"),QString("%1").arg(NetError),QMessageBox::Ok);
    }
}

//设置主窗体无边框并实现移动功能
// 实现移动功能需要函数：判断 左键是否按下，左键按下，左键是否释放，鼠标移动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
     if( event->button() == Qt::LeftButton )
     {
         mMousePoint = event->globalPos() - this->pos();  //在移动时鼠标相对于应用程序窗口的位置是不会变的
         mDragWindow = true;
     }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
      //当鼠标释放时，将移动标志置为不可移动
    Q_UNUSED(event);
    mDragWindow = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //鼠标在移动窗体的时候是没有按钮被按下的,但实际时左键按下并移动
    if(  mDragWindow  )
    {
        this->move( event->globalPos() - mMousePoint );
    }
}

void MainWindow::on_tBtn_miniMum_clicked()
{
   this->hide();
   msystemTray = new MSystemTray;
   connect( msystemTray, &MSystemTray::sig_showMainActionClicked, this,&MainWindow::slot_showMainWindow);
   connect( msystemTray,&MSystemTray::sig_exitAppActionClicked, this,&MainWindow::on_tBtn_close_clicked);
   connect(msystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
   msystemTray->show();
}

void MainWindow::slot_showMainWindow()
{
    this->show();
}

void MainWindow::on_tBtn_close_clicked()
{
    if( this->close() ){
          qApp->exit(0);
    }

}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::DoubleClick:
        msystemTray->showMessage(QObject::tr("Message Title"),
                                 QObject::tr("欢迎使用此程序"),
                                 QSystemTrayIcon::Information,
                                 1000);
        break;
    case QSystemTrayIcon::Trigger:
        if( ! this->isVisible() ){
            this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
            this->show();
        }else{
            this->hide();
        }
        break;
    default:
        break;
    }
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if( playModeForm == watched ){
        if( QEvent::Leave ==  event->type() ){
            playModeForm->hide();
            return true;
        }
    }

    if( QEvent::Enter == event->type()  ){
        if ( ui->tBtn_volumn == watched){
            ui->verSlider_volumn->show();
          }
    }

    if( ui->verSlider_volumn == watched ){
        if( QEvent::Leave  == event->type() ){
            ui->verSlider_volumn->hide();
        }
    }
    return QMainWindow::eventFilter( watched, event);
}


MainWindow::~MainWindow()
{
    freeSongitem( this->songInfo );
    delete mediaPlayer;
    delete musicPlayList;
    if(desklyrics != nullptr ){
        delete desklyrics;
        desklyrics = nullptr;
    }

    if( mNetWork != nullptr ){
        delete mNetWork;
        mNetWork = nullptr;
    }
    if( downloadForm != nullptr ){
        delete downloadForm;
        downloadForm = nullptr;
    }
    if( mCentralWidget != nullptr){
        delete mCentralWidget;
        mCentralWidget = nullptr;
    }
    if( rightWidget != nullptr){
        delete rightWidget;
        rightWidget = nullptr;
    }
    if( rightWidget_VLayout != nullptr){
        delete rightWidget_VLayout;
        rightWidget_VLayout= nullptr;
    }
    if( playModeForm != nullptr ){
       delete playModeForm;
       playModeForm = nullptr;
    }

    if( msystemTray != nullptr){
        delete msystemTray;
        msystemTray= nullptr;
    }

  if( centralWidget_VLayout != nullptr){
         delete centralWidget_VLayout;
      centralWidget_VLayout = nullptr;
  }

//  if( stackedLayout != nullptr){
//       delete stackedLayout;
//      stackedLayout = nullptr;
//  }

    delete ui;
}

void MainWindow::freeSongitem(SongInfo &songInfo)
{
    songInfo.clearAllItems();
}

