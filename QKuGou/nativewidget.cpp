#include "nativewidget.h"
#include "ui_nativewidget.h"
#include<QDir>
#include<QFileInfoList>
#include<QFileInfo>
#include<QDebug>
#include<QFileDialog>
#include<windows.h>
NativeWidget::NativeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NativeWidget)
{
    ui->setupUi(this);
    connect( ui->listWid_nativeMusic, &QListWidget::itemDoubleClicked, this,&NativeWidget::slot_nativeListItem_DoubleClicked);
}

void NativeWidget::Load_nativeMusic(const QString &scanDir)
{
    QDir dir( scanDir  );//定义一个QDir变量,设置路径为当前目录的music文件夹(这里可用绝对路径也可以用相对路径)
    QFileInfoList fileinfolist; //文件信息表
    QStringList filters( "*.mp3" );
    dir.setNameFilters( filters );  //过滤文件
    fileinfolist = dir.entryInfoList(); //列出所有符合的文件
    if( fileinfolist.isEmpty() ) { return ;}
    nativePlayList.clear();
    foreach (QFileInfo fileinfo, fileinfolist) {
        ui->listWid_nativeMusic->addItem( fileinfo.fileName() );
        nativePlayList.append(  fileinfo.filePath() );    // 文件的url 路径，和 文件无后缀名
    }

}

void NativeWidget::setListwidCurrentItem(const int &index)
{
    ui->listWid_nativeMusic->setCurrentRow( index );
}


void NativeWidget::on_chBox_native_clicked(bool checked)
{
    ui->listWid_nativeMusic->setVisible( checked );
}

void NativeWidget::slot_nativeListItem_DoubleClicked(QListWidgetItem *item)
{
    int row = ui->listWid_nativeMusic->row( item );
    emit sig_sendPlayHash(row,nativePlayList);
}


NativeWidget::~NativeWidget()
{
    delete ui;
}

void NativeWidget::on_tBtn_scan_clicked()
{
    QString scanDir= QFileDialog::getExistingDirectory(this,tr("选择音乐文件夹"),"D:",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    Load_nativeMusic( scanDir );
}
