#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include "builderitem.h"

class ApplicationItem : public BuilderItem
{
public:
    ApplicationItem(Application* application, Manager *lazyManager/*,Manager *manager*/, Manager *manager, BuilderItem *parent = 0);
    ~ApplicationItem();

    QRectF boundingRect() const;
    QPointF connectionPoint();
    int type() const ;
    void init();
    QList<QGraphicsItem *> *getModulesList();
    Application* getInnerApplication();
    void setModuleRunning(int,bool);
    void setConnectionConnected(bool connected, QString from, QString to);

private:
    void updateBoundingRect();
    void updateSizes(BuilderItem *it);
    void findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                             InputData* &input_, OutputData* &output_, QString *inModulePrefix, QString *outModulePrefix);
    PortItem* findModelFromOutput(OutputData* output, QString modulePrefix);
    PortItem* findModelFromInput(InputData* input, QString modulePrefix);

    BuilderItem* addModule(Module *module, int moduleId);
    qreal minx,miny,maxw,maxh;



protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


private:
    QFont customFont;
    int textWidth ;
    Application* application;
    Manager *lazyManager;
    Manager manager;
    Manager *mainAppManager;
    int index;
    QList <QGraphicsItem*> itemsList;

};

#endif // APPLICATIONITEM_H
