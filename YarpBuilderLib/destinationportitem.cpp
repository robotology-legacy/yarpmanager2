#include "destinationportitem.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QLineEdit>


DestinationPortItem::DestinationPortItem(QString itemName, bool isInApp, BuilderItem *parent) : BuilderItem(parent)
{
    itemType = DestinationPortItemType;
    this->itemName = itemName;

    sigHandler = new ItemSignalHandler((QGraphicsItem*)this,DestinationPortItemType,NULL);
    pressed = false;
    moved = false;
    this->isInApp = isInApp;

    QFontMetrics fontMetric(font);
    int textWidth = fontMetric.width(itemName);

    prepareGeometryChange();
    mainRect = QRect(-((2*PORT_TEXT_WIDTH) + textWidth)/2,-15,(2*PORT_TEXT_WIDTH) + textWidth,30);

    boundingR = QRectF(mainRect);

    setToolTip(itemName);

    setFlag(ItemIsMovable,!isInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,true);

    if(!isInApp){
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setColor(QColor(80,80,80,80));
        effect->setBlurRadius(5);
        setGraphicsEffect(effect);
    }

    allowInputs = true;
    allowOutputs= false;

    lineEditWidget = new QGraphicsProxyWidget(this);
    QLineEdit *lineEdit = new QLineEdit();
    QObject::connect(lineEdit,SIGNAL(editingFinished()),signalHandler(),SLOT(onEditingFinished()));
    lineEdit->setText(itemName);
    lineEditWidget->setWidget(lineEdit);
    lineEditWidget->setVisible(false);
    QRectF geo = lineEditWidget->geometry();
    geo.setWidth(textWidth);
    lineEditWidget->setGeometry(geo);
    lineEditWidget->setPos(-textWidth/2,-lineEditWidget->geometry().height()/2);
}

DestinationPortItem::~DestinationPortItem()
{
    removeArrows();
    delete sigHandler;
    scene()->removeItem(this);
}



int DestinationPortItem::type() const
{
    return (int)(UserType + (int)itemType);
}
void DestinationPortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if(!isInApp){
        painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH));
    }else{
        painter->setPen(QPen(QBrush(QColor(Qt::black)),BORDERWIDTH,Qt::DashLine));
    }
    QPainterPath path;
    path.moveTo(mainRect.x(),mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(), mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x() +10 ,mainRect.y() + mainRect.height()/2);
    path.lineTo(mainRect.x(),mainRect.y());
    if (isSelected()) {
        painter->setBrush(QBrush(QColor(220,220,220)));
    }else{
        painter->setBrush(QBrush(QColor(245,245,245)));
    }
    painter->drawPath(path);

    //TEXT
    if(!lineEditWidget->isVisible()){
        painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
        painter->drawText(mainRect,Qt::AlignCenter,itemName);
    }else{
        if(!isSelected()){
            editingFinished();
            painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
            painter->drawText(mainRect,Qt::AlignCenter,itemName);
        }
    }

}

void DestinationPortItem::editingFinished()
{
    QString text = ((QLineEdit*)lineEditWidget->widget())->text();
    this->itemName = text;
    lineEditWidget->setVisible(false);

    QFontMetrics fontMetric(font);
    int textWidth = fontMetric.width(itemName);

    prepareGeometryChange();
    mainRect = QRect(-((2*PORT_TEXT_WIDTH) + textWidth)/2,-15,(2*PORT_TEXT_WIDTH) + textWidth,30);
    boundingR = QRectF(mainRect);
    setToolTip(itemName);

    QRectF geo = lineEditWidget->geometry();
    geo.setWidth(textWidth);
    lineEditWidget->setGeometry(geo);
    lineEditWidget->setPos(-textWidth/2,-lineEditWidget->geometry().height()/2);

    update();
    updateConnections();
}


void DestinationPortItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    lineEditWidget->setVisible(true);
    update();
    //QGraphicsItem::mouseDoubleClickEvent(event);
}

QRectF DestinationPortItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH, boundingR.y() - BORDERWIDTH , boundingR.width() + (2*BORDERWIDTH), boundingR.height() + (2*BORDERWIDTH));
    return br;
}




void DestinationPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    moved = true;
//    if(pressed){
//        ((DestinationPortItem*)this)->updateConnections();
//    }
    QGraphicsItem::mouseMoveEvent(event);
}

void DestinationPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    setZValue(zValue() + 1);
    editingFinished();
    QGraphicsItem::mousePressEvent(event);
}

QPointF DestinationPortItem::connectionPoint()
{
    QPointF startIngPoint;
    startIngPoint = QPointF(mainRect.x() +10 ,mainRect.y() + mainRect.height()/2);
    return startIngPoint;
}

void DestinationPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    pressed = false;
    moved = false;
    QGraphicsItem::mouseReleaseEvent(event);
}



QVariant DestinationPortItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
        if(snap){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }

    return value;
}

