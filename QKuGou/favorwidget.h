#ifndef FAVORWIDGET_H
#define FAVORWIDGET_H

#include"songinfo.h"

#include<QWidget>
#include<QString>

class QListWidgetItem;
class QToolButton;
class  QListWidget;

namespace Ui {
    class favorWidget;
}

class FavorWidget: public QWidget
{
    Q_OBJECT
public:
    FavorWidget(QWidget *parent = 0);
    ~FavorWidget();
  void  setListwidCurrentItem( const int & index );

private:
    QWidget* customeWidgetItem( QWidget * itemWidget ,  SongItem *   songItem);
    QString  getAudioName_FromBtnClicked(QToolButton *tBtn, QListWidget *listWidget, int &row );
    void getFavorPlaylist(const QListWidget * const listWidget, QList<SongItem *> &favorPlaylist );
    bool songIsExist( const SongItem * const songItem,  const QListWidget * const listWidget);

public slots:
    void  slot_addFavorSong(  SongItem *   songItem);   //const 右边是谁，谁就不能被改变
    void  slot_removeFavorSong( const SongItem  * const songItem );  //并重新释放列表加载数据库，与历史数据表结合信号

private slots:
    void slotHandle_favBtnClicked();
    void slotHandle_delBtnClicked();

    void slot_listItemDoubleClicked(QListWidgetItem *listItem );

signals:
    void sig_updateFavBtnStatus( const QString objectName);
    void sig_sendFavorPlaylist(ListWidgetFlags,const int &, QList<SongItem *> & );

private:
    Ui::favorWidget *ui;
    QList<SongItem *> favorPlaylist;
    QHash<QString, SongItem * > favorQHash;

};

#endif // FAVORWIDGET_H
