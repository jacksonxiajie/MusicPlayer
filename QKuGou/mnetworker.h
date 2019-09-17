
#ifndef MNETWORKER_H
#define MNETWORKER_H

/*工程文件里添加 头文件*/

#include <QObject>
#include<QMap>
#include<QString>

#include<QNetworkReply>
#include<QDir>
#include<QImage>
#include<QEventLoop>

#include "downinfo.h"
#include"songinfo.h"


class QNetworkAccessManager;
class QNetworkRequest;

typedef QHash<QString,SongItem *>  PlayHash;  //取别名

class MNetWorker: public QObject
{
    Q_OBJECT
public:
     explicit MNetWorker(QObject * parent =0);
    ~MNetWorker();
     void slot_getPlayInfo(DownInfo &downloadInfo ,const QString &audio_name);
    void downloadFile(const QString &http,  const QString &fileName,const QString &saveDir); //QString URL

     QString getSelectedUrl( QString hash, int albumID );

     QByteArray reuqestBgPic(const QString &author);  //请求歌手背景图片
     const QImage &BgWhiteChange(QImage &image , int brightness);
     void  searchSongInfo(SongInfo &songInfo,QString  keyword );

public slots:
     void slot_getPlayUrl(  QList<SongItem *> & playlist );

private slots:
   void splitString_Iric( const QString & IricStr,  QMap<qint64,QString> &map);
   void slot_showErrorInfo( QNetworkReply::NetworkError   error);
   void getLrc( SongItem * songItem, QEventLoop  &eventLoop , QNetworkAccessManager *network_manager);

signals:
     retResult( QString result);
    void sig_taskFinished();
    void sig_findError( QString errorStr);
    void sig_searchFinished(const SongInfo &songInfo , const QString keyword);
    void  sig_getUrlFinished( const QList<SongItem *> &playlist  );
    void sig_downFinish(const QString &fileName);

private:
    QString formatTime(qint64 time);
    QMap<qint64,QString>   lyricMap;
    QString fileet;
};

#endif // MNETWORKER_H
