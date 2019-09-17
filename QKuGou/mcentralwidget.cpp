#include "mcentralwidget.h"
#include "ui_mcentralwidget.h"

#include<QPushButton>
#include<QDebug>
#include<QAbstractButton>
#include<QRegExp>
MCentralWidget::MCentralWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MCentralWidget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose); //继承Qwidget 窗体关闭时自动销毁
    this->setWindowFlags( Qt::FramelessWindowHint );



    lrcWidget = false;
    mCheckHeader =  new SCheckBoxHeaderView(0,Qt::Horizontal,ui->tableWid_song);
    ui->tableWid_song->setHorizontalHeader(mCheckHeader);


   ui->tableWid_song->setColumnCount( 4 );
   ui->tableWid_song->setColumnWidth(0,450);
   ui->tableWid_song->setColumnWidth(1,120);
   ui->tableWid_song->setColumnWidth(2,50);
   ui->tableWid_song->setColumnWidth(3,30);

    ui->tableWid_song->verticalHeader()->setVisible(false);
    ui->tableWid_song->horizontalHeader()->setVisible(true);
     ui->tableWid_song->setShowGrid( false );
     ui->tableWid_song->setTextElideMode(Qt::ElideRight);
     ui->tableWid_song->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWid_song->setEditTriggers(QAbstractItemView::NoEditTriggers  );

    QFile qssFile(":/CentralWidStyle.qss");
    if( qssFile.open(QFile::ReadOnly)){
        this->setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    QFile scrollBarStyle(":/ScrollBar.qss");
    if( scrollBarStyle.open(QFile::ReadOnly)){
        ui->tableWid_song->verticalScrollBar()->setStyleSheet(scrollBarStyle.readAll());
        scrollBarStyle.close();
    }
    connect( ui->tableWid_song,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT( slotHandle_ItemClicked(QTableWidgetItem*)));
    connect( ui->tableWid_song,SIGNAL(itemDoubleClicked(QTableWidgetItem*)) , this,SLOT(  slotHandle_ItemDoubleClicked(QTableWidgetItem*)));
    connect(mCheckHeader, SIGNAL(checkStausChange(bool)),this,SLOT(slotHandle_checkedAll(bool)));
}

//显示搜索结果
void MCentralWidget::Show_SearchedSong( SongInfo  &songInfo)
{
    QHash<QString,SongItem *>  hash = songInfo.getSongQHash();
   this->searchQHash.clear();
    this->searchQHash =  hash;

      //获取行数
     int rowNumer  = hash.size();
     int i = 0;
    ui->tableWid_song->clearContents();
    ui->tableWid_song->setRowCount( rowNumer );
    rowNumer = 0 ;


    foreach (SongItem *item, hash)
    {
        QTableWidgetItem* tableItem[ 3 ];
        QPushButton* btn_play  =  new QPushButton(this);


       connect(btn_play,SIGNAL(clicked(bool)),this,SLOT(slotHandle_btnClicked()));

        tableItem[0]  =  new QTableWidgetItem;
        tableItem[0]->setText(  item->fileName );

        tableItem[0]->setCheckState(Qt::Unchecked);
        tableItem[0]->setTextAlignment(Qt::AlignCenter);

        tableItem[1]  =  new QTableWidgetItem(QString("%1").arg( item->albumName));
        tableItem[1]->setTextAlignment(Qt::AlignCenter);


        tableItem[2]  =  new QTableWidgetItem(tr("%1").arg( item->playTime));
        tableItem[2]->setText(tr("%1").arg( item->playTime ) );
        tableItem[2]->setTextAlignment(Qt::AlignCenter);

        //添加每一行的前三列数据
        for( i =0 ; i<3; i++){
            ui->tableWid_song->setItem(rowNumer, i ,tableItem[i]);
        }

        //单独添加一个按钮到第三列
        btn_play->setFixedSize(QSize(30,30));
        btn_play->setToolTip(QString("播放"));
        btn_play->setIcon( QIcon(":/bottomImage/play.png"));
        btn_play->setIconSize(QSize(30,30));
        btn_play->setStyleSheet("QPushButton{ border:none; background:transparent;}");
        ui->tableWid_song->setCellWidget( rowNumer,3,btn_play);
       rowNumer++;
       i = 0;
     }    

    ui->tableWid_song->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWid_song->setCurrentItem(0,QItemSelectionModel::ClearAndSelect);
}

void MCentralWidget::setLabelText( QString keyword)
{

    keyword = tr("<font style = 'font-family:MicrosoftYaHei ; size: 14px ; color: blue;'><b> <i>%1</i> </b></font>").arg( keyword );
    keyword = "搜索到 "+ tr("%1").arg( keyword ) +" 的相关歌曲";
    ui->lbl_keyWord->setText( keyword  );
}

void MCentralWidget::slot_updateCentralWidget()
{
    if(  ! lrcWidget ){
        ui->stackedWidget->setCurrentIndex( 1 );
    }else{
        ui->stackedWidget->setCurrentIndex( 0 );
    }
    lrcWidget = ! lrcWidget;
}

//某一行被单击
void MCentralWidget::slotHandle_ItemClicked(QTableWidgetItem *item)
{
    if( item != nullptr ){
        int rowId = item->row();
        QTableWidgetItem  * tempItem = ui->tableWid_song->item( rowId,0);
        if( tempItem->checkState() == Qt::Checked ){
            tempItem->setCheckState(Qt::Unchecked);
             mCheckHeader->setHeaderItemChecked();
        }else{
            tempItem->setCheckState(Qt::Checked);
        }
    }
}


//全部选中
void MCentralWidget::slotHandle_checkedAll(bool checkAll)
{
    if( checkAll ){
          checkedAll(ui->tableWid_song, Qt::Checked);

    }else{
       checkedAll(ui->tableWid_song, Qt::Unchecked);
    }
}


void MCentralWidget::show_lrcWidget()
{

}


void MCentralWidget::checkedAll(QTableWidget *tableWidget, Qt::CheckState checkState)
{
    int i = 0;
    int rowCnt = tableWidget->rowCount();
    QTableWidgetItem * item = nullptr;
    for( i = 0 ; i< rowCnt ; ++ i){
         item = ui->tableWid_song->item(i,0);
         item->setCheckState(checkState);
    }
}

//某一行被双击要添加，并且要播放
void MCentralWidget::slotHandle_ItemDoubleClicked(QTableWidgetItem *item)
{

    if( nullptr !=  item ){
        QTableWidgetItem * tempItem =  ui->tableWid_song->item( item->row(), 0);

        QString  fileName = tempItem->text() ;
        qDebug()<<"双击的行"<<fileName;
         SongItem * songItem = searchQHash.value(fileName  );
          sendPlaylist.clear();
          sendDataQHash.clear();
          sendDataQHash.insert( fileName, songItem );
         sendPlaylist.append( songItem );
        emit sig_sendAudioName( sendDataQHash );
        emit  sig_playInstantly(update_Null,0,sendPlaylist );  //在主界面更新其列表
    }
}

//播放被点击，只播放不添加
void MCentralWidget::slotHandle_btnClicked( )
{
      QPushButton *tempBtn = qobject_cast<QPushButton *> ( sender() );  //基类- 派生类:向下转型避免二义性
    int x = tempBtn->frameGeometry().x();
    int y = tempBtn->frameGeometry().y();

   QModelIndex modelIndex = ui->tableWid_song->indexAt(QPoint(x,y));


   QTableWidgetItem *tempItem = ui->tableWid_song->item( modelIndex.row() , 0);
         QString  fileName = tempItem->text() ;
         SongItem * songItem = searchQHash.value(  fileName  );
         sendPlaylist.clear();
         sendPlaylist.append( songItem );
         emit  sig_playInstantly(update_Null,0, sendPlaylist );
}

//播放选中,添加和播放
void MCentralWidget::on_tBtn_playSelection_clicked()
{
     sendPlaylist.clear();
     sendDataQHash.clear();
    int  rowNum = ui->tableWid_song->rowCount();
    int i = 0;
    QTableWidgetItem * checkedItem = nullptr;

    for(i=0;i<rowNum;i++ ){
        checkedItem = ui->tableWid_song->item(i,0);
        if( checkedItem->checkState() == Qt::Checked ){
            QString  fileName = checkedItem->text() ;
            SongItem * songItem = searchQHash.value( fileName  );
            sendDataQHash.insert( fileName ,songItem );
            sendPlaylist.append( songItem );
        }
    }
    emit sig_sendAudioName( sendDataQHash );
    emit sig_playInstantly(update_Null,0, sendPlaylist  );
}


//添加到播放列表，只添加不播放
void MCentralWidget::on_btn_addPlayList_clicked()
{
    sendDataQHash.clear();
    int  rowNum = ui->tableWid_song->rowCount();
    int i = 0;
    QTableWidgetItem * checkedItem = nullptr;

    for(i=0;i<rowNum;i++ ){
        checkedItem = ui->tableWid_song->item(i,0);
        if( checkedItem->checkState() == Qt::Checked ){
            QString  fileName = checkedItem->text() ;
            SongItem * songItem = searchQHash.value(fileName  );
            sendDataQHash.insert( fileName ,songItem );
        }
    }
    emit sig_sendAudioName( sendDataQHash );  //不播放
}

MCentralWidget::~MCentralWidget()
{
   if(mCheckHeader != nullptr ){
           mCheckHeader->deleteLater();
   }
    delete ui;
}
