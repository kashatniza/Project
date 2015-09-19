#ifndef MAPLAYER_H
#define MAPLAYER_H

#include <QGraphicsRectItem>
#include <QVector>
#include <QRectF>

class MapImage : public QGraphicsRectItem
{
public:
    MapImage();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = NULL);
    virtual QRectF boundingRect() const;
private:
    QVector<QRectF> grassRect;
    QVector<QPointF> markupPoint;
};

#endif // MAPLAYER_H
