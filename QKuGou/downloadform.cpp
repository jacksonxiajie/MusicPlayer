#include "downloadform.h"
#include "ui_downloadform.h"


#include<QFileDialog>
#include<QStringLiteral>
#include<QMessageBox>
#include<QButtonGroup>
#include<QAbstractButton>

DownloadForm::DownloadForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadForm)
{
    ui->setupUi(this);
    mNetWorker = new MNetWorker(this);
    radioBtnGroup = new QButtonGroup(this);
    playUrl_selected ="";

   connect(mNetWorker,SIGNAL(sig_findError(QString)),this,SLOT(slot_handleNetworkError(QString)) );
   connect(mNetWorker,SIGNAL(sig_downFinish(const QString &)), this,SLOT( slot_downFinish( const QString &) ));
   connect( radioBtnGroup,SIGNAL(buttonClicked(int)) ,this,SLOT(slot_selectedBtn(int)));
    initWidget();
    loadStyleSheet(":/DownloadWidStyle.qss");
}


void DownloadForm::initWidget()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool );
    ui->stackedWidget->setCurrentWidget( ui->page_down );
    ui->lbl_title->setText(QStringLiteral("下载窗口"));
    ui->btn_close->setIcon(QIcon( " filename"));
    ui->lbl_audioName->setText("");
    ui->lbl_quality->setText("选择音质");
    ui->radioBtn_NQ->setText(QStringLiteral("标准音质"));
    ui->radioBtn_HQ->setText(QStringLiteral("高品音质"));
    ui->radioBtn_SQ->setText(QStringLiteral("无损音质"));
    ui->lbl_downAddress->setText(QStringLiteral("下载地址"));
    ui->btn_alterDir->setText(QStringLiteral("更改地址"));
    ui->btn_startDown->setText(QStringLiteral("立即下载"));
    ui->cmb_dir->setInsertPolicy(QComboBox::InsertAtTop);
    ui->cmb_dir->addItem(QStringLiteral("D:"));
    ui->cmb_dir->setCurrentIndex(ui->cmb_dir->currentIndex() +1);
    radioBtnGroup->addButton( ui->radioBtn_NQ);
    radioBtnGroup->setId( ui->radioBtn_NQ ,1);
    radioBtnGroup->addButton( ui->radioBtn_HQ);
    radioBtnGroup->setId( ui->radioBtn_HQ ,2);
    radioBtnGroup->addButton( ui->radioBtn_SQ);
    radioBtnGroup->setId( ui->radioBtn_SQ ,3);
    radioBtnGroup->button(1)->setChecked(true);
}

void DownloadForm::loadStyleSheet(QString qssFile)
{
    QFile file(qssFile);
    if(file.open(QFile::ReadOnly)){
        QString style = QString::fromLatin1(file.readAll());
        this->setStyleSheet(style);
        file.close();
     }
}



void DownloadForm::slot_downFinish(const QString &fileName)
{
    if( fileName.isEmpty() ){ return ;}
   int ret =  QMessageBox::information(this,"消息提示",tr("一首歌曲已下载到该路径下：\n%1").arg(fileName),QString("确认"));
   if( 0 == ret  ){
       this->deleteLater();
   }
}

void DownloadForm::slot_receiveAudioName(const QString &audioName)
{

    ui->lbl_audioName->setText( audioName );
    ui->lbl_audioName->adjustSize();  //在有内容的前提下才会生效，所以要先设置内容
    mNetWorker->slot_getPlayInfo( this->downInfo , audioName );

    normalInfo *temNormalInfo = downInfo.getNormalInfo();
    HQInfo * tempHQInfo = downInfo.getHQInfo();
    SQInfo *tempSQInfo = downInfo.getSQInfo();

    float NormalfileSize= (float) (temNormalInfo->fileSize) / (1024 *1024); //MB
    int normalBitrate =temNormalInfo ->bitrate;

    float HQfileSize= (float) (tempHQInfo->HQfileSize) / (1024 *1024); //MB
    int HQBitrate = tempHQInfo->HQbitrate;

    float SQfileSize= (float) (tempSQInfo->SQfileSize) / (1024 *1024); //MB
    int SQBitrate = tempSQInfo->SQbitrate;

    if( (temNormalInfo->privilege != 0 ) && ( tempHQInfo->HQprivilege != 0 ) && ( tempSQInfo->SQprivilege != 0 )){
        ui->label_fileName->setText( audioName );
        ui->stackedWidget->setCurrentWidget( ui->page_refuseDown );
    }

    if( (0 == temNormalInfo->privilege) && ( NormalfileSize != 0) && ( normalBitrate != 0))
    {
        QString normalStr = QString::number(NormalfileSize,'f',2)+tr(" MB/ ")+QString::number(normalBitrate)+tr(" Kbps");
        ui->lbl_nRate->setText( normalStr );
        ui->lbl_nRate->adjustSize();
    }else{
        ui->lbl_nRate->setVisible(false);
        ui->radioBtn_NQ->hide();
    }

    if(   ( 0 == tempHQInfo->HQprivilege ) && ( 0 != HQfileSize) && ( 0 != HQBitrate)) {
        QString HQStr = QString::number(HQfileSize,'f',2)+tr(" MB/ ")+QString::number(HQBitrate)+tr(" Kbps");
        ui->lbl_hRate->setText(HQStr);
        ui->lbl_hRate->adjustSize();
    }else{
        ui->lbl_hRate->hide();
        ui->radioBtn_HQ->hide();
    }

    if( ( 0 == tempSQInfo->SQprivilege ) && ( 0.0 != SQfileSize) && ( 0 != SQBitrate)){
        QString SQStr = QString::number(SQfileSize,'f',2)+tr(" MB/ ")+QString::number(SQBitrate)+tr(" Kbps");
        ui->lbl_sRate->setText( SQStr );
        ui->lbl_sRate->adjustSize();
    }else{
        ui->lbl_sRate->hide();
        ui->radioBtn_SQ->hide();
    }

}

void DownloadForm::on_btn_alterDir_clicked()
{
     QString selectedDir = QFileDialog::getExistingDirectory(this,QStringLiteral("请选择下载目录"),QStringLiteral("/home"),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
      ui->cmb_dir->setInsertPolicy(QComboBox::InsertAtTop);
     ui->cmb_dir->addItem( selectedDir);
     ui->cmb_dir->setCurrentIndex( ui->cmb_dir->currentIndex() +1);

}


void DownloadForm::on_btn_startDown_clicked()
{
    //获取保存路径
    QString Dir = ui->cmb_dir->currentText().trimmed();
    //获取文件名后缀
    QString fileName = ui->lbl_audioName->text().trimmed() +tr(".").trimmed()+  downInfo.getNormalInfo()->extName.trimmed();
    //获取要下载文件的http地址
    QString playUrl_selected ="";

    switch (  radioBtnGroup->checkedId()  ) {
    case 1:
        playUrl_selected = mNetWorker->getSelectedUrl( downInfo.getNormalInfo()->fileHash,downInfo.getNormalInfo()->albumId );
        break;
    case 2:
        playUrl_selected = mNetWorker->getSelectedUrl( downInfo.getHQInfo()->HQfileHash,downInfo.getHQInfo()->albumId );
        break;
    case 3:
        playUrl_selected = mNetWorker->getSelectedUrl( downInfo.getSQInfo()->SQfileHash,downInfo.getSQInfo()->albumId );
        break;
    default:
        playUrl_selected ="";
        break;
    }
    if( ! playUrl_selected.isEmpty() ){
        mNetWorker->downloadFile( playUrl_selected,fileName,Dir);
    }else{
        slot_handleNetworkError(QString("下载地址不能为空!"));
    }
}


void DownloadForm::slot_handleNetworkError(const QString netError)
{
    if( netError.trimmed() != QString("NoError").trimmed() ){
        ui->frame_downInfo->hide();
   int ret = QMessageBox::critical(this,tr("网络似乎出现了错误哦"),tr("%1").arg(netError),QMessageBox::Abort,QMessageBox::NoButton,QMessageBox::NoButton);
          if( 0 == ret ){
              this->deleteLater();
          }
    }
}

void DownloadForm::slot_selectedBtn(int id)
{
    radioBtnGroup->button(id)->setChecked(true);
}


void DownloadForm::on_btn_close_clicked()
{
    this->deleteLater();
}


DownloadForm::~DownloadForm()
{
    delete ui;
    if( mNetWorker != nullptr )
    {
        delete  mNetWorker;
        mNetWorker = nullptr;
    }

    if( radioBtnGroup != nullptr)
    {
        delete  radioBtnGroup;
        radioBtnGroup = nullptr;
    }

}




