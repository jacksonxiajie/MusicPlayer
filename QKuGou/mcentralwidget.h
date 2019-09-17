#ifndef MCENTRALWIDGET_H
#define MCENTRALWIDGET_H

#include "songinfo.h"
#include "scheckboxheaderview.h"

#include <QWidget>
#include<QStackedWidget>
#include <QTableWidget>
#include<QStringList>
#include<QButtonGroup>
#include<QVBoxLayout>
#include<QScrollBar>
namespace Ui {
class MCentralWidget;
}

class MCentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MCentralWidget(QWidget *parent = 0);
    ~MCentralWidget();

    void Show_SearchedSong( SongInfo   &songInfo );   //传的是引用，不是副本
    void setLabelText( QString keyword);

public slots:
    void slot_updateCentralWidget();

private:
    void  checkedAll(QTableWidget *tableWidget, Qt::CheckState checkState);
    void   show_lrcWidget();

signals:
    void  sig_sendAudioName(  QHash<QString,SongItem*> &hash );
    void  sig_playInstantly(ListWidgetFlags,const int &, QList<SongItem *> &);


private slots:
    void on_tBtn_playSelection_clicked();
    void on_btn_addPlayList_clicked();

    void slotHandle_ItemClicked(QTableWidgetItem* item);
    void slotHandle_ItemDoubleClicked(QTableWidgetItem* item);
    void slotHandle_btnClicked();
    void slotHandle_checkedAll( bool  checkAll);

private:
    Ui::MCentralWidget *ui;
    QHash<QString,SongItem*> searchQHash;
    QHash<QString,SongItem*> sendDataQHash;  //添加选中的歌曲
    QList<SongItem *> sendPlaylist;
    SCheckBoxHeaderView * mCheckHeader ;
    bool  lrcWidget ;

};

#endif // MCENTRALWIDGET_H
