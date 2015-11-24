/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "arrow.h"

#include <math.h>

#include <QPen>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

const qreal Pi = 3.14;

Arrow::Arrow(BuilderItem *startItem, BuilderItem *endItem, Manager *safeManager, bool isInApp,BuilderItem *parent)
    : BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    myStartItem = startItem;
    myEndItem = endItem;
    connected = false;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemClipsToShape);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsMovable,true);
    //setFlag(QGraphicsItem::ItemIsMovable, true);
    myColor = Qt::black;
    this->textLbl = NULL;
    QString label;
    this->isInApp = isInApp;


    firstTime = true;

    setToolTip(QString("%1 --> %2").arg(startItem->itemName).arg(endItem->itemName));


//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
//    effect->setBlurRadius(5);
//    setGraphicsEffect(effect);

    QFontMetrics fontMetric(font);
    textWidth = fontMetric.width(label);

    bool bExternTo = false;
    bool bExternFrom = false;

    InputData* input = NULL;
    OutputData* output = NULL;

    string strFrom,strTo ;


    if(endItem->type() == (QGraphicsItem::UserType + (int)ModulePortItemType)){
        PortItem *port = ((PortItem*)endItem);
        ModuleItem *module = (ModuleItem *)port->parentItem();
        int portType = port->getPortType();
        if(portType == INPUT_PORT){
            input = port->getInputData();
            strTo = string(module->getInnerModule()->getPrefix()) + string(port->getInputData()->getPort());
            label = QString("%1").arg(port->getInputData()->getCarrier());
        }

    }else if(startItem->type() == (QGraphicsItem::UserType + (int)DestinationPortItemType)){

    }

    if(startItem->type() == (QGraphicsItem::UserType + (int)ModulePortItemType)){
        PortItem *port = ((PortItem*)startItem);
        ModuleItem *module = (ModuleItem *)port->parentItem();
        int portType = port->getPortType();
        if(portType == OUTPUT_PORT){
            output = port->getOutputData();
            strFrom = string(module->getInnerModule()->getPrefix()) + string(port->getOutputData()->getPort());
            label = QString("%1").arg(port->getOutputData()->getCarrier());
        }

    }else if(startItem->type() == (QGraphicsItem::UserType + (int)SourcePortItemType)){

    }


    connection.setFrom(strFrom.c_str());
    connection.setTo(strTo.c_str());
    connection.setCarrier(label.toLatin1().data());
    connection.setFromExternal(bExternFrom);
    connection.setToExternal(bExternTo);
    connection.setCorOutputData(output);
    connection.setCorInputData(input);
    //connection.setModel(this);
    Application* mainApplication = safeManager->getKnowledgeBase()->getApplication();
    connection = safeManager->getKnowledgeBase()->addConnectionToApplication(mainApplication, connection);

    if(!label.isEmpty()){
        textLbl = new Label(label,this);
        qDebug() << textLbl->type();
    }


}

Arrow::Arrow(BuilderItem *startItem, BuilderItem *endItem, Connection conn, int id, bool isInApp,BuilderItem *parent)
    : BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    itemType = ConnectionItemType;
    connected = false;
    myStartItem = startItem;
    myEndItem = endItem;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemClipsToShape);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsMovable,true);
    //setFlag(QGraphicsItem::ItemIsMovable, true);
    myColor = Qt::black;
    this->textLbl = NULL;
    this->connection = conn;
    this->id = id;
    this->isInApp = isInApp;
    QString label = conn.carrier();
    if(!label.isEmpty()){
        textLbl = new Label(label,this);
        qDebug() << textLbl->type();
    }
    externalSelection = false;

    firstTime = true;

    setToolTip(QString("%1 --> %2").arg(startItem->itemName).arg(endItem->itemName));


//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
//    effect->setBlurRadius(5);
//    setGraphicsEffect(effect);

    QFontMetrics fontMetric(font);
    textWidth = fontMetric.width(label);



}

Arrow::~Arrow()
{
    startItem()->removeArrow(this);
    endItem()->removeArrow(this);

    foreach (LineHandle *handle, handleList) {
        //scene()->removeItem(handle);
        delete handle;
    }
    handleList.clear();

    if(textLbl){
        //scene()->removeItem(textLbl);
        delete textLbl;
    }

    scene()->removeItem(this);
}
void Arrow::setConnectionSelected(bool selected)
{
    externalSelection = true;
    setSelected(selected);
}

QPointF Arrow::connectionPoint()
{
    QPointF startIngPoint;

    return startIngPoint;
}

QString Arrow::getFrom()
{
    return QString("%1").arg(connection.from());
}

QString Arrow::getTo()
{
    return QString("%1").arg(connection.to());
}

void Arrow::setConnected(bool connected)
{
    this->connected = connected;
    update();
}

int Arrow::getId()
{
    return id;
}

QRectF Arrow::boundingRect() const
{
    qreal extra = (/*pen().width()*/2 + 30) / 2.0;

    return QRectF(boundingPolyline.boundingRect())
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath Arrow::shape() const
{
    QPainterPath path;// = QGraphicsItem::shape();
    QPainterPathStroker pathStroke;
    pathStroke.setWidth(8);
    for(int i=0;i<boundingPolyline.count();i++){
        QPointF p = boundingPolyline.at(i);
        if(i==0){
            path.moveTo(p);
            continue;
        }
        path.lineTo(p.x(),p.y());
    }

    //path.addPolygon(arrowHead);
    return pathStroke.createStroke(path);
}

void Arrow::updatePosition()
{

    if(boundingPolyline.isEmpty()){
        boundingPolyline.prepend(mapFromItem(myStartItem,myStartItem->connectionPoint()));
        boundingPolyline.append( mapFromItem(myEndItem,myEndItem->connectionPoint()));
    }

    update();
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
          QWidget *)
{
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    painter->setClipRect( option->exposedRect );

    qreal arrowSize = 10;


    startP = mapFromItem(myStartItem,myStartItem->connectionPoint());
    endP = mapFromItem(myEndItem,myEndItem->connectionPoint());
    polyline.clear();


    polyline.prepend(startP);
    foreach (LineHandle *handle, handleList) {
        QPointF handlePoint = handle->handlePoint();
        polyline.append(handlePoint);
        if(isSelected()){
            handle->show();
        }else{
            handle->hide();
        }
    }
    polyline.append(endP);
    boundingPolyline = polyline;


    QLineF lastLine(polyline.at(polyline.count()-2),
                      polyline.last());
    QPointF p1 = lastLine.p1();
    QPointF p2 = lastLine.p2();

    QLineF line = QLineF(p2,p1);

    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0){
        angle = (Pi * 2) - angle;
    }

    QPointF arrowP1 = line.p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;
    if (isSelected()) {
        painter->setPen(QPen(Qt::blue, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    }else{
        if(connected){
            painter->setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }else{
            painter->setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }


    }
    if(connected){
        painter->setBrush(Qt::green);
    }else{
        painter->setBrush(Qt::red);
    }
    painter->drawPolyline(polyline);
    painter->drawPolygon(arrowHead);


    if(textLbl){
        QPointF centerP = QPointF((p1.x() + p2.x())/2 - textWidth/2,((p1.y() + p2.y())/2) - 5);
        if(firstTime){
            //scene()->addItem(this->textLbl);
            firstTime = false;
            textLbl->setPos(0,0);
        }
        textLbl->setPos(centerP);
    }


}




void Arrow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isInApp){
        QPointF clickPos = event->pos();
        addHandle(clickPos);

        updatePosition();
    }

    QGraphicsItem::mouseDoubleClickEvent(event);
}

void Arrow::addHandle(QPointF clickPos)
{
    LineHandle *handle = new LineHandle(clickPos,this);
    handle->setSelected(true);
    if(handleList.isEmpty()){
        handleList.append(handle);
    }else{
        bool inserted = false;
        for(int i=0;i<handleList.count();i++){
            LineHandle *hndl = handleList.at(i);
            if(hndl->x() > clickPos.x()){
                handleList.insert(i,handle);
                inserted = true;
                break;
            }
        }
        if(!inserted){
            handleList.append(handle);
        }
    }
}

void Arrow::removeHandle(LineHandle *handle)
{
    handleList.removeOne(handle);
    update();
}
QList <LineHandle*> Arrow::handles()
{
    return handleList;
}

QVariant Arrow::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {

        if(snap){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        //bool selected = value.toBool();
        if(!externalSelection){
            sigHandler->connectctionSelected(this);
        }
        externalSelection = false;
    }

    return value;
}

/******************************************************************/

LineHandle::LineHandle(QPointF center, Arrow *parent) : QGraphicsRectItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);

    rectSize = 8;
    setRect( - rectSize/2, -rectSize/2, rectSize,rectSize);
    setPos(center);
    setPen(QPen(QColor(Qt::black),1));
    setBrush(QBrush(QColor(Qt::red)));

    this->parent = parent;
    pressed = false;
    this->center = center;
    qDebug() << "CENTER CREATED IN " << center;
}

LineHandle::~LineHandle()
{
    parent->removeHandle(this);
    scene()->removeItem(this);
}

QPointF LineHandle::handlePoint()
{
    return QPointF(x(),y());
}

void LineHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    QPainterPath newPath;

    newPath.addRect(scenePos().x() + rect().x() - 2,scenePos().y() + rect().y() - 2 ,rect().width() + 4,rect().height()+4);
    qDebug() << newPath;

    scene()->setSelectionArea(newPath);
    parent->update();
    QGraphicsRectItem::mousePressEvent(event);
}

void LineHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(pressed){
        parent->setFlag(QGraphicsItem::ItemIsMovable,false);
        parent->update();

        qDebug() << "CENTER MOVED" << rect().center();
        qDebug() << "rect " << this->x();
    }

    QGraphicsRectItem::mouseMoveEvent(event);
}

void LineHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = false;

    QGraphicsRectItem::mouseReleaseEvent(event);

    QPainterPath newPath;
    newPath.addRect(scenePos().x() + rect().x() - 2,scenePos().y() + rect().y() - 2 ,rect().width() + 4,rect().height()+4);
    qDebug() << newPath;

    scene()->setSelectionArea(newPath);
    parent->setFlag(QGraphicsItem::ItemIsMovable,true);
    parent->update();
}

void LineHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    QRectF myRect = rect();

    if(isSelected()){
        painter->setPen(QPen(QColor(Qt::black),1,Qt::DashLine));
        painter->setBrush(QBrush(QColor(Qt::blue)));
    }else{
        painter->setPen(QPen(QColor(Qt::black),1));
        painter->setBrush(QBrush(QColor(Qt::red)));
    }


    painter->drawRect(myRect);
}


/**********************************************************************/

Label::Label(QString label, QGraphicsItem *parent) : QGraphicsTextItem(label,parent){

    sigHandler = new ItemSignalHandler((QGraphicsItem*)this,ArrowLabelItemType,NULL);
    comboWidget = new QGraphicsProxyWidget(this);
    QComboBox *combo = new QComboBox();
    combo->setEditable(true);
    QObject::connect(combo,SIGNAL(activated(QString)),
                     sigHandler,SLOT(onConnectionComboChanged(QString)));
    this->text = label;
    combo->addItem("tcp");
    combo->addItem("udp");
    comboWidget->setWidget(combo);
    comboWidget->setVisible(false);

}
Label::~Label()
{
    scene()->removeItem(this);
}
void Label::currentComboTextChanged(QString text){
    setPlainText(text);
    comboWidget->setVisible(false);
}
void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    if(comboWidget->isVisible()){
        comboWidget->setVisible(false);
    }else{
        comboWidget->setVisible(true);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

//void Label::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
//{
//    if(!isSelected()){
//        comboWidget->setVisible(false);
//    }else{
//        comboWidget->setVisible(true);
//    }
//    QGraphicsTextItem::paint(painter,option,widget);
//}

//QPointF Label::connectionPoint()
//{
//    QPointF startIngPoint;

//    return startIngPoint;
//}

//QRectF Label::boundingRect() const
//{
//    //QRectF br = QRectF(boundingR.x() - BORDERWIDTH, boundingR.y() - BORDERWIDTH , boundingR.width() + (2*BORDERWIDTH), boundingR.height() + (2*BORDERWIDTH));
//    //qDebug() << br;
//    return boundingR;
//}
