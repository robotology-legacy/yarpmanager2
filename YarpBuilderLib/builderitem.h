#ifndef BUILDERITEM_H
#define BUILDERITEM_H

#include <QObject>
#include <QAction>
#include <QPainter>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <yarp/manager/manager.h>

#include "commons.h"


using namespace std;
using namespace yarp::manager;

class PortItem;
class ItemSignalHandler;
class Arrow;



class BuilderItem :   public QGraphicsObject
{
    friend class PortItem;
    friend class Arrow;
    friend class Label;

public:

    BuilderItem(QGraphicsObject * parent = 0): QGraphicsObject(parent){
        snap = false;
        offset = QPointF(0,0);
    }
    virtual QRectF boundingRect() const = 0;
    virtual QPointF connectionPoint() = 0;
    virtual int type() const  = 0;

    QString getItemName();
    QPointF getStartingPoint();
    ItemSignalHandler *signalHandler();
    void removeArrow(Arrow *arrow);
    void removeArrows();
    void addArrow(Arrow *arrow);

    bool arrowAlreadyPresent(BuilderItem *endItem);
    void updateConnections();
    bool allowInputConnections();
    bool allowOutputConnections();
    void snapToGrid(bool snap);
    QPointF computeTopLeftGridPoint(const QPointF &pointP);
    QList<QAction*> getActions();
    void setActions(QList<QAction*>);
    bool isNestedInApp();


protected:
    QFont font;
    ItemType itemType;
    QString itemName;

    QRectF mainRect;
    QRectF boundingR;

    QPointF offset;

    bool pressed;
    bool moved;
    bool creatingNewConnection;
    bool allowInputs;
    bool allowOutputs;
    bool snap;
    bool nestedInApp;

    ItemSignalHandler *sigHandler;
    QList<Arrow *> arrows;
    QList<QAction*> actions;

    QPointF startingPoint;

};

#endif // BUILDERITEM_H
