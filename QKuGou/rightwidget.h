#ifndef RIGHTWIDGET_H
#define RIGHTWIDGET_H

#include <QTabWidget>
#include <QHash>
class QToolButton;
class QListWidget;
class QListWidgetItem;

class FavorWidget;
class NativeWidget;
#include <QMediaContent>
#include<QList>
#include"songinfo.h"

namespace Ui {
   class RightWidget;
}


class RightWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit RightWidget(QWidget *parent = 0);
    ~RightWidget();

    void load_FavorSongList( QHash<QString,SongItem *> hash );

    void sethistQhash(const QHash<QString, SongItem *> &hash );
   void  setListwidCurrentItem( const int & index );

private:
  void  inItialListWidget();
  QString  getAudioName_FromBtnClicked(QToolButton *tBtn, QListWidget *listWidget, int &row );

  void show_FavorWidget();  //歌曲收藏模块
  bool songIsExist(const SongItem * const songItem, const QListWidget * const listWidget, int &row);

  void show_NativeWidget();  //本地音乐模块
  void getHistPlaylist(const QListWidget * const listWidget, QList<SongItem *> &HistPlaylist);

  void  init_favorWidget();

signals:
  void sig_rvFavListWidItem( const SongItem * const songItem );
  void sig_adFavListWidItem(   SongItem *  songItem );
  void sig_sendHistPlaylist(ListWidgetFlags ,const int & row,  QList<SongItem *> &histPlaylist);
  void sig_sendPlayHash( const  int & row,   const  QList< QString > & nativePlayList );


public slots:
  void slot_setFavBtnStatus( const QString objectName);
   void slot_addSongToPlayList( QHash<QString,SongItem *> &hash );
    void slot_setListWidCurItem(const ListWidgetFlags &listWidgetFlags, const int &index);

private slots:
  void slotHandle_favBtnClicked();
  void slotHandle_delBtnClicked();
  void slot_listItemDoubleClicked(QListWidgetItem *listItem );
  void slotHandle_tabBarClicked(int index);


protected:
  Ui::RightWidget *ui;
  QWidget* customeWidgetItem(QWidget * const  itemWidget, SongItem *const  songItem );  //const SongItem *songItem

private:
    QHash<QString, SongItem *> histQhash;
//   QHash<QString,SongItem * > playHash;
     QList<SongItem *> histPlaylist;
    FavorWidget * favorWidget;
    NativeWidget *nativeWidget;

};

#endif // RIGHTWIDGET_H
