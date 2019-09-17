#ifndef SCHECKBOXHEADERVIEW_H
#define SCHECKBOXHEADERVIEW_H

#include <QtGui>
#include <QPainter>
#include <QHeaderView>
#include <QStyleOptionButton>
#include <QStyle>
class SCheckBoxHeaderView : public QHeaderView
{
    Q_OBJECT
private:
    bool isChecked;
    int m_checkColIdx;
public:
    SCheckBoxHeaderView( int checkColumnIndex, Qt::Orientation orientation, QWidget * parent = 0) :
    QHeaderView(orientation, parent) {
        m_checkColIdx = checkColumnIndex;
        isChecked = false;
        this->updateSection( m_checkColIdx );
    }

    void setHeaderItemChecked( )
    {
        isChecked = false ;
        this->updateSection( m_checkColIdx );  //updateSection 触发paintSection 事件绘图
    }


signals:
    void checkStausChange(bool);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
    {
        painter->save();
        QHeaderView::paintSection(painter, rect, logicalIndex);
        painter->restore();
        if (logicalIndex == m_checkColIdx) {
            QStyleOptionButton option;
            int width = 10;
            for (int i=0; i<logicalIndex; ++i)
            width += sectionSize( i );
            option.rect = QRect(3, 5, 21, 21);
            if (isChecked)
                option.state = QStyle::State_On;
            else
                option.state = QStyle::State_Off;
            this->style()->drawControl(QStyle::CE_CheckBox, &option, painter);
        }
    }

    void mousePressEvent(QMouseEvent *event) {
        if (visualIndexAt(event->pos().x()) == m_checkColIdx) {
            isChecked = ! isChecked;
            this->updateSection(m_checkColIdx);
            emit checkStausChange(isChecked);
        }
       QHeaderView::mousePressEvent(event);
    }   
};
#endif // SCHECKBOXHEADERVIEW_H
