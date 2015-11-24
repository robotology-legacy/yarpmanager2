#include "moduleitem.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>


#define TRIANGLEH   (double)((double)PORT_LINE_WIDTH * (double)sqrt(3.0) ) / 2.0

//ModuleItem::ModuleItem(QString itemName, QStringList inputPorts, QStringList outputPorts, BuilderItem *parent) : BuilderItem(parent)
//{
//    this->itemType = ModuleItemType;
//    this->itemName = itemName;
//    this->inputPorts = inputPorts;
//    this->outputPorts = outputPorts;
//    running = false;

//    init();
//}

ModuleItem::ModuleItem(Module *module, int moduleId, bool isInApp,BuilderItem * parent) : BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    this->itemType = ModuleItemType;
    this->module = module;
    this->moduleId = moduleId;
    this->isInApp = isInApp;
    running = false;
    externalSelection = false;
    startingPoint = QPointF(10,10);
    itemName = QString("%1").arg(module->getName());

//    for(int i=0; i<module->inputCount();i++){

//        inputPorts << QString("%1").arg(module->getInputAt(i).getName());
//    }
//    for(int i=0; i<module->outputCount();i++){
//        OutputData out = module->getOutputAt(i);
//        outputPorts << QString("%1").arg(out.getName());
//    }

    if(module->getModelBase().points.size()>0){
        startingPoint = QPointF(module->getModelBase().points[0].x,module->getModelBase().points[0].y);
    }



    init();


}

int ModuleItem::getId()
{
    return moduleId;
}

void ModuleItem::setRunning(bool running)
{
    this->running = running;
    update();
}

void ModuleItem::init()
{

    setFlag(ItemIsMovable,!isInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,true);

    pressed = false;
    moved = false;

    setToolTip(itemName);


    QFontMetrics fontMetric(font);
    int textWidth = fontMetric.width(itemName);

    prepareGeometryChange();
    mainRect = QRect(-((2*PORT_TEXT_WIDTH) + textWidth)/2,
                     -20,
                     ((2*PORT_TEXT_WIDTH) + textWidth),
                     40);

    boundingR = QRectF(mainRect);

    if(module->inputCount() > 0){

        for(int i=0;i<module->inputCount();i++){
            iPorts.append(new PortItem(&module->getInputAt(i),this));
        }
        prepareGeometryChange();
        boundingR.setX(boundingR.x() - (PORT_LINE_WIDTH + TRIANGLEH / 2.0) );
//        boundingR.setWidth(boundingR.width() +  PORT_LINE_WIDTH + TRIANGLEH );



        int newH = module->inputCount() * 2 * PORT_LINE_WIDTH;
        if(mainRect.height() <= newH){

            mainRect.setY(mainRect.y() - newH/2);
            mainRect.setHeight(mainRect.height() + newH/2);
            boundingR.setY(mainRect.y());
            boundingR.setHeight(mainRect.height());
        }
    }

    if(module->outputCount() > 0){
        //prepareGeometryChange();
        //boundingR.setWidth(mainRect.width() + PORT_LINE_WIDTH);
        for(int i=0;i<module->outputCount();i++){
            oPorts.append(new PortItem(&module->getOutputAt(i),this));
        }
        prepareGeometryChange();
        boundingR.setWidth(boundingR.width() +  (PORT_LINE_WIDTH +TRIANGLEH/2.0) );

        int newH = module->outputCount() * 2 * PORT_LINE_WIDTH;
        if(mainRect.height() <= newH){

            mainRect.setY(mainRect.y() - newH/2);
            mainRect.setHeight(mainRect.height() + newH/2);
            boundingR.setY(mainRect.y());
            boundingR.setHeight(mainRect.height());
        }
    }


    if(!isInApp){
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setColor(QColor(80,80,80,80));
        effect->setBlurRadius(5);
        setGraphicsEffect(effect);
    }

    allowInputs = true;
    allowOutputs = true;
}

ModuleItem::~ModuleItem()
{
    foreach (PortItem *port, iPorts) {
        port->removeArrows();
    }
    foreach (PortItem *port, oPorts) {
        port->removeArrows();
    }
    delete sigHandler;
    scene()->removeItem(this);
}



int ModuleItem::type() const
{
    return (int)(QGraphicsItem::UserType + (int)itemType);
}
void ModuleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //painter->fillRect(boundingR,QBrush(Qt::red));
    //Input Ports
    qreal partialH = mainRect.height()/(module->inputCount() + 1);
    for(int i=0; i < module->inputCount(); i++){
        painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));
        painter->drawLine(mainRect.x() - PORT_LINE_WIDTH, mainRect.y() + ((i+1) * partialH ),mainRect.x() , mainRect.y() + ((i+1) * partialH ));
        PortItem *it = iPorts.at(i);
        it->setPos(mainRect.x() - PORT_LINE_WIDTH,mainRect.y() + ((i+1) * partialH ));
    }

    //Output Ports ?????????
    partialH = mainRect.height()/(module->outputCount() + 1);
    for(int i=0; i < module->outputCount(); i++){
        painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));
        painter->drawLine(mainRect.x() + mainRect.width() , mainRect.y() + ((i+1) * partialH ),
                          mainRect.x() + mainRect.width() + PORT_LINE_WIDTH , mainRect.y() + ((i+1) * partialH ));
        PortItem *it = oPorts.at(i);
        it->setPos(mainRect.x() + mainRect.width() + PORT_LINE_WIDTH,mainRect.y() + ((i+1) * partialH ));
    }


    if(!isInApp){
        if(!running){
            painter->setPen(QPen(QBrush(QColor("#BF0303")),BORDERWIDTH ));
        }else{
            painter->setPen(QPen(QBrush(QColor("#00E400")),BORDERWIDTH ));
        }
    }else{
        if(!running){
            painter->setPen(QPen(QBrush(QColor("#BF0303")),BORDERWIDTH, Qt::DashLine ));
        }else{
            painter->setPen(QPen(QBrush(QColor("#00E400")),BORDERWIDTH , Qt::DashLine));
        }
    }
    QPainterPath path;
    path.addRoundedRect( mainRect, 5.0, 5.0 );
    if (isSelected()) {
        painter->setBrush(QBrush(QColor(220,220,220)));
    }else{
        painter->setBrush(QBrush(QColor(Qt::white)));
    }


    painter->drawPath(path);

//    painter->setBrush(QBrush(QColor(Qt::red)));
//    painter->drawRect(-10,-10,20,20);


    //TEXT
    painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
    painter->drawText(mainRect,Qt::AlignCenter,itemName);

}


QRectF ModuleItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH, boundingR.y() - BORDERWIDTH , boundingR.width() + (2*BORDERWIDTH), boundingR.height() + (2*BORDERWIDTH));
    //qDebug() << br;
    return br;
}




void ModuleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    moved = true;
//    if(pressed){
//        foreach (PortItem *port, iPorts) {
//            port->updateConnections();
//        }
//        foreach (PortItem *port, oPorts) {
//            port->updateConnections();
//        }
//    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ModuleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    setZValue(zValue() + 10);
    QGraphicsItem::mousePressEvent(event);
    //sigHandler->moduleSelected(this);
    if(isInApp && isSelected()){
        parentItem()->setSelected(true);
    }
    QGraphicsItem::mousePressEvent(event);
}

QPointF ModuleItem::connectionPoint()
{
    QPointF startIngPoint;

    return startIngPoint;
}

void ModuleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    pressed = false;
    moved = false;
    setZValue(zValue() - 10);
    QGraphicsItem::mouseReleaseEvent(event);
    if(isInApp && isSelected()){
        parentItem()->setSelected(true);
    }
}

void ModuleItem::setModuleSelected(bool selected)
{
    externalSelection = true;
    setSelected(selected);
}

QVariant ModuleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (PortItem *port, iPorts) {
            port->updateConnections();
        }
        foreach (PortItem *port, oPorts) {
            port->updateConnections();
        }
        if(snap  && !isInApp){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        //bool selected = value.toBool();
        if(!externalSelection){
            sigHandler->moduleSelected(this);
        }
        externalSelection = false;
    }

    return value;
}

void ModuleItem::portPressed(PortItem *port,QGraphicsSceneMouseEvent *e)
{
    pressed = true;
}

Module *ModuleItem::getInnerModule()
{
    return module;
}

void ModuleItem::portReleased(PortItem *port,QGraphicsSceneMouseEvent *e)
{
//    if(!moved){
//        sigHandler->newConnection(mapFromItem(port,port->connectionPoint()),this);
//    }
//    pressed = false;
//    moved = false;
    qDebug() << "MODULE RELEASED";
}

void ModuleItem::portMoved(PortItem *port,QGraphicsSceneMouseEvent *e)
{
//    if(!moved){
//        sigHandler->newConnection(mapFromItem(port,port->connectionPoint()),this);
//    }
//    pressed = false;
//    moved = false;
}



/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/
/********************************************************************************************************************/

PortItem::PortItem(QString portName, int type, BuilderItem *parent) : BuilderItem(parent)
{
    triangleH = (PORT_LINE_WIDTH/2)* sqrt(3.0);
    portAvailable = false;

    polygon << QPointF(-triangleH/2, - PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2);

    setAcceptHoverEvents(true);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsSelectable,true);

    boundingR = QRectF(-triangleH/2, - PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);


    this->itemName = portName;
    setToolTip(itemName);
    this->parent = parent;
    portType = type;

    sigHandler = NULL;
    pressed = false;
    moved = false;
    hovered =false;
    if(type == INPUT_PORT){
        allowInputs = true;
        allowOutputs = false;
    }
    if(type == OUTPUT_PORT){
        allowInputs = false;
        allowOutputs = true;
    }

    itemType = ModulePortItemType;
}
PortItem::PortItem(InputData *node, BuilderItem *parent) : BuilderItem(parent)
{
    triangleH = (PORT_LINE_WIDTH/2)* sqrt(3.0);
    inData = node;
    portAvailable = false;

    polygon << QPointF(-triangleH/2, - PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2);

    setAcceptHoverEvents(true);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsSelectable,true);

    boundingR = QRectF(-triangleH/2, - PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);

    this->itemName = node->getPort();
    setToolTip(itemName);
    this->parent = parent;
    portType = INPUT_PORT;

    sigHandler = NULL;
    pressed = false;
    moved = false;
    hovered =false;
    if(portType == INPUT_PORT){
        allowInputs = true;
        allowOutputs = false;
    }
    if(portType == OUTPUT_PORT){
        allowInputs = false;
        allowOutputs = true;
    }
    itemType = ModulePortItemType;
}

PortItem::PortItem(OutputData* node, BuilderItem *parent) : BuilderItem(parent)
{
    triangleH = (PORT_LINE_WIDTH/2)* sqrt(3.0);
    outData = node;
    portAvailable = false;
    polygon << QPointF(-triangleH/2, - PORT_LINE_WIDTH/2) << QPointF(triangleH/2, 0) << QPointF(-triangleH/2, PORT_LINE_WIDTH/2);

    setAcceptHoverEvents(true);
    setFlag(ItemSendsGeometryChanges,true);
    setFlag(ItemIsSelectable,true);

    boundingR = QRectF(-triangleH/2, - PORT_LINE_WIDTH/2,triangleH,PORT_LINE_WIDTH);

    this->itemName = node->getPort();
    setToolTip(itemName);
    this->parent = parent;
    portType = OUTPUT_PORT;

    sigHandler = NULL;
    pressed = false;
    moved = false;
    hovered =false;
    if(portType == INPUT_PORT){
        allowInputs = true;
        allowOutputs = false;
    }
    if(portType == OUTPUT_PORT){
        allowInputs = false;
        allowOutputs = true;
    }
    itemType = ModulePortItemType;
}

void PortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH/2));
    if(!portAvailable){
        if(!hovered){
            painter->setBrush(QBrush(QColor("#F74D4D")));
        }else{
            painter->setBrush(QBrush(QColor("#BF0303")));
        }
    }else{
        if(!hovered){
            painter->setBrush(QBrush(QColor("#1CE61C")));
        }else{
            painter->setBrush(QBrush(QColor("#008C00")));
        }
    }

    painter->drawPolygon(polygon);
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    hovered = true;
    update();

}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    hovered = false;
    update();
}

int PortItem::type() const
{
    return UserType + (int)itemType;
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PORT PRESSED";
    //parent->portPressed(this,event);
    pressed = true;
    QGraphicsItem::mousePressEvent(event);
}

void PortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PORT RELEASED";
    //parent->portReleased(this,event);
    if(!moved && event->modifiers() == Qt::NoModifier && event->button() == Qt::LeftButton){
        if(portType == OUTPUT_PORT){
            parent->signalHandler()->newConnectionRequested(connectionPoint(),this);
        }else{
            parent->signalHandler()->newConnectionAdded(connectionPoint(),this);
        }
    }
    pressed = false;
    moved = false;
    //QGraphicsItem::mouseReleaseEvent(event);
}

void PortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PORT MOVED";
    //parent->portReleased(this,event);
    moved = true;
    QGraphicsItem::mouseMoveEvent(event);
}

QPointF PortItem::connectionPoint()
{
    switch (portType) {
    case INPUT_PORT:
        return QPointF(-triangleH/2, - 0);
    case OUTPUT_PORT:
        return QPointF(triangleH/2, - 0);
    }
}

int PortItem::getPortType()
{
    return portType;
}


QRectF PortItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH, boundingR.y() - BORDERWIDTH , boundingR.width() + (2*BORDERWIDTH), boundingR.height() + (2*BORDERWIDTH));
    return br;
}

InputData *PortItem::getInputData()
{
    return inData;
}

OutputData *PortItem::getOutputData()
{
    return outData;
}


QVariant PortItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
//    if (change == QGraphicsItem::ItemPositionChange) {
//        foreach (Arrow *arrow, arrows) {
//            arrow->updatePosition();
//        }
////        if(snap  && !isInApp){
////            QPointF newPos = value.toPointF();
////            QPointF closestPoint = computeTopLeftGridPoint(newPos);
////            return closestPoint+=offset;
////        }
//    }

    return value;
}

void PortItem::setAvailable(bool available)
{
    portAvailable = available;
    update();
}
