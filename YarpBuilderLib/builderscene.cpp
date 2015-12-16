#include "builderscene.h"
#include <QDebug>
#include <QMimeData>
#include "moduleitem.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include <QCursor>
#include "arrow.h"
#include <QGraphicsView>
#include <QGraphicsSceneWheelEvent>

BuilderScene::BuilderScene(QObject *parent) :
    QGraphicsScene(parent)
{
    currentLine = NULL;
    startConnectionItem = NULL;
    //connect(this,SIGNAL(changed(QList<QRectF>)),this,SLOT(onSceneChanged(QList<QRectF>)));
    snap = false;
}



void BuilderScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "Enter Drag";
    QString itemType = event->mimeData()->text();
    if(itemType == "module" ||
       itemType == "application"){
        event->setAccepted(true);
        qDebug() << "Enter Drag ACCEPTED";
    }else{
        event->setAccepted(false);
        qDebug() << "Enter Drag REJECTED";
    }



}

void BuilderScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qlonglong pointer = event->mimeData()->data("pointer").toLongLong();
    QString itemType = event->mimeData()->text();

    if(itemType == "module" ){
        Module *mod = (yarp::manager::Module*)pointer;
        addedModule((void*)mod,event->scenePos());
    }
    if(itemType == "application" ){
        Application *app = (yarp::manager::Application*)pointer;
        addedApplication((void*)app,event->scenePos());
    }
//    int type = sType.toInt();
//    BuilderItem *it = NULL;

//    switch (type) {
//    case 1:
//        //it = new ModuleItem("Modulo",QStringList() << "input1" << "input2" ,QStringList() << "output1");
//        break;
//    case 2:
//        it = new SourcePortItem("SourcePort");
//        break;
//    case 3:
//        it = new DestinationPortItem("DestinationPort");
//        break;
//    default:
//        break;
//    }

//    it->snapToGrid(snap);

//    addItem(it);
//    it->setPos(event->scenePos());
//    connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),this,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
}

void BuilderScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(!editingMode){
        event->setAccepted(false);
        return;
    }
    event->setAccepted(true);
    //qDebug() << "Drag Move";
}

void BuilderScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mousePressEvent";
    if(currentLine){
        removeItem(currentLine);
        delete currentLine;
        currentLine = NULL;
    }

//    if(currentLine){
//        QList<QGraphicsItem *> startItems = items(currentLine->line().p1());
//        if (startItems.count() && startItems.first() == currentLine)
//            startItems.removeFirst();
//        QList<QGraphicsItem *> endItems = items(currentLine->line().p2());
//        if (endItems.count() && endItems.first() == currentLine)
//            endItems.removeFirst();

//        removeItem(currentLine);
//        delete currentLine;
//        currentLine = NULL;

//         if (startItems.count() <= 0 || endItems.count() <= 0){
//             return;
//         }

//         for(int i=startItems.count() - 1; i >= 0; i--){
//             BuilderItem *it = ((BuilderItem*)startItems.at(i));
//             if(it->type() != (QGraphicsItem::UserType + (int)ModulePortItemType) &&
//                 it->type() != (QGraphicsItem::UserType + (int)SourcePortItemType)){
//                 startItems.removeAt(i);
//            }
//         }
//         for(int i=endItems.count() - 1; i >= 0; i--){
//             BuilderItem *it = ((BuilderItem*)endItems.at(i));
//             if(it->type() !=(QGraphicsItem::UserType + (int)ModulePortItemType) &&
//                 it->type() != (QGraphicsItem::UserType + (int)DestinationPortItemType)){
//                 endItems.removeAt(i);
//            }
//         }

//         if (startItems.count() <= 0 || endItems.count() <= 0){
//             return;
//         }

//         BuilderItem *startItem = (BuilderItem*)startItems.first();
//         BuilderItem *endItem = (BuilderItem*)endItems.first();

//         if(startItem->type() == QGraphicsItem::UserType + (int)ModuleItemType || endItem->type() == QGraphicsItem::UserType + (int)ModuleItemType){
//             return;
//         }



//        if (startItem->allowOutputConnections() &&
//            endItem->allowInputConnections()) {

//            //qDebug() << endItems.first()->type();


////            BuilderItem *startItem = qgraphicsitem_cast<BuilderItem *>(startItems.first());
////            BuilderItem *endItem = qgraphicsitem_cast<BuilderItem *>(endItems.first());
//            if(!startItem->arrowAlreadyPresent(endItem)){
//                addNewConnection(startItem,endItem);

//            }



//        }
//    }
    QGraphicsScene::mousePressEvent(event);
}

//void BuilderScene::wheelEvent(QGraphicsSceneWheelEvent *event)
//{
//    qDebug() << event->pos();

//    if(event->modifiers() == Qt::CTRL){
//        if(event->delta() > 0){
//            views().first()->scale(1.1,1.1);
//        }else{
//            views().first()->scale(0.9,0.9);
//        }

//        views().first()->centerOn(event->pos());

//    }
//}

void BuilderScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void BuilderScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(currentLine){
        //qDebug() << startingPoint.x() << " || " << startingPoint.y() << " || " << QCursor::pos().x() << " || " << QCursor::pos().y();
        currentLine->setLine(startingPoint.x(),startingPoint.y(),event->scenePos().x(),event->scenePos().y());
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void BuilderScene::onNewConnectionRequested(QPointF p,QGraphicsItem *item)
{

    startConnectionItem = NULL;
    if(!editingMode || !((BuilderItem*)item)->allowOutputConnections()){
        return;
    }

    if(!currentLine){
        startConnectionItem = item;
        startingPoint = item->mapToScene(p);
        currentLine = new QGraphicsLineItem();
        addItem(currentLine);
    }else{
        removeItem(currentLine);
        delete currentLine;
        currentLine = NULL;
    }



}

void BuilderScene::onNewConnectionAdded(QPointF p,QGraphicsItem *item)
{
    qDebug() << "onNewConnectionAdded";
    if(!editingMode){
        return;
    }
    if(startConnectionItem){
        if(currentLine){
            removeItem(currentLine);
            delete currentLine;
            currentLine = NULL;
        }

            BuilderItem *startItem = (BuilderItem*)startConnectionItem;
            BuilderItem *endItem = (BuilderItem*)item;

            if(startItem->type() == QGraphicsItem::UserType + (int)ModuleItemType || endItem->type() == QGraphicsItem::UserType + (int)ModuleItemType){
                return;
            }



           if (startItem->allowOutputConnections() &&
               endItem->allowInputConnections()) {
               if(!startItem->arrowAlreadyPresent(endItem)){
                   addNewConnection(startItem,endItem);

               }
           }

        startConnectionItem = NULL;
    }

}

void BuilderScene::onSceneChanged(QList<QRectF> rects)
{
    foreach (QRectF r, rects) {
        QList<QGraphicsItem *> startItems = items(r);

        for(int i=0;i<startItems.count();i++){
            QGraphicsItem *gIt = startItems.at(i);
            if(gIt){
                if(gIt == currentLine){
                    return;
                }
                if(gIt->type() == QGraphicsItem::UserType + (int) ConnectionItemType){

                    return;

                }
                BuilderItem *it = (BuilderItem*)gIt;
                it->updateConnections();
                //qDebug() << "UPDATE";
            }
        }
    }
}

void BuilderScene::snapToGrid(bool snap)
{
    this->snap = snap;
    foreach (QGraphicsItem *it, items()) {
//        if(it->type() == QGraphicsItem::UserType + (int)ConnectionItemType ||
//            it->type() == QGraphicsItem::UserType + (int)HandleItemType){
//            return;
//        }

        ((BuilderItem*)it)->snapToGrid(snap);
    }
}
