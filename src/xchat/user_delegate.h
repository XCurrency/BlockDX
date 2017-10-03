#ifndef USER_DELEGATE_H
#define USER_DELEGATE_H

#include <QStyledItemDelegate>

//*****************************************************************************
//*****************************************************************************
class UserDelegate : public QStyledItemDelegate
{
    enum
    {
        fontSizeForLabel = 14,
        fontSizeForAddr = 10
    };

public:
    UserDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    void paint(QPainter * painter, const QStyleOptionViewItem & option,
               const QModelIndex & index) const;
};



#endif // USER_DELEGATE_H
