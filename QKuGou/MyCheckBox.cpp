#include "MyCheckBox.h"

MyCheckBox::MyCheckBox(QString text,QWidget *parent)
    : QCheckBox(text,parent)
{

}

MyCheckBox::MyCheckBox(QWidget *parent)
    : QCheckBox(parent)
{

}

MyCheckBox::~MyCheckBox()
{
   this->deleteLater();
}

void MyCheckBox::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED( e );
    this->setChecked( ! this->isChecked() );
    emit clicked( this->isChecked());
}
