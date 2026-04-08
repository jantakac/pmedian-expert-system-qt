#ifndef NODEGRAPHICSITEM_HPP
#define NODEGRAPHICSITEM_HPP

#include <QBrush>
#include <QGraphicsItem>
#include <QPen>
#include <qfont.h>

class NodeGraphicsItem : public QGraphicsEllipseItem
{
public:
    explicit NodeGraphicsItem(
        const QString &label, int32_t x, int32_t y, uint16_t size, QGraphicsItem *parent = nullptr);
    void changePosOnGrid(int32_t xGrid, int32_t yGrid);

private:
    QGraphicsTextItem *m_label;
    uint16_t m_size;

    void updateLabelPos();
};

#endif // NODEGRAPHICSITEM_HPP
