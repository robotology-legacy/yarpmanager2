#ifndef BUILDERWINDOW_H
#define BUILDERWINDOW_H

#include <QWidget>
#include "builderscene.h"
#include <QGraphicsView>
#include "customtree.h"
#include <QGraphicsItem>
#include <QMenu>
#include "commons.h"
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include "builderitem.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include "moduleitem.h"
#include "applicationitem.h"
#include "yarpbuilderlib_global.h"
#include <yarp/manager/manager.h>
#include "safe_manager.h"

#include <QToolBar>

//namespace BuilderUi {
//class BuilderWindow;
//}

class YARPBUILDERLIBSHARED_EXPORT BuilderWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BuilderWindow(Application *app,Manager *lazyManager,SafeManager *safeManager,QWidget *parent = 0);
    ~BuilderWindow();
    void setModuleRunning(bool, int id);
    void setConnectionConnected(bool, QString from, QString to);

    void init(bool refresh = false);
    void refresh();

    void setSelectedModules(QList<int>selectedIds);

    //BuilderItem* addModule(QString itemName, QStringList inputPorts, QStringList outputPorts , QPointF pos, BuilderItem * parent = 0);
    BuilderItem* addModule(Module *module, int moduleId);
    void setOutputPortAvailable(QString, bool);
    void setInputPortAvailable(QString, bool);

    QToolBar *getToolBar();

private:

    void initModuleTab(ModuleItem *it);
    void findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                             InputData* &input_, OutputData* &output_, QString *inModulePrefix, QString *outModulePrefix);
    PortItem* findModelFromOutput(OutputData* output, QString modulePrefix);
    PortItem* findModelFromInput(InputData* input, QString modulePrefix);

private:
    //BuilderUi::BuilderWindow *ui;
    QToolBar builderToolbar;
    BuilderScene *scene;
    QTabWidget *propertiesTab;
    QTreeWidget *appProperties;
    QTreeWidget *moduleProperties;
    QTreeWidget *moduleDescription;
    MyView *view;
    Manager *lazyManager;
    Manager manager;
    SafeManager *safeManager;
    Application *app;
    int index;
    QList <QGraphicsItem*> itemsList;

signals:
    void refreshApplication();

private slots:
    void onRestoreZoom();
    void onSnap(bool);
    void onModuleSelected(QGraphicsItem *it);
    void onApplicationSelected(QGraphicsItem* it);
    void initApplicationTab(ApplicationItem *application = NULL);
    void onAddedModule(void*, QPointF pos);
    void onAddNewConnection(void *, void *);

};

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    MyView(QGraphicsView *parent = 0) : QGraphicsView(parent){
        setInteractive(true);
        mousepressed = false;
        m_rubberBandActive = false;
        rubberBand = NULL;

//        QGLWidget *viewport = new QGLWidget(QGLFormat(QGL::SampleBuffers));
//        setViewport(viewport);

        setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
        setDragMode(QGraphicsView::RubberBandDrag);
        setOptimizationFlags(QGraphicsView::DontSavePainterState |QGraphicsView::DontAdjustForAntialiasing);
        setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    }

protected:
    void wheelEvent(QWheelEvent *event){
        if(event->modifiers() == Qt::CTRL){
            if(event->delta() > 0){
                scale(1.1,1.1);
            }else{
                scale(0.9,0.9);
            }

            return;
        }
        QGraphicsView::wheelEvent(event);
    }

    void keyPressEvent(QKeyEvent *event){
        if(event->key() == Qt::Key_A && event->modifiers() == Qt::CTRL){
            foreach (QGraphicsItem *it, items()) {
                it->setSelected(true);
            }
        }
        if(event->key() == Qt::Key_Delete){
            deleteSelectedItems();
        }
        if(event->key() == Qt::Key_C && event->modifiers() == Qt::CTRL){
            copySelectedItems();
        }
        if(event->key() == Qt::Key_V && event->modifiers() == Qt::CTRL){
            pasteSelectedItems();
        }
        QGraphicsView::keyPressEvent(event);
    }

    void mousePressEvent(QMouseEvent* event)
    {
        origin = event->pos();
        if(event->button()==Qt::LeftButton){
            if(event->modifiers()==Qt::ControlModifier){
                setDragMode(QGraphicsView::ScrollHandDrag);

                QGraphicsView::mousePressEvent(event);
                return;
            }else{
                if(itemAt(event->pos()) == NULL){
                    setDragMode(QGraphicsView::RubberBandDrag);
                    QGraphicsView::mousePressEvent(event);
                    pressedNullItem();
                    return;
                }else{
                    setDragMode(QGraphicsView::NoDrag);
                    QGraphicsView::mousePressEvent(event);
                    return;
                }
            }
        }

        setDragMode(QGraphicsView::NoDrag);
        QGraphicsView::mousePressEvent(event);

    }

    void contextMenuEvent(QContextMenuEvent *event)
    {
        QGraphicsItem *it = itemAt(event->pos());

        QMenu menu(this);
        QAction *copyAction = menu.addAction("Copy");
        QAction *pasteAction = menu.addAction("Paste");
        QAction *deleteAction = menu.addAction("Delete");

        if(!it){
            copyAction->setEnabled(false);
            deleteAction->setEnabled(false);
        }
        if(!copiedItems.isEmpty()){
            pasteAction->setEnabled(true);
        }else{
            pasteAction->setEnabled(false);
        }


        QAction *act = menu.exec(event->globalPos());
        if(act == deleteAction){
            if(it->type() == QGraphicsItem::UserType + (int)HandleItemType){
                deleteSelectedItems(it);
            }else{
                deleteSelectedItems();
            }
        }
        if(act == copyAction){
            copySelectedItems();
        }
        if(act == pasteAction){
            pasteSelectedItems();
        }

        update();

    }


private:
    void deleteSelectedItems(QGraphicsItem *it = NULL){
        copiedItems.clear();
        QList <QGraphicsItem*>selectedItems;
        bool deleteOnlyHandle = false;
        if(it && it->type() == QGraphicsItem::UserType + (int)HandleItemType){
            deleteOnlyHandle  = true;
        }

        foreach (QGraphicsItem* item, items()) {
            if(item->isSelected()){
                selectedItems.append(item);
            }
        }

        foreach (QGraphicsItem* item, selectedItems) {
            if(item->type() == QGraphicsItem::UserType + (int)HandleItemType){
                selectedItems.removeOne(item);
                delete item;
            }

        }

        if(deleteOnlyHandle){
            return;
        }

        foreach (QGraphicsItem* item, selectedItems) {
            if(item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){
                selectedItems.removeOne(item);
                delete item;
            }

        }
        foreach (QGraphicsItem* item, selectedItems) {
            if(item->type() != QGraphicsItem::UserType + (int)ArrowLabelItemType){
                selectedItems.removeOne(item);
                delete item;
            }

        }
    }

    void copySelectedItems(){
        copiedItems.clear();

        foreach (QGraphicsItem* item, items()) {
            if(item->isSelected()){
                if(item->type() == QGraphicsItem::UserType + (int)SourcePortItemType ||
                        item->type() == QGraphicsItem::UserType + (int)DestinationPortItemType ||
                        item->type() == QGraphicsItem::UserType + (int)ModuleItemType ||
                        item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){

                    copiedItems.append(item);
                }
            }
        }
    }

    void pasteSelectedItems(){

        foreach (QGraphicsItem* item, items()) {
            item->setSelected(false);
        }
        QList <QGraphicsItem*> itemsToRemove;
        foreach (QGraphicsItem* item, copiedItems) {
            if(item->type() == QGraphicsItem::UserType + (int)ConnectionItemType){
                Arrow *arrow = ((Arrow*)item);
                BuilderItem *startItem = arrow->startItem();
                BuilderItem *endItem = arrow->endItem();

                BuilderItem *copiedStartItem = NULL;
                BuilderItem *copiedEndItem = NULL;
                if(copiedItems.indexOf(startItem) >= 0 && copiedItems.indexOf(endItem) >= 0){

                    switch (startItem->type()) {
                    case QGraphicsItem::UserType + (int)SourcePortItemType:
                        copiedStartItem = new SourcePortItem("SourcePort_Copia");
                        break;

                    default:
                        break;
                    }
                    if(copiedStartItem){
                        copiedStartItem->setSelected(true);
                        copiedStartItem->snapToGrid(((BuilderScene*) scene())->snap);
                        scene()->addItem(copiedStartItem);
                        copiedStartItem->setPos(startItem->pos().x() + 20,startItem->pos().y() + 20);
                        connect(copiedStartItem->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),
                                ((BuilderScene*) scene()),SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));

                    }

                    switch (endItem->type()) {
                    case QGraphicsItem::UserType + (int)DestinationPortItemType:
                        //copiedEndItem = new DestinationPortItem("DestinationPort_Copia");
                        break;

                    default:
                        break;
                    }
                    if(copiedEndItem){
                        copiedEndItem->setSelected(true);
                        copiedEndItem->snapToGrid(((BuilderScene*) scene())->snap);
                        scene()->addItem(copiedEndItem);
                        copiedEndItem->setPos(endItem->pos().x() + 20,endItem->pos().y() + 20);
                        connect(copiedEndItem->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),
                                ((BuilderScene*) scene()),SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));

                    }


                    if(copiedStartItem && copiedEndItem){
//                        Arrow *newArrow = new Arrow(copiedStartItem, copiedEndItem,"etichetta");
//                        newArrow->setColor(QColor(Qt::red));
//                        copiedStartItem->addArrow(newArrow);
//                        copiedEndItem->addArrow(newArrow);
//                        scene()->addItem(newArrow);
//                        newArrow->setZValue(-1000.0);
//                        newArrow->updatePosition();
//                        newArrow->setSelected(true);

//                        foreach (LineHandle *handle, arrow->handles()) {
//                            newArrow->addHandle(QPointF(handle->pos().x() + 20, handle->pos().y() + 20));
//                        }


//                        if(itemsToRemove.indexOf(startItem) < 0){
//                            itemsToRemove.append(startItem);
//                        }
//                        if(itemsToRemove.indexOf(endItem) < 0){
//                            itemsToRemove.append(endItem);
//                        }
                    }

                }
                //arrow->setSelected(false);
                copiedItems.removeOne(arrow);
            }
        }

        foreach (QGraphicsItem* item, itemsToRemove) {
            //item->setSelected(false);
            copiedItems.removeOne(item);
        }

        foreach (QGraphicsItem* item, copiedItems) {
            //item->setSelected(false);
            BuilderItem *it = NULL;
            switch (item->type()) {
            case QGraphicsItem::UserType + (int)SourcePortItemType:
                it = new SourcePortItem("SourcePort_Copia");
                break;
            case QGraphicsItem::UserType + (int)DestinationPortItemType:
                //it = new DestinationPortItem("DestinationPort_Copia");
                break;
            case QGraphicsItem::UserType + (int)ModuleItemType:
               // it = new ModuleItem("Modulo_Copia",QStringList() << "input1" << "input2" ,QStringList() << "output1");
                break;
            default:
                break;
            }

            if(it){
                it->setSelected(true);
                it->snapToGrid(((BuilderScene*) scene())->snap);
                scene()->addItem(it);
                it->setPos(item->pos().x() + 20,item->pos().y() + 20);
                connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),
                        ((BuilderScene*) scene()),SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));

            }
        }


        hide();
        show();

    }

private:
    QPoint origin;
    QPoint m_lastDragPos;
    QPoint m_rubberBandOrigin;
    bool m_rubberBandActive;
    bool mousepressed;
    QRubberBand *rubberBand;

    QList <QGraphicsItem*>copiedItems;
signals:
    void pressedNullItem();


};


#endif // BUILDERWINDOW_H
