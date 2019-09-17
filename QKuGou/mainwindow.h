  #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QMediaPlayer>
#include<QMediaPlaylist>
#include<QUrl>
#include<QtCore/QThread>
#include<QStackedLayout>
#include<QSystemTrayIcon>
#include<QPoint>
#include<QString>
#include<QList>
#include"songinfo.h"

typedef QHash<QString,SongItem *>  PlayHash;  //取别名

class QListWidgetItem;
class QMouseEvent;

class DeskIyrics;
class DownloadForm;
class MNetWorker;
class MCentralWidget;
class RightWidget;
class PlayModeForm;
class MSystemTray;

class QVBoxLayout;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
     //实现无边框窗体的移动，无论是否写virtual 都会去父类中查找相同的虚函数
     virtual void  mousePressEvent(QMouseEvent *event);
     virtual void mouseReleaseEvent(QMouseEvent * event);
     virtual void mouseMoveEvent(QMouseEvent *event );
     virtual bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void slot_tBtn_nextSong();  //下一首
    void slot_btn_playClicked();    //播放按钮
    void slot_mediaStateChanged(QMediaPlayer::State  state);  //获取当前media的播放状态
    void slot_tBtn_preSong();  //上一首
                                             //设置正在播放的歌曲名
    void  slot_progress_sliderMoved(int position);    //进度条被拖动，改变媒体的播放位置
    void  slot_changeHsbProgress( qint64 progress);  //媒体的播放位置改变，进度条更新位置
    void  slot_durationChanged(qint64  duration);         // 设置进度条的范围变化的变化
    void  slot_showPlayTime( qint64 playTime); //设置播放时间

     void slot_tBtn_downloadClicked();   // 下载按钮被点击
     void slot_tBtn_volumn_clicked();      // 音量按钮被点击，静音变化，声音的相关控制
     void  slot_volumn_sliderMoved(int position);
     void  slot_setVolSliderPos(int position);

      void slot_changePlayMode(int  index);      // 选中的播放模式改变，媒体的播放模式改变，提示信息改变
      void slot_setModeTip( const QString & toolTip );   //当播放模式的列表行发生变化，其提示变化
      void slot_tBtn_desktopIricsClicked();         //歌词按钮点击

     void slot_mutedChanged(bool muted );  //媒体静音变化时，按钮状态提示变化

     void slot_recvPlayList( ListWidgetFlags listWidgetFlags,const int & row,  QList<SongItem *> &playlist);
     void slot_recvPlaySong(const int & row,  const  QList< QString > & nativePlayList );

     void slot_changeListitemPos(const int & index);
     void slot_setMediaPlayList(  const QList<SongItem *> & playlist);

     void  slot_MediaStatusChange( QMediaPlayer::MediaStatus status);

private slots:
      void on_tBtn_close_clicked();
      void on_tBtn_miniMum_clicked();
      void slot_showMainWindow();
      void  on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

      //网络搜索
      void on_tBtn_search_clicked();
      void slot_startSearch();
      void replySearchFinished( SongInfo songInfo ,const QString keyword);

      void slot_HandleNetError(QString NetError);

      void slot_MetaDataAvailableChanged( );

      void on_tBtn_cyclePlay_clicked();   //播放模式按钮被点击
      void set_lyricsBackground( const QMediaContent &media );

signals:
     void sig_initDownForm( QString );
     void sig_changeListWidCurItem(const ListWidgetFlags & ,const int &index);

private:
    void inIt_privateData();
    void inIt_MediaPlayer();
    void inIt_NetWork();
    void inIt_BottomWidget();
    void inIt_RightWidget();
    void inIt_CentralWidget();
    void  freeSongitem(  SongInfo & songInfo);

private:
    Ui::MainWindow *ui;

   QMediaPlayer * mediaPlayer;
   QMediaPlaylist *musicPlayList;

   QList<QMediaContent>  nativeMusicPlayList;

   DeskIyrics *desklyrics;   //桌面歌词类
   MNetWorker *mNetWork;   //网络模块类

   DownloadForm *downloadForm;  //下载界面类
   MCentralWidget *mCentralWidget;
   RightWidget *rightWidget;
   PlayModeForm *playModeForm;  //模式选择类
   MSystemTray *msystemTray;

   QVBoxLayout  *centralWidget_VLayout;  //各个窗口类的布局管理器
   QVBoxLayout *rightWidget_VLayout;
   QStackedLayout *stackedLayout;

    QPoint mMousePoint;
    bool  mDragWindow;
    bool  IsUpdateWidget;

    SongInfo  songInfo;  //歌曲信息类
    QThread childThread;
    ListWidgetFlags  listWidgetFlags;
    int saveVolume;
    int savePlayRow;
    int lyricflag ;

    QHash<QString ,SongItem *> playlistHash;
    QMap<qint64,QString> lrcMap;

};

#endif // MAINWINDOW_H
