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

#ifndef ARROW_H
#define ARROW_H

#include <QStaticText>
#include <QGraphicsLineItem>
#include <QGraphicsProxyWidget>
#include <QComboBox>
#include "commons.h"
#include "itemsignalhandler.h"
#include "builderitem.h"



QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE

//class BuilderItem;
class LineHandle;
class Label;
//class ItemSignalHandler;

class Arrow : public BuilderItem
{
public:

    friend class LineHandle;

    Arrow(BuilderItem *startItem, BuilderItem *endItem, Manager *safeManager, bool isInApp = false,
      BuilderItem *parent = 0);
    Arrow(BuilderItem *startItem, BuilderItem *endItem, yarp::manager::Connection connection, int id,
          bool isInApp = false,
      BuilderItem *parent = 0);
    QPointF connectionPoint();
    void setConnected(bool);
    int getId();
    QString getFrom();
    QString getTo();

    void setConnectionSelected(bool selected);

    ~Arrow();
    int type() const  { return (int)QGraphicsItem::UserType + (int)itemType; }
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void setColor(const QColor &color) { myColor = color; }
    BuilderItem *startItem() const { return myStartItem; }
    BuilderItem *endItem() const { return myEndItem; }

    void addHandle(QPointF clickPos);
    void updatePosition();
    QList <LineHandle*> handles();
    void removeHandle(LineHandle*);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    bool externalSelection;
    bool isInApp;
    bool connected;
    yarp::manager::Connection connection;
    BuilderItem *myStartItem;
    BuilderItem *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
    Label *textLbl;
    int textWidth;
    QFont font;
    //QLineF line;
    QPolygonF polyline;
    QPolygonF boundingPolyline;
    int id;

    QPointF startP;
    QPointF endP;

    bool firstTime;

    QList <LineHandle*> handleList;
};


class LineHandle : public QGraphicsRectItem
{
public:
    LineHandle(QPointF center, Arrow *parent = 0);
    ~LineHandle();
    QPointF handlePoint();
    int type() const  { return UserType + (int)HandleItemType; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
private:
    QPointF center;
    Arrow *parent;
    bool pressed;
    int rectSize;
};


class Label : public QGraphicsTextItem
{
public:
    Label(QString label, QGraphicsItem *parent = 0);
    ~Label();
    int type() const  { return UserType + (int)ArrowLabelItemType; }
//    QRectF boundingRect() const;
//    QPointF connectionPoint();

    void currentComboTextChanged(QString text);

protected:
    //void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
protected:
    QGraphicsProxyWidget *comboWidget;
    QString text;
    ItemSignalHandler *sigHandler;
};

#endif // ARROW_H
