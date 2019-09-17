#include "songinfo.h"
#include<QDebug>
#include<QThread>

SongInfo::SongInfo()
{

}

void SongInfo::addSongItem(QString fileName, QString albumName,QString  playTime,QString hash,QString albumID,QString singerName,QString durtion)
{

     SongItem *songItem = new SongItem;
  songItem->fileName = fileName;
   songItem->albumName =albumName;
   songItem->playTime = playTime;
  songItem->hash =hash;
   songItem->albumID = albumID;
   songItem->singerName = singerName;
   songItem->duration = durtion;
   songQHash.insertMulti(fileName,songItem );
   songQHash.reserve( songQHash.size() );

}

void SongInfo::clearAllItems()
{
    if( this->songQHash.count()> 0 ){
             QHash<QString,SongItem *>::iterator  iterator = songQHash.begin();
             while( iterator !=  songQHash.end() ){
                 if( iterator.value() != nullptr ){
                     SongItem *songItem =  iterator.value();
                     delete songItem;
                     songItem == Q_NULLPTR;
                     iterator++;
                 }
             }
    }
    songQHash.clear();
}


QHash<QString, SongItem *> SongInfo::getSongQHash()
{
    return this->songQHash;
}

SongInfo::~SongInfo()
{

}


