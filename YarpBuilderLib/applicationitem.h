#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include "builderitem.h"

class ApplicationItem : public BuilderItem
{
public:
    ApplicationItem(Application* application, Manager *manager, QList <int> *, bool isInApp = false, BuilderItem *parent = NULL);
    ~ApplicationItem();

    QRectF boundingRect() const;
    QPointF connectionPoint();
    int type() const ;
    void init();
    QList<QGraphicsItem *> *getModulesList();
    Application* getInnerApplication();
    void setModuleRunning(bool,int);
    void setConnectionConnected(bool connected, QString from, QString to);
    void setSelectedModules(QList<int>selectedIds);
    void setOutputPortAvailable(QString oData, bool available);
    void setInputPortAvailable(QString iData, bool available);

private:
    void updateBoundingRect();
    void updateSizes(BuilderItem *it);
    void findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                             InputData* &input_, OutputData* &output_);
    PortItem* findModelFromOutput(OutputData* output);
    PortItem* findModelFromInput(InputData* input);

    BuilderItem* addModule(Module *module, int moduleId);
    qreal minx,miny,maxw,maxh;



protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


private:
    bool isInApp;
    QList <int> *usedModulesId;
    QFont customFont;
    int textWidth ;
    Application* application;
    Manager *mainAppManager;
    int index;
    QList <QGraphicsItem*> itemsList;

};

#endif // APPLICATIONITEM_H
