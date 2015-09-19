#ifndef CAR_H
#define CAR_H

#include <QGraphicsPixmapItem>

class Scene;

class Car : public QGraphicsPixmapItem
{
public:
    Car(const QColor & color);

    Scene * scene() const;
    void setCell(int row, int col);
    int direction() const;

    virtual void advance(int phase);
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QPointF findTargetPoint(int row, int col);
    int selectDirectionFromCell(int cell) const;

    // Actual vars
    QPointF startPoint;
    QPointF targetPoint;
    int targetCellCol;
    int targetCellRow;
    int targetDirection;
    int progress;
    int progressDelta;
    int _direction;
};

#endif // CAR_H
