#include "semaphore.h"

#include <QPainter>
#include <QPen>
#include <QBrush>

//static QColor semaphoreRefColor[] = {
//    QColor::fromRgb(0xFF, 0x00, 0x00),      // Red
//    QColor::fromRgb(0xFF, 0xE4, 0x00),      // Yellow
//    QColor::fromRgb(0x00, 0xFF, 0x00)       // Green
//};

struct Semaphore::TransitionTable_t /*{
    Semaphore::State_t currentState;
    Semaphore::State_t nextState;
    int ticksInState;                   // 100's of milliseconds; in timer ticks actually
    QColor color1, color2;
    bool singleColor;
}*/ Semaphore::transitionTable[] = {
    { Semaphore::GREEN, Semaphore::FLASHING_GREEN, 50, QColor(0x00, 0xFF, 0x00), QColor(), true },
    { Semaphore::FLASHING_GREEN, Semaphore::YELLOW, 30, QColor(0x00, 0xFF, 0x00), QColor(Qt::black), false },
    { Semaphore::YELLOW, Semaphore::RED, 20, QColor(0xFF, 0xE4, 0x00), QColor(), true },
    { Semaphore::RED, Semaphore::RED_YELLOW, 50, QColor(0xFF, 0x00, 0x00), QColor(), true },
    { Semaphore::RED_YELLOW, Semaphore::GREEN, 50, QColor(0xFF, 0x00, 0x00), QColor(0xFF, 0xE4, 0x00),false }
};

int Semaphore::transitionCount = sizeof(Semaphore::transitionTable) /
        sizeof(struct Semaphore::TransitionTable_t);

Semaphore::Semaphore() :
    state(GREEN),
    nextState(FLASHING_GREEN),
    stateTicks(50),
    ticksLeft(50),
    color1(0x00, 0xFF, 0x00),
    color2(Qt::black),
    singleColorState(true)
{

}

void Semaphore::setState( Semaphore::State_t _s )
{
    for(int i = 0 ; i < transitionCount ; i++) {
        if( transitionTable[i].currentState == _s ) {
            state = _s;
            nextState = transitionTable[i].nextState;
            stateTicks = ticksLeft = transitionTable[i].ticksInState;
            color1 = transitionTable[i].color1;
            color2 = transitionTable[i].color2;
            singleColorState = transitionTable[i].singleColor;
            break;
        }
    }
}

bool Semaphore::isOpen() const
{
    return (state == GREEN || state == FLASHING_GREEN);
}

void Semaphore::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QPen pen(Qt::SolidLine);
    QBrush brush(Qt::SolidPattern);
    QRectF ellipseRect( 0, 0, 0.5, 0.5 );

    ellipseRect.moveCenter(boundingRect().center());

    pen.setColor(QColor::fromRgb(0, 0, 0));
    pen.setWidth(0);

    brush.setColor( colorForState() );

    painter->save();

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse( ellipseRect );

    painter->restore();
}

QRectF Semaphore::boundingRect() const
{
    return QRectF(0, 0, 1.0, 1.0);
}

void Semaphore::advance(int phase)
{
    if( phase == 0 ) {
        return;
    }

    ticksLeft--;
    if( ticksLeft <= 0 ) {
        setState(nextState);
    }
    update();
}

QColor Semaphore::colorForState() const
{
    QColor result(color1);
    if( ! singleColorState ) {
        // This allow blink 4 times within state. Blink duration mapped on time interval.
        int octant = stateTicks / 8;
        int nQuarter = (stateTicks - ticksLeft) / octant;
        qreal percents = ((qreal)((stateTicks - ticksLeft) % octant)) / ((qreal)octant);
        const QColor * ps = ( (nQuarter % 2 == 1) ? &color2 : &color1),
                     * pe = ( (nQuarter % 2 == 1) ? &color1 : &color2);

        result.setRed(ps->red() + (pe->red() - ps->red()) * percents);
        result.setGreen(ps->green() + (pe->green() - ps->green()) * percents);
        result.setBlue(ps->blue() + (pe->blue() - ps->blue()) * percents);
    }
    return result;
}
