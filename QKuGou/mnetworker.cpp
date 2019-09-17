#include "mnetworker.h"


#include<QtNetwork/QNetworkRequest>
#include<QNetworkAccessManager>

#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonParseError>
#include<QJsonValue>

#include<QVariant>
#include<QByteArray>
#include<QDebug>
#include<QStringList>
#include<QTextCodec>

#include<QFile>
#include<QUrl>
#include<QThread>
#include<QRegExp>


MNetWorker::MNetWorker(QObject *parent):
    QObject(parent)
{

      qRegisterMetaType<PlayHash>("PlayHash");
}

void MNetWorker::searchSongInfo( SongInfo &songInfo,QString  keyword)
{     

   QString KGAPI_ReqStr1 = QString::fromUtf8("http://songsearch.kugou.com/song_search_v2?keyword="
                           "%1&page=1&pagesize=20&userid=-1&clientver=&"
                        " platform=WebFilter&tag=em&filter=2"
                      " &iscorrection=1&privilege_filter=0").arg( keyword);

    QNetworkAccessManager *network_manager = new QNetworkAccessManager;
    QEventLoop eventLoop;
    QNetworkReply  *reply = network_manager->get(QNetworkRequest(QUrl( KGAPI_ReqStr1 )));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(slot_showErrorInfo(QNetworkReply::NetworkError)));
    connect(reply,SIGNAL(finished()),&eventLoop,SLOT(quit()));
    eventLoop.exec();

    if(reply->error() != QNetworkReply::NoError){
        emit sig_taskFinished();
        return ;
    }

    QByteArray bytes = reply->readAll();
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(bytes, &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootObj = parse_doucment.object();
            QJsonValue valuedata = rootObj.value("data");
            QJsonObject valuedataObject = valuedata.toObject();
            QJsonValue valueArray = valuedataObject.value("lists");
            QJsonArray array = valueArray.toArray();
            int size = array.size();
            for(int i = 0;i < size;i++)
            {
                QJsonObject object = array.at(i).toObject();
                QString fileName = object.value("FileName").toString();
                fileName.replace( QRegExp("<[^>]*>") ," ");              
                QString AlbumName =   object.value("AlbumName").toString();
                 AlbumName.replace( QRegExp("<[^>]*>") ," ");

                int duration = object.value("Duration").toInt();
                QString totalTime = formatTime( duration );
                songInfo.addSongItem( fileName,
                                       AlbumName  ,
                                      totalTime,
                                      object.value("FileHash").toString(),
                                       object.value("AlbumID").toString(),//AlbumID
                                      object.value("SingerName").toString(),
                                      QString::number(duration));
            }        
        }
        emit sig_searchFinished(songInfo , keyword );
   }

    delete network_manager;
    network_manager = nullptr;

    emit sig_taskFinished();
}

//获取播放列表中即将播放歌曲的url
void MNetWorker::slot_getPlayUrl( QList< SongItem *> & playlist )   //QString hash, QString albumID
{
      QNetworkAccessManager *networkManager =  new QNetworkAccessManager ;
      QEventLoop eventLoop ;
      foreach (SongItem * songItem, playlist) {
          bool  playUrlBool =   songItem->play_url.isEmpty() ;;
          bool albumIDBool =   songItem->albumID.isEmpty();;
          bool hashBool = songItem->hash.isEmpty();;
        if(  playUrlBool &&   ( ! albumIDBool )   &&   ( ! hashBool )){

            QString url = QString("http://m.kugou.com/app/i/getSongInfo.php?cmd=playInfo&hash=%1").arg( songItem->hash);
            QNetworkReply *reply = networkManager->get( QNetworkRequest(QUrl( url )) );
            connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(slot_showErrorInfo(QNetworkReply::NetworkError)));
            connect(reply,SIGNAL(finished()),&eventLoop,SLOT(quit()) );
            eventLoop.exec();
            if( reply->error() != QNetworkReply::NoError){  return ;}

            int nHttpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if( nHttpCode != 200 ){ return ;}
            QByteArray  byteArray = reply->readAll();
            QJsonParseError json_error;
            QJsonDocument  jsonDoc=QJsonDocument::fromJson(byteArray,&json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if(jsonDoc.isObject())
                {
                    QJsonObject rootObj = jsonDoc.object();
                    QJsonValue valuedata = rootObj.take("url");
                    songItem->play_url = valuedata.toString();
                }
            }
            reply->deleteLater();
            reply =nullptr;
        }

        if( (! songItem->hash.isEmpty() ) && ( ! songItem->duration.isEmpty())){
               getLrc( songItem,eventLoop, networkManager );
        }
    }
    emit sig_getUrlFinished( playlist );
    emit sig_taskFinished();
    delete networkManager;
    networkManager = nullptr;
}

void MNetWorker::getLrc( SongItem * songItem, QEventLoop  &eventLoop ,QNetworkAccessManager *network_manager)
{
    if(  ! songItem->lyrics.isEmpty()  ){  return ;}
    QString lyricUrl = QString("http://m.kugou.com/app/i/krc.php?cmd=100&hash=%1&timelength=%2").arg( songItem->hash).arg( songItem->duration);
    QNetworkReply  *reply = network_manager->get(QNetworkRequest(QUrl( lyricUrl )));
    connect(reply,SIGNAL(finished()),&eventLoop,SLOT(quit()));
    eventLoop.exec();
    int nHttpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if( nHttpCode != 200 ){ return ;}
    if( reply->error() != QNetworkReply::NoError){
        reply->deleteLater();
        reply = Q_NULLPTR;
        return ;
    }

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    songItem->lyrics  =  codec->toUnicode(  reply->readAll() );
    reply->deleteLater();
    reply = Q_NULLPTR;
}

void MNetWorker::downloadFile( const QString &http,  const QString &fileName,const QString &saveDir )
{
    QNetworkAccessManager networkManager;
    QEventLoop loop(this);    //给出事件循环，保证请求的数据能传输完成后再进行后续操作，相当于将事件阻塞在此处
    QNetworkReply  * reply = networkManager.get( QNetworkRequest(QUrl(http)));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(slot_showErrorInfo(QNetworkReply::NetworkError)));
    connect(reply,SIGNAL(finished()),&loop,SLOT(quit()));   //当所有数据都接收后，不再有新的数据待读取时，离开事件循环，继续后面的工作。
    loop.exec();
    if( reply->error() != QNetworkReply::NoError)
    {
        reply->deleteLater();
        reply =nullptr;
        return ;
    }
    QByteArray mp3Data =   reply->readAll();
    QDir directory(saveDir);
    fileet = directory.filePath( fileName );
    QFile file( fileet );
    if( file.open(QIODevice::WriteOnly))
    {
        if ( -1 == file.write( mp3Data ) ){
            file.remove( fileet );
            fileet.clear();
        }
        file.close();
        emit sig_downFinish( fileet );
    }
    reply->deleteLater();
    reply =nullptr;
}


//根据当前播放歌曲的audio_name 获取下载歌曲所需要的信息
void MNetWorker::slot_getPlayInfo( DownInfo &downloadInfo,const QString &audio_name )
{

   QString  httpUrl = QString("http://songsearch.kugou.com/song_search_v2?keyword=%1&page=1&pagesize=1&userid=-1&clientver=&platform=WebFilter&tag=em&filter=2&iscorrection=1&privilege_filter=0").arg(audio_name);
   QNetworkAccessManager networkManager;
   QEventLoop  eventLoop;
   QNetworkReply  *reply = networkManager.get(QNetworkRequest(QUrl( httpUrl )));

    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(slot_showErrorInfo(QNetworkReply::NetworkError)));
    connect(reply,SIGNAL(finished()),&eventLoop,SLOT(quit()));
   eventLoop.exec();

   if(  QNetworkReply::NoError  != reply->error() )
   {
       emit sig_taskFinished();
       return ;
   }

   //解析数据结构得到下载相关的信息

   QByteArray byteArray = reply->readAll();
    QJsonDocument doc=QJsonDocument::fromJson(byteArray);
    QJsonObject objTemp= doc.object();
    objTemp= objTemp.value("data").toObject();
    QJsonArray array0= objTemp.value("lists").toArray();
    objTemp= array0.at(0).toObject();    //获取指定的数据

    downloadInfo.setNormalInfo( objTemp.value("Price").toInt(),
                                 objTemp.value("FileHash").toString(),
                                 objTemp.value("FileSize").toInt(),
                                 objTemp.value("Bitrate").toInt(),
                                 objTemp.value("ExtName").toString(),
                                objTemp.value("AlbumID").toInt());

    downloadInfo.setHQInfo( objTemp.value("HQPrice").toInt(),
                             objTemp.value("HQFileHash").toString(),
                             objTemp.value("HQFileSize").toInt(),
                             objTemp.value("HQBitrate").toInt(),
                             objTemp.value("HQExtName").toString(),
                               objTemp.value("AlbumID").toInt());

    downloadInfo.setSQInfo( objTemp.value("SQPrice").toInt(),
                                 objTemp.value("SQFileHash").toString(),
                                 objTemp.value("SQFileSize").toInt(),
                                 objTemp.value("SQBitrate").toInt(),
                                  objTemp.value("SQExtName").toString() ,
                                objTemp.value("AlbumID").toInt());

   reply->deleteLater();
   reply =nullptr;

}


//根据选择的音质hash 和 albumId，获取其不同音质下的play_url,然后启动下载
QString MNetWorker::getSelectedUrl(QString hash, int albumID)
{
    Q_UNUSED(albumID);
    QString url = QString("http://m.kugou.com/app/i/getSongInfo.php?cmd=playInfo&hash=%1").arg( hash );
    QNetworkAccessManager networkManager ;
    QNetworkReply *reply = networkManager.get(QNetworkRequest(QUrl( url )));
    QEventLoop eventLoop ;
    connect(reply,SIGNAL(finished()),&eventLoop,SLOT(quit()) );
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(slot_showErrorInfo(QNetworkReply::NetworkError)));
    eventLoop.exec();

    QString play_url ="";
    if(reply->error() != QNetworkReply::NoError){
        emit sig_taskFinished();
        return  play_url;
    }

   QByteArray  byteArray = reply->readAll();
   QJsonParseError json_error;
    QJsonDocument  jsonDoc=QJsonDocument::fromJson(byteArray,&json_error);
   if(json_error.error == QJsonParseError::NoError)
   {
       if(jsonDoc.isObject())
       {
           QJsonObject rootObj = jsonDoc.object();
           QJsonValue valuedata = rootObj.take("url");
           play_url = valuedata.toString();
       }
    }
   reply->deleteLater();
   reply =nullptr;
   return play_url;
}


const QImage &MNetWorker::BgWhiteChange(QImage &image , int brightness)
    {
        uchar *line =image.scanLine(0);
            uchar *pixel = line;
            for (int y = 0; y < image.height(); ++y)
            {
                pixel = line;
                for (int x = 0; x < image.width(); ++x)
                {
                    *pixel = qBound(0, *pixel + brightness, 255);
                    *(pixel + 1) = qBound(0, *(pixel + 1) + brightness, 255);
                    *(pixel + 2) = qBound(0, *(pixel + 2) + brightness, 255);
                    pixel += 4;
                }
                line += image.bytesPerLine();
            }
            return image;
}


 QByteArray  MNetWorker::reuqestBgPic(const QString &author)
{
     QString strTemp( "http://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&id=0&name=%1&from=pc&json=1&version=1&width=1920&height=1080");
     strTemp=strTemp.arg(author);

     QNetworkAccessManager manger;
     QNetworkReply * reply= manger.get(QNetworkRequest(QUrl(strTemp)));

     QEventLoop loop;
     connect(&manger,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
     connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT( slot_showErrorInfo(QNetworkReply::NetworkError)));
     loop.exec();


     QByteArray bytTemp=reply->readAll();
     reply->deleteLater();
     reply = nullptr;

     QJsonDocument doc=QJsonDocument::fromJson(bytTemp);
     QJsonObject obj=doc.object();
     QJsonArray array=obj.value("array").toArray();

     if( 0 != array.count() ){
         obj= array.at(0).toObject();  //只取一个数据
         strTemp=obj.value("bkurl").toString();
         if( ! strTemp.isEmpty() ){
             reply=manger.get(QNetworkRequest(strTemp));
             connect(&manger,SIGNAL(finished(QNetworkReply*)),&loop,SLOT(quit()));
             loop.exec();
             if( reply->error() ==  QNetworkReply::NoError )
             {
                 bytTemp= reply->readAll();
                 return bytTemp;
             }
         }
     }
 }


void MNetWorker::slot_showErrorInfo(QNetworkReply::NetworkError error)
{
   QString errorStr;
    switch (  error ) {
    case QNetworkReply::NoError:
        errorStr = QStringLiteral("NoError");
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::RemoteHostClosedError:
        errorStr = QStringLiteral("服务器拒绝链接!");
        break;
    case QNetworkReply::HostNotFoundError :
        errorStr = QStringLiteral("未发现服务器!");
        break;
    case QNetworkReply::TimeoutError:
        errorStr = QStringLiteral("链接超时！");
        break;
    case QNetworkReply::OperationCanceledError:
        errorStr = QStringLiteral("服务器断开链接，数据传输中断！");
        break;
    case QNetworkReply::SslHandshakeFailedError:
        errorStr = QStringLiteral("访问失败：403 ！");
        break;
    case QNetworkReply::TemporaryNetworkFailureError:
        errorStr = QStringLiteral("未发现请求的数据: 404!") ;
        break;
    default:
        errorStr = QStringLiteral("未知错误！");
        break;
    }

   //线程里面不能使用GUI 函数
     emit sig_findError(errorStr );
}



QString MNetWorker::formatTime(qint64 time)
{
    int  minute = time / 60;
    int  second = time % 60;
    int hour = 0;
    if( minute >= 60 )
    {
        hour = minute / 60;
        minute = minute % 60;
    };

   QString strminute =  ( minute > 9 )?QString::number( minute ): "0" + QString::number( minute );
   QString strsecond =  ( second > 9 )?QString::number( second ): "0" + QString::number( second );
   if( hour > 0  )
   {
        QString strhour =  ( hour > 9 )?QString::number(hour):"0" + QString::number(  hour );
         return  (strhour + ":"+ strminute + ":" + strsecond);
   } else {
       return (strminute+":"+strsecond);
   }
}

void MNetWorker::splitString_Iric( const QString & IricStr, QMap<qint64,QString> &map)
{

    QStringList  iricStrList =  IricStr.split("\r\n");
    qint64 msec = 0;
    map.clear();
    foreach(QString text ,  iricStrList )
    {
        if ( ! text.isEmpty() )
        {
            QString timeStr = text.left(10);    //提取时间
            QString lyricStr = text.right( text.length() - 10 ); //提取歌词
            if(   timeStr.isEmpty()  &&  lyricStr.isEmpty()  )  {  return ;}
            QTime  time = QTime::fromString(timeStr,   "[m:s.z]");
            if(  time.isValid()  )    // valid 表示有效的，invalid 无效的
            {
                msec = ( time.hour() * 3600+ time.minute() * 60 + time.second() ) *1000;
                msec += ( time.msec() / 10 );  //不用计算小时的，因为在网页中的歌词没有来自小时的，除以10 的原因是转化为time 后会在后面默认加上一个0
                map.insert( msec , lyricStr );
            }
        }
    }
}


MNetWorker::~MNetWorker()
{

}





