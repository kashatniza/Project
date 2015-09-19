#include "mapimage.h"

#include <QPainter>
#include <QColor>
#include <QPen>
#include <QBrush>

#include <QVector>

#include "scene.h"

MapImage::MapImage() :
    grassRect(8),
    markupPoint(18)
{
    grassRect[0] = QRectF( 0, 0, 4, 1 );
    grassRect[1] = QRectF( 0,11, 4, 1 );
    grassRect[2] = QRectF( 1, 2, 3, 3 );
    grassRect[3] = QRectF( 1, 7, 3, 3 );
    grassRect[4] = QRectF( 6, 2, 8, 3 );
    grassRect[5] = QRectF( 6, 7, 8, 3 );
    grassRect[6] = QRectF(16, 1, 3, 4 );
    grassRect[7] = QRectF(16, 7, 3, 4 );

    markupPoint[0] = QPointF( 1,  6);
    markupPoint[1] = QPointF( 4,  6);
    markupPoint[2] = QPointF( 6,  6);
    markupPoint[3] = QPointF(14,  6);
    markupPoint[4] = QPointF(16,  6);
    markupPoint[5] = QPointF(19,  6);
    markupPoint[6] = QPointF( 6,  1);
    markupPoint[7] = QPointF(14,  1);
    markupPoint[8] = QPointF( 6, 11);
    markupPoint[9] = QPointF(14, 11);

    markupPoint[10] = QPointF( 5,  2);
    markupPoint[11] = QPointF( 5,  5);
    markupPoint[12] = QPointF( 5,  7);
    markupPoint[13] = QPointF( 5, 10);
    markupPoint[14] = QPointF(15,  2);
    markupPoint[15] = QPointF(15,  5);
    markupPoint[16] = QPointF(15,  7);
    markupPoint[17] = QPointF(15, 10);
}

static QColor grassColor = QColor::fromRgb(0x0D, 0x88, 0x00);
static QColor roadColor = QColor::fromRgb(0xAA, 0xAA, 0xAA);
static QColor markupColor = QColor::fromRgb(0xFF, 0xFF, 0xFF);

void MapImage::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QPen borderPen(markupColor);
    borderPen.setStyle(Qt::SolidLine);
    borderPen.setWidthF(0.2);

    painter->save();

    // Background
    painter->fillRect( boundingRect(), QBrush(roadColor) );

    // Grass
    painter->setPen(borderPen);
    painter->setBrush(QBrush(grassColor));
    for( int i = 0 ; i < grassRect.count() ; i++ ) {
        painter->drawRect( grassRect[i] );
    }

    // Frame
    painter->setPen(QPen(Qt::black));
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->drawRect( QRectF(QPointF(0,0), QSizeF(20, 12)) );

    // Markup
    QPen markupPen(Qt::DashLine);
    markupPen.setColor(markupColor);
    markupPen.setWidthF(0.2);
    painter->setPen(markupPen);

    for( int i = 0 ; i < markupPoint.count() ; i += 2 ) {
        painter->drawLine(markupPoint[i],markupPoint[i + 1]);
    }

    painter->restore();
}

QRectF MapImage::boundingRect() const
{
    return QRectF(0, 0, 20, 12);
}
