#ifndef DOWNLOADFORM_H
#define DOWNLOADFORM_H

#include <QWidget>
#include<QtCore/QThread>

#include "mnetworker.h"
#include "downinfo.h"

class QButtonGroup;
class QAbstractButton;

namespace Ui {
class DownloadForm;
}
class DownloadForm : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadForm(QWidget *parent = 0);
    ~DownloadForm();


private slots:
    void on_btn_alterDir_clicked();

    void on_btn_startDown_clicked();

    void on_btn_close_clicked();

    //接收主窗口传过来的歌曲专辑名称
    void slot_receiveAudioName(const QString &audioName);
    void slot_handleNetworkError(const QString netError);
    void slot_selectedBtn(int id);

    //启动线程获取将要下载的音质信息

    void slot_downFinish(const QString &fileName);

private:
    void initWidget();
     void loadStyleSheet(QString qssFile);
private:
    Ui::DownloadForm *ui;
   MNetWorker *mNetWorker;
   DownInfo  downInfo;
   QButtonGroup *radioBtnGroup;
   QString playUrl_selected;
};

#endif // DOWNLOADFORM_H
