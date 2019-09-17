#ifndef PLAYMODEFORM_H
#define PLAYMODEFORM_H

#include <QFrame>

namespace Ui {
class PlayModeForm;
}

class PlayModeForm : public QFrame
{
    Q_OBJECT

public:
    explicit PlayModeForm(QWidget *parent = 0);
    ~PlayModeForm();



signals:
    void sig_playModeChanged( const int & index);
     void sig_modeTipChanged( const QString & toolTip );

private:
   Ui::PlayModeForm *ui;
   int LastIndex;
};

#endif // PLAYMODEFORM_H
