#ifndef BUILDERSCENE_H
#define BUILDERSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>

class MyView;

class BuilderScene : public QGraphicsScene
{
    friend class MyView;
    Q_OBJECT
public:
    explicit BuilderScene(QObject *parent = 0);


private:
    QGraphicsLineItem *currentLine;
    QPointF startingPoint;
    QGraphicsItem *startConnectionItem;
    QGraphicsItem *endConnectionItem;

protected:
    bool snap;


protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //void wheelEvent(QGraphicsSceneWheelEvent *event);

signals:
    void addedModule(void *mod,QPointF);
    void addNewConnection(void *start, void *end);

public slots:
    void snapToGrid(bool snap);
    void onNewConnectionRequested(QPointF, QGraphicsItem *item);
    void onNewConnectionAdded(QPointF, QGraphicsItem *item);
    void onSceneChanged(QList<QRectF> rects);

};

#endif // BUILDERSCENE_H
