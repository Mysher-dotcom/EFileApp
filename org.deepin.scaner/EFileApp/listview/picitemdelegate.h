#ifndef PICITEMDELEGATE_H
#define PICITEMDELEGATE_H
#include <QStyledItemDelegate>
#include <DMainWindow>
#include <QLineEdit>

DWIDGET_USE_NAMESPACE

class PicItemDelegate : public QStyledItemDelegate
{
public:
    PicItemDelegate(QObject *parent=nullptr);
    QObject *parentCtrl;
    //QLineEdit *lineEdit;
    void paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const Q_DECL_OVERRIDE;


    bool isDarkType() const ; //当前系统是否为深色主题

};

#endif // PICITEMDELEGATE_H
