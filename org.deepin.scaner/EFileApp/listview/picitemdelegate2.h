#ifndef PICITEMDELEGATE_H
#define PICITEMDELEGATE_H
#include <QStyledItemDelegate>

class PicItemDelegate : public QStyledItemDelegate
{
public:
    PicItemDelegate(QObject *parent=nullptr);

   static char* substrend(char * str, int n);
    void paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;

    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;


};

#endif // PICITEMDELEGATE_H
