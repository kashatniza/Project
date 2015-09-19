#include "scene.h"
#include <math.h>
#include <QVector2D>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

#include "mapimage.h"
#include "car.h"

#include <QDebug>

static int lanes[12][20] = {
    { 0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0C, 0x08, 0x08, 0x08, 0x08, 0x08 },
    { 0x02, 0x02, 0x02, 0x02, 0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x08, 0x08, 0x08, 0x0C, 0x09, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0C, 0x09, 0x08, 0x08, 0x08, 0x09 },
    { 0x03, 0x02, 0x02, 0x02, 0x06, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x06, 0x03, 0x02, 0x02, 0x02, 0x01 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x01, 0x08, 0x08, 0x08, 0x0C, 0x09, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0C, 0x01, 0x00, 0x00, 0x00, 0x01 },
    { 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x02, 0x02, 0x02, 0x01 }
};

struct Scene::_semaphoreConfiguration_t /*{
    int column, row;
    int controlColumn, controlRow;
    Semaphore::State_t state;
}*/ Scene::semaphoreConfig[] = {
    { 3, 4,  4, 4, Semaphore::GREEN},
    { 6, 4,  6, 5, Semaphore::RED},
    { 6, 7,  5, 7, Semaphore::GREEN},
    { 3, 7,  3, 6, Semaphore::RED},
    {13, 4, 14, 4, Semaphore::GREEN},
    {16, 4, 16, 5, Semaphore::RED},
    {16, 7, 15, 7, Semaphore::GREEN},
    {13, 7, 13, 6, Semaphore::RED}
};
const int Scene::semaphoreCount = sizeof(Scene::semaphoreConfig) /
        sizeof(struct Scene::_semaphoreConfiguration_t);


Scene::Scene() :
    scaleFactor(24),
    animationTimer(-1),
    timerInterval(100),
    sceneControlState(Scene::NOCONTROL)
{
    MapImage * map = new MapImage();
    map->setScale(scaleFactor);
    addItem(map);

    semaphore.reserve(semaphoreCount);
    for(int i = 0 ; i < semaphoreCount ; i++) {
        Semaphore * s = new Semaphore();

        addItem(s);
        semaphore.append(s);
        s->setScale(scaleFactor);
        s->setPos(semaphoreConfig[i].column * scaleFactor,
                  semaphoreConfig[i].row * scaleFactor);
        s->setState(semaphoreConfig[i].state);
    }

    QList<QColor> carColors = QList<QColor>() << QColor(Qt::black)
                                              << QColor(Qt::blue);
    QVector<QPair<int,int> > suitableCell;

    for( int r = 0 ; r < 12 ; r ++ ) {
        for( int c = 0 ; c < 20 ; c++ ) {
            if( lanes[r][c] == 0x01 || lanes[r][c] == 0x02 ||
                lanes[r][c] == 0x04 || lanes[r][c] == 0x08 ) {
                suitableCell.append( QPair<int,int>(r, c) );
            }
        }
    }

    foreach (QColor cc, carColors) {
        int i = (((qreal)rand()) / ((qreal)RAND_MAX)) * suitableCell.count();
        addCarAt( cc, suitableCell[i].first, suitableCell[i].second );
        suitableCell.remove(i);
    }


    animationTimer = startTimer(timerInterval);
}

int Scene::cellData(int row, int col)
{
    if( row < 0 || row > 11 || col < 0 || col > 19 ) {
        return 0;
    }
    return lanes[row][col];
}

QRectF Scene::cellRect(int row, int col) const
{
    if( row < 0 || row > 11 || col < 0 || col > 19 ) {
        return QRectF();
    }
    return QRectF( scaleFactor * col, scaleFactor * row, scaleFactor, scaleFactor );
}

bool Scene::cellBlocked(Car * pCar) const
{
    QPoint carPos = pCar->pos().toPoint() / scaleFactor;
    QRectF carRect = pCar->boundingRect().translated(pCar->pos());
    QVector2D carMotion = QVector2D(QTransform().rotate(pCar->rotation()).map(QPointF(0, -1)));
    int row = carPos.y();
    int col = carPos.x();
    QRectF cell = cellRect(row, col);
    bool blocked = false;

    for( int i = 0 ; i < semaphoreCount ; i++ ) {
        if( row == semaphoreConfig[i].controlRow &&
            col == semaphoreConfig[i].controlColumn ) {
            QVector2D fromCellToCar( carRect.center() - cell.center() );
            if( fromCellToCar.normalized() == carMotion ) { // We've passed semaphore
                break;
            }
            if( fromCellToCar.length() <= 2 ) {
                blocked = ! semaphore[i]->isOpen();
                break;
            }
        }
    }

    if( ! blocked ) {
        static struct _interveneCell_t {
            int row, column;
            int direction;
            int checkRow, checkColumn;
        } interveneCell[] = {
            {6, 0, 0x01, 5, 1},
            {0, 4, 0x04, 1, 3},
            {1, 4, 0x02, 2, 5},
            {10, 5,0x08, 9, 4},
            {6, 18,0x02, 7, 19}
        };
        int interveneCellCount = sizeof(interveneCell) / sizeof(struct _interveneCell_t);

        for(int i = 0 ; i < interveneCellCount ; i++) {
            if( carRect.center() == cellRect( interveneCell[i].row, interveneCell[i].column ).center() ) {
                // We're right at the cell; neither advancing in it, nor leaving it
                if(interveneCell[i].direction == pCar->direction()) {
                    QRectF checkCellRect = cellRect(interveneCell[i].checkRow, interveneCell[i].checkColumn);
                    for( int j = 0 ; j < car.count() ; j++ ) {
                        if( pCar != car[j] ) {
                            if( car[j]->boundingRect().translated(car[j]->pos()).intersects(checkCellRect) ) {
                                blocked = true;
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    if( ! blocked ) {
        qreal dx = pCar->boundingRect().width() / 2.0;
        QRectF controlRect = pCar->boundingRect().translated(-dx, -10.0);

        controlRect = QTransform().rotate(pCar->rotation()).mapRect(controlRect);
        controlRect.translate( pCar->pos() );

        for( int i = 0 ; i < car.count() ; i++ ) {
            if( pCar != car[i] ) {
                QTransform t = QTransform().rotate(car[i]->rotation());
                QRectF r = t.mapRect(car[i]->boundingRect()).translated(car[i]->pos());
                if( r.intersects(controlRect) ) {
                    blocked = true;
                    break;
                }
            }
        }
    }

    return blocked;
}


void Scene::enterAddCarWith(const QColor & color)
{
    _addCarColor = color;
    sceneControlState = ADD_CAR;
}

void Scene::enterDelCarMode()
{
    sceneControlState = DEL_CAR;
}

void Scene::addCarAt(const QColor & color, int row, int col)
{
    if( lanes[row][col] != 0 ) {
        QRectF newCarRect( col * scaleFactor, row * scaleFactor, scaleFactor, scaleFactor );
        QList<QGraphicsItem*> itemsInRect = items(newCarRect);
        if( itemsInRect.count() > 1 ) {
            QMessageBox::warning(NULL, tr("Used cell"), tr("Couldn't place car one upon another!"));
        } else {
            Car * pCar = new Car(color);

            addItem(pCar);
            car.append(pCar);
            pCar->setScale(scaleFactor);
            pCar->setCell(row, col);
        }
    } else {
        QMessageBox::warning(NULL, tr("Error"), tr("Car can be placed only at lane!"));
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if( (sceneControlState != NOCONTROL) && event->button() == Qt::LeftButton ) {
        if( sceneControlState == ADD_CAR ) {
            int _col = (int)(event->scenePos().x()) / scaleFactor,
                _row = (int)(event->scenePos().y()) / scaleFactor;
            addCarAt(_addCarColor, _row, _col);
            sceneControlState = NOCONTROL;
        }
        if( sceneControlState == DEL_CAR ) {
            Car * pCar = dynamic_cast<Car*>(itemAt(event->scenePos()));
            if( pCar != NULL ) {
                bool timerActive = (animationTimer != -1);
                if( timerActive ) {
                    killTimer(animationTimer);
                }

                removeItem(pCar);
                delete pCar;
                car.remove(car.indexOf(pCar));

                if( timerActive ) {
                    animationTimer = startTimer(timerInterval);
                }

                sceneControlState = NOCONTROL;
            } else {
                QMessageBox::information(NULL, tr("Miss car"), tr("Click on car to remove it"));
            }
        }
    } else {
        QGraphicsScene::mousePressEvent(event);
    }
}

void Scene::timerEvent(QTimerEvent */*event*/)
{
    advance();
}

// Slots
void Scene::setPause(bool bPause)
{
    if( bPause ) {
        killTimer(animationTimer);
        animationTimer = -1;
    } else {
        animationTimer = startTimer(timerInterval);
    }
}
