#ifndef MSYSTEMTRAY_H
#define MSYSTEMTRAY_H
#include<QSystemTrayIcon>
#include<QMenu>
#include<QAction>
#include<QObject>
#include<QMainWindow>

/**
 * @brief The MSystemTray class
 * 改写默认的QSystemTrayIcon类，添加特殊功能
 */

class MSystemTray: public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit MSystemTray(QObject *parent = 0);
    ~MSystemTray();

signals:
    void sig_showMainActionClicked();
    void sig_exitAppActionClicked();

private:
    QMenu *menue;
    QAction * showMainAction;
    QAction * exitAppAction;
};

#endif // MSYSTEMTRAY_H
