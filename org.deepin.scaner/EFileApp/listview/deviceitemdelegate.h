#ifndef DEVICEITEMDELEGATE_H
#define DEVICEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "listviewitemdata.h"
#include <DMainWindow>

class DeviceItemDelegate: public QStyledItemDelegate
{
public:
    DeviceItemDelegate(QObject *parent=nullptr);
    void paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;


    bool isDarkType() const ; //当前系统是否为深色主题
};


#endif // DEVICEITEMDELEGATE_H
