#ifndef ROADMAP_H
#define ROADMAP_H

#include <QGraphicsScene>
#include <QVector>

#include "semaphore.h"

class MapImage;
class Car;

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    typedef enum {
        NOCONTROL = 0,
        ADD_CAR,
        DEL_CAR
    } SceneControlState_t;

    explicit Scene();

    int cellData(int row, int col);
    QRectF cellRect(int row, int col) const;
    bool cellBlocked(Car * pCar) const;

    void enterAddCarWith(const QColor & color);
    void enterDelCarMode();
    void addCarAt(const QColor &color, int row, int col);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void timerEvent(QTimerEvent *event);

private:
    int scaleFactor;
    QVector<Car*> car;
    QVector<Semaphore*> semaphore;
    int animationTimer;
    int timerInterval;
    SceneControlState_t sceneControlState;
    QColor _addCarColor;
    static struct _semaphoreConfiguration_t {
        int column, row;
        int controlColumn, controlRow;
        Semaphore::State_t state;
    } semaphoreConfig[];
    static const int semaphoreCount;
public slots:
    void setPause(bool bPause);
};

#endif // ROADMAP_H
