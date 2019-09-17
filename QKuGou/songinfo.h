#ifndef SONGINFO_H
#define SONGINFO_H

#include<QString>
#include<QHash>

typedef enum
{
    update_nativeListWid,
    update_favorListwid,
    update_histListwid,
    update_Null,
}ListWidgetFlags;

struct  SongItem
{
    QString fileName;
    QString albumName;  //中心控件需要显示专辑名
    QString  duration;
    QString hash;
    QString albumID;
    QString play_url;
    QString playTime;
    QString singerName; //获取歌手写真要用
    QString lyrics;
};

class SongInfo
{
public:
    SongInfo();
    ~SongInfo();
    void addSongItem(QString fileName, QString albumName,QString  playTime,QString hash,QString albumID,QString singerName, QString duration);
   QHash<QString,SongItem *> getSongQHash( );
    void clearAllItems();

private:
    QHash<QString,SongItem *> songQHash;
};

#endif // SONGINFO_H
