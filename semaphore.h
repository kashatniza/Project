#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <QGraphicsEllipseItem>
#include <QColor>

class Semaphore : public QGraphicsEllipseItem
{
public:
    typedef enum {
        GREEN = 0,
        FLASHING_GREEN,
        YELLOW,
        RED,
        RED_YELLOW
    } State_t;

    Semaphore();
    void setState( State_t _s );
    bool isOpen() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = NULL);
    virtual QRectF boundingRect() const;
    virtual void advance(int phase);

protected:
    QColor colorForState() const;

    State_t state;
    State_t nextState;
    int stateTicks, ticksLeft;
    QColor color1, color2;
    bool singleColorState;

    static struct TransitionTable_t {
        State_t currentState;
        State_t nextState;
        int ticksInState;                   // 100's of milliseconds; in timer ticks actually
        QColor color1, color2;
        bool singleColor;
    } transitionTable[];
    static int transitionCount;
};

#endif // SEMAPHORE_H
