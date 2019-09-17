#ifndef DOWNINFO_H
#define DOWNINFO_H

#include<QString>

struct normalInfo
{
  int privilege;     //权限为 0  才可以下载，默认权限为 2；
  QString  fileHash;
  qint64   fileSize;
  int  bitrate;
  QString extName;
  int albumId;
};

struct HQInfo
{
    int HQprivilege;
    QString  HQfileHash;
    qint64   HQfileSize;
    int  HQbitrate;
    QString HQextName;
   int albumId;
};

struct SQInfo
{
    int SQprivilege;
    QString  SQfileHash;
    qint64   SQfileSize;
    int  SQbitrate;
    QString SQextName;
    int albumId;
};


class DownInfo
{
public:
    DownInfo();
    ~DownInfo();

    void setNormalInfo(int privilege,QString  fileHash ,qint64 fileSize,int  bitrate,QString extName,qint64 albumId);
    void setHQInfo(int privilege,QString  fileHash ,qint64 fileSize,int  bitrate,QString extName,qint64 albumId );
    void setSQInfo(int privilege,QString  fileHash ,qint64 fileSize,int  bitrate,QString extName,qint64 albumId);
    normalInfo *getNormalInfo();
    HQInfo *getHQInfo();
    SQInfo *getSQInfo();


private:
    normalInfo *normalFile;
    HQInfo  *HQFile;
    SQInfo   *SQFile;

};

#endif // DOWNINFO_H
