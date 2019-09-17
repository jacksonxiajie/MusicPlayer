#include "downinfo.h"
#include<QStringLiteral>
DownInfo::DownInfo()
{

    normalFile = new normalInfo;
    HQFile = new HQInfo;
    SQFile = new SQInfo;

    this->normalFile->privilege = 2;
    this->HQFile->HQprivilege = 2;
    this->SQFile->SQprivilege =2;

}

void DownInfo::setNormalInfo(int privilege, QString fileHash, qint64 fileSize, int bitrate, QString extName,qint64 albumId)
{
    this->normalFile->privilege = privilege;
    this->normalFile->fileHash = fileHash;
    this->normalFile->fileSize = fileSize;
    this->normalFile->bitrate = bitrate;
    this->normalFile->extName = extName;
    this->normalFile->albumId = albumId;

}

void DownInfo::setHQInfo(int privilege, QString fileHash, qint64 fileSize, int bitrate, QString extName,qint64 albumId)
{
    this->HQFile->HQprivilege = privilege;
    this->HQFile->HQfileHash = fileHash;
    this->HQFile->HQfileSize = fileSize;
    this->HQFile->HQbitrate = bitrate;
    this->HQFile->HQextName = extName;
    this->HQFile->albumId = albumId;
}

void DownInfo::setSQInfo(int privilege, QString fileHash, qint64 fileSize, int bitrate, QString extName,qint64 albumId)
{
    this->SQFile->SQprivilege = privilege;
    this->SQFile->SQfileHash = fileHash;
    this->SQFile->SQfileSize = fileSize;
    this->SQFile->SQbitrate = bitrate;
    this->SQFile->SQextName = extName;
     this->SQFile->albumId = albumId;
}

normalInfo *DownInfo::getNormalInfo()
{
    return this->normalFile;
}

HQInfo *DownInfo::getHQInfo()
{
    return this->HQFile;
}

SQInfo* DownInfo::getSQInfo()
{
    return this->SQFile;
}


DownInfo::~DownInfo()
{
    delete normalFile;
    normalFile = nullptr;
    delete HQFile;
    HQFile = nullptr;
    delete SQFile;
    SQFile = nullptr;
}
