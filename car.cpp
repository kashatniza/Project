#include "car.h"

#include <math.h>
#include <QPixmap>
#include <QImage>
#include <QVector2D>

#include <QDebug>

#include "scene.h"

Car::Car(const QColor &color) :
    startPoint(0,0),
    targetPoint(0,0),
    targetCellCol(0),
    targetCellRow(0),
    targetDirection(0),
    progress(0),
    progressDelta(10),
    _direction(-1)
{
    QPixmap carPix;
    QImage carImg = QImage(":/images/red-car.png");
    QImage alphaChannel = carImg.alphaChannel();

    carImg.fill(color);
    carImg.setAlphaChannel(alphaChannel);

    carPix.convertFromImage(carImg);

    setPixmap(carPix);
    setTransformOriginPoint( carPix.rect().center() );
    setFlag(ItemSendsGeometryChanges);
}

Scene * Car::scene() const
{
    return (Scene*)QGraphicsItem::scene();
}

static struct DirectionAngle_t {
    int bitMask;
    qreal angle;
    int tRow, tCol;
} directionAngleMap[] = {
    { 0x01,  0.0, -1,  0 },     // Up
    { 0x02, 90.0,  0,  1 },     // Right
    { 0x04,180.0,  1,  0 },     // Down
    { 0x08,270.0,  0, -1 }      // Left
};
static const int mapCount = sizeof(directionAngleMap) / sizeof(struct DirectionAngle_t);

static DirectionAngle_t directionDef(int cell) {
    DirectionAngle_t result = { 0, 0.0, 0, 0 };
    for(int i = 0 ; i < mapCount ; i++) {
        if( cell & directionAngleMap[i].bitMask ) {
            result = directionAngleMap[i];
            break;
        }
    }
    return result;
}

void Car::setCell(int row, int col)
{
    int cell = scene()->cellData(row, col);

    startPoint = scene()->cellRect(row, col).center();
    progress = 0;

    if( _direction == -1 || (_direction & cell) == 0 ) {       // Car not oriented yet or my direction isn't
        _direction = selectDirectionFromCell( cell );        // suitable for current cell.
    }

    DirectionAngle_t dir = directionDef(_direction);
    _direction = dir.bitMask;

    targetPoint = findTargetPoint( row + dir.tRow, col + dir.tCol );

    QRectF bounds = boundingRect();
    bounds.moveCenter( startPoint );
    setPos(bounds.topLeft());
    setRotation(dir.angle);
}

int Car::direction() const
{
    return _direction;
}

QPointF Car::findTargetPoint( int row, int col )
{
    targetDirection = selectDirectionFromCell(scene()->cellData(row, col));

    targetCellCol = col;
    targetCellRow = row;

    return scene()->cellRect(row, col).center();
}

int Car::selectDirectionFromCell(int cell) const {
    int cellDirections[4] = { 0, 0, 0, 0 };
    int n = 0;
    for( int i = 0 ; i < 4 ; i++ ) {
        if( (cell & (1 << i)) != 0 ) {
            cellDirections[n++] = 1 << i;
        }
    }
    return cellDirections[ (int)(((qreal)rand())/((qreal)RAND_MAX) * n) ];
}

void Car::advance(int phase)
{
    if( phase == 0 ) {
        return;
    }
    if( ! scene()->cellBlocked(this) ) {
        QRectF carRect = boundingRect();
        QVector2D v = QVector2D(targetPoint - startPoint) / 100.0 * progress;

        carRect.moveCenter( v.toPointF() + startPoint );        
        setPos(carRect.topLeft());

        progress += progressDelta;

        if( progress >= 100 ) {
            _direction = targetDirection;
            setCell( targetCellRow, targetCellCol );
            progress %= 100;
        }

        update();
    }
}

QVariant Car::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if( change == ItemScaleChange ) {
        double scaleFactor = value.toDouble();
        QSizeF size = boundingRect().size();
        qreal maxSize = qMax( size.width(), size.height() );

        return QVariant::fromValue(scaleFactor / (maxSize + 2));
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}
