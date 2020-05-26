#ifndef DEVICEITEMDELEGATE_H
#define DEVICEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "listviewitemdata.h"

class DeviceItemDelegate: public QStyledItemDelegate
{
public:
    DeviceItemDelegate(QObject *parent=nullptr);
    void paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;


};


#endif // DEVICEITEMDELEGATE_H
