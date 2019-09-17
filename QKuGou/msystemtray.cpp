#include "msystemtray.h"
#include<QIcon>
#include<QDebug>
MSystemTray::MSystemTray(QObject *parent):
    QSystemTrayIcon(parent)
{
    menue =  new QMenu;
    showMainAction =  new QAction(QObject::trUtf8("显示主界面"),this);
    exitAppAction =  new QAction(QObject::trUtf8("退出程序"),this);
    menue->addAction( showMainAction );
    menue->addSeparator();
    menue->addAction( exitAppAction );
    this->setContextMenu( menue );
    this->setIcon( QIcon(":/background/music.ico" ));
    this->setToolTip(QStringLiteral("仿酷狗音乐"));
    connect(showMainAction,SIGNAL(triggered()),this,SIGNAL( sig_showMainActionClicked()));
    connect(exitAppAction,SIGNAL(triggered()),this,SIGNAL( sig_exitAppActionClicked()));
}

MSystemTray::~MSystemTray()
{
    delete menue;
    delete showMainAction;
    delete exitAppAction;
}


