#ifndef ITEMSIGNALHANDLER_H
#define ITEMSIGNALHANDLER_H

#include <QObject>
#include <QGraphicsItem>
#include "moduleitem.h"
//#include "arrow.h"

class Label;

class ItemSignalHandler : public QObject
{
    Q_OBJECT

public:
    ItemSignalHandler(QObject *parent = NULL);
    ItemSignalHandler(QGraphicsItem *it, ItemType type, QObject *parent = NULL);
    void newConnectionRequested(QPointF, QGraphicsItem *it);
    void newConnectionAdded(QPointF, QGraphicsItem *it);

private:
       QGraphicsItem *parentItem;
       ItemType type;

signals:
    void addNewConnection(QPointF,QGraphicsItem *it);
    void requestNewConnection(QPointF,QGraphicsItem *it);
    void moduleSelected(QGraphicsItem *it);
    void applicationSelected(QGraphicsItem *it);

public slots:
    void onModuleSelected(QGraphicsItem *it);
    void onConnectionComboChanged(QString);
    void onEditingFinished();

};

#endif // ITEMSIGNALHANDLER_H
