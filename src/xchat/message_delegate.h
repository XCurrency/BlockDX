#ifndef MESSAGE_DELEGATE_H
#define MESSAGE_DELEGATE_H

#include <QStyledItemDelegate>

//*****************************************************************************
//*****************************************************************************
class MessageDelegate : public QStyledItemDelegate
{
    enum
    {
        fontSizeForDate = 12,
        fontSizeForText = 14
    };

public:
    MessageDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    void paint(QPainter * painter, const QStyleOptionViewItem & option,
               const QModelIndex & index) const;
};


#endif // MESSAGE_DELEGATE_H
