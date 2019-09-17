#ifndef CUSTOMFUNCTION_H
#define CUSTOMFUNCTION_H

#include<QString>
#include<QMap>
#include<QStringList>
#include<QTime>

void splitString_Iric( const QString & IricStr, QMap<qint64,QString> &map)
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
                  msec += ( time.msec() / 10 );
                   map.insert( msec , lyricStr );
            }
       }
   }
}


QString formatTime(qint64 time)
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

#endif // CUSTOMFUNCTION_H
