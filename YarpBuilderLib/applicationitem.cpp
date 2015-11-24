#include "applicationitem.h"
#include "moduleitem.h"
#include "arrow.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include "builderscene.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>

ApplicationItem::ApplicationItem(Application* application, Manager *manager,  QList <int> *usedIds,bool isInApp,BuilderItem *parent) :
    BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    this->itemType = ApplicationItemType;
    this->itemName = QString("%1").arg(application->getName());
    this->application = application;
    //this->itemsList = itemsList;
    this->mainAppManager = manager;
    index = 0;
    minx = miny = maxw = maxh = -1000;
    this->isInApp = isInApp;

    customFont.setPointSize(font.pointSize() + 5);
    customFont.setBold(true);
    QFontMetrics fontMetric(customFont);
    textWidth = fontMetric.width(itemName);
    usedModulesId = usedIds;
}

ApplicationItem::~ApplicationItem()
{

}

Application* ApplicationItem::getInnerApplication()
{
    return application;
}



void ApplicationItem::init()
{
    setFlag(ItemIsMovable,!isInApp);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,true);

    pressed = false;
    moved = false;

    setToolTip(itemName);


    QFontMetrics fontMetric(font);
    int textWidth = fontMetric.width(itemName);

//    prepareGeometryChange();
//    mainRect = QRect(0,0,400,400);
//    boundingR = QRectF(mainRect);


    /*********************************************************/
    ModulePContainer modules = mainAppManager->getKnowledgeBase()->getModules(application);
    CnnContainer connections = mainAppManager->getKnowledgeBase()->getConnections(application);
    ApplicaitonPContainer applications = mainAppManager->getKnowledgeBase()->getApplications(application);
    ExecutablePContainer exes = mainAppManager->getExecutables();
    yarp::manager::ExecutablePIterator moditr;


    if(1){


        /*************************************/


        //CnnContainer connections = mainAppManager->getConnections();

        /*************************************/

        ApplicationPIterator appItr;
        for(appItr=applications.begin(); appItr!=applications.end(); appItr++)
        {
            Application* application = (*appItr);
            ApplicationItem *appItem = new ApplicationItem(application,mainAppManager,usedModulesId,true,this);
            //connect(appItem->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
            QObject::connect(appItem->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),sigHandler,SLOT(onConnectionSelected(QGraphicsItem*)));
            //connect(appItem->signalHandler(),SIGNAL(applicationSelected(QGraphicsItem*)),sigHandler,SLOT(onApplicationSelected(QGraphicsItem*)));
            appItem->setZValue(zValue());
            appItem->init();

            if(application->getModelBase().points.size()>0){
                appItem->setPos(application->getModelBase().points[0].x,
                                application->getModelBase().points[0].y);
            } else {
                appItem->setPos(index%900+10,
                                (index/900)*100+10);
                index += 300;
            }
            itemsList.append(appItem);
            updateSizes(appItem);

        }

        ModulePIterator itr;
        //int n = application->sucCount();
        //for(int i=0;i<n;i++){
        for(itr=modules.begin(); itr!=modules.end(); itr++){
            Module* module1 = (*itr); //dynamic_cast<Module*>(application->getLinkAt(i).to());

            for(moditr=exes.begin(); moditr<exes.end(); moditr++){

                Module* module = (*moditr)->getModule();

                if(module == module1){
                    QString id = QString("%1").arg((*moditr)->getID());
                    usedModulesId->append(id.toInt());
                    addModule(module,id.toInt());
                    break;
                }
            }
        }



        index = (index/900)*100+50;
        CnnIterator citr;
        ModulePContainer allModules = mainAppManager->getKnowledgeBase()->getSelModules();
        int id = 0;
        for(citr=connections.begin(); citr<connections.end(); citr++){
            Connection baseCon = *citr;
//            if(baseCon.owner()->getLabel() != application->getLabel()){
//                continue;
//            }
            GraphicModel model = baseCon.getModelBase();
            InputData* input = NULL;
            OutputData* output = NULL;
            BuilderItem *source = NULL;
            BuilderItem *dest = NULL;
            QString inModulePrefix,outModulePrefix;
            findInputOutputData((*citr), allModules, input, output);
            if(output){
                source = findModelFromOutput(output);
            }else{
                bool bExist = false;
                SourcePortItem *sourcePort = NULL;
                for(int i=0;i<itemsList.count() && !bExist;i++){
                    if(itemsList.at(i)->type() == (QGraphicsItem::UserType + SourcePortItemType)){
                        SourcePortItem *auxSourceport = (SourcePortItem*)itemsList.at(i);
                        if(auxSourceport->getItemName() == baseCon.from()){
                            source = auxSourceport;
                            bExist = true;
                            break;
                        }

                    }
                    if(itemsList.at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
                        // TODO
                    }
                }
                if(!bExist){
                    sourcePort = new SourcePortItem((*citr).from(),true,this);
                    itemsList.append(sourcePort);
                    //scene->addItem(sourcePort);
                    source = sourcePort;

                    if(model.points.size() > 1){
                        source->setPos(model.points[1].x + source->boundingRect().width()/2,
                                       model.points[1].y + source->boundingRect().height()/2);
                    }else{
                        source->setPos(10 + source->boundingRect().width()/2, index);
                    }
                    updateSizes(source);
                }


                index += 40;
            }
            if(input){
                dest = findModelFromInput(input);
            }else{
                bool bExist = false;
                DestinationPortItem *destPort = NULL;
                for(int i=0;i<itemsList.count() && !bExist;i++){
                    if(itemsList.at(i)->type() == (QGraphicsItem::UserType + DestinationPortItemType)){
                        DestinationPortItem *auxDestPort = (DestinationPortItem*)itemsList.at(i);
                        if(auxDestPort->getItemName() == baseCon.to()){
                            dest = auxDestPort;
                            bExist = true;
                            break;
                        }
                    }
                    if(itemsList.at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
                        // TODO
                    }
                }
                if(!bExist){
                    destPort = new DestinationPortItem((*citr).to(),true,this);
                    itemsList.append(destPort);
                    dest = destPort;

                    //scene->addItem(destPort);
                    size_t size = model.points.size();
                    if(size > 2){
                        dest->setPos(model.points[size-1].x + dest->boundingRect().width()/2,
                                       model.points[size-1].y + dest->boundingRect().height()/2);
                    }else{
                        dest->setPos(400 + dest->boundingRect().width()/2, index);
                    }
                    updateSizes(dest);
                }


             }

            Arrow *arrow;
            // check for arbitrators
            string strCarrier = baseCon.carrier();
            if((strCarrier.find("recv.priority") != std::string::npos)){
                // TODO
            }else{
                if(source && dest){

                    arrow = new Arrow(source, dest, baseCon,id,true,this);
                    QObject::connect(arrow->signalHandler(),SIGNAL(connectctionSelected(QGraphicsItem*)),sigHandler,SLOT(onConnectionSelected(QGraphicsItem*)));
                    arrow->setColor(QColor(Qt::red));
                    source->addArrow(arrow);
                    dest->addArrow(arrow);
                    //scene->addItem(arrow);
                    arrow->setZValue(zValue()+1);
                    arrow->updatePosition();
                    itemsList.append(arrow);
                }
            }
            id++;
        }

    }







    /***********************************************************/




    if(!isInApp){
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setColor(QColor(80,80,80,80));
        effect->setBlurRadius(5);
        setGraphicsEffect(effect);
    }

    allowInputs = true;
    allowOutputs = true;
    updateBoundingRect();


}


QRectF ApplicationItem::boundingRect() const
{
    QRectF br = QRectF(boundingR.x() - BORDERWIDTH, boundingR.y() - BORDERWIDTH -30 , boundingR.width() + (2*BORDERWIDTH), boundingR.height() + (2*BORDERWIDTH) + 30);
    //qDebug() << br;
    return br;
}


int ApplicationItem::type() const
{
    return (int)(QGraphicsItem::UserType + (int)itemType);
}

QPointF ApplicationItem::connectionPoint()
{
    QPointF startIngPoint;

    return startIngPoint;
}

BuilderItem * ApplicationItem::addModule(Module *module,int moduleId)
{
    ModuleItem *it = new ModuleItem(module,moduleId,true,this);
    QObject::connect(it->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),sigHandler,SLOT(onModuleSelected(QGraphicsItem*)));
    QObject::connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),((BuilderScene*)scene()),SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
    QObject::connect(it->signalHandler(),SIGNAL(addNewConnection(QPointF,QGraphicsItem*)),((BuilderScene*)scene()),SLOT(onNewConnectionAdded(QPointF,QGraphicsItem*)));
    itemsList.append(it);
    //scene->addItem(it);
    it->setZValue(zValue()+2);
    if(module->getModelBase().points.size()>0){
        it->setPos(module->getModelBase().points[0].x + it->boundingRect().width()/2,
                module->getModelBase().points[0].y + it->boundingRect().height()/2);
    }else{
        it->setPos(index%900+10 + it->boundingRect().width()/2,
                   (index/900)*100+10 + it->boundingRect().height()/2);
        index += 300;
    }

    updateSizes(it);

    return it;
}

void ApplicationItem::updateSizes(BuilderItem *it)
{
    if(minx == -1000){
        minx = it->pos().x() - it->boundingRect().width()/2;
    }else{
        if(it->pos().x() - it->boundingRect().width()/2 < minx){
            minx = it->pos().x() - it->boundingRect().width()/2;
        }
    }
    if(miny == -1000){
        miny = it->pos().y() - it->boundingRect().height()/2;
    }else{
        if(it->pos().y() - it->boundingRect().height()/2 < miny){
            miny = it->pos().y() - it->boundingRect().height()/2;
        }
    }
    if(maxw == -1000){
        maxw = it->pos().x() + it->boundingRect().width() /2;
    }else{
        if(it->pos().x() + it->boundingRect().width()/2 > maxw){
            maxw = it->pos().x() + it->boundingRect().width()/2;
        }
    }
    if(maxh == -1000){
        maxh = it->pos().y() + it->boundingRect().height()/2;
    }else{
        if(it->pos().y() + it->boundingRect().height()/2 > maxh){
            maxh = it->pos().y() + it->boundingRect().height()/2;
        }
    }

}

void ApplicationItem::updateBoundingRect()
{
    if(minx < 25){
       qreal diff = 25 - minx;
       foreach (QGraphicsItem *it, itemsList) {
           qreal newPos = it->pos().x() + diff;
           it->setPos(newPos,it->pos().y());
       }
       mainRect.setWidth(maxw + 25 + diff);

    }else{
        qreal diff = minx - 25;
        foreach (QGraphicsItem *it, itemsList) {
            qreal newPos = it->pos().x() - diff;
            it->setPos(newPos,it->pos().y());
        }
        mainRect.setWidth(maxw + 25 - diff);
    }

    if(miny < 25){
       qreal diff = 25 - miny;
       foreach (QGraphicsItem *it, itemsList) {
           qreal newPos = it->pos().y() + diff;
           it->setPos(it->pos().x() ,newPos);
       }
       mainRect.setHeight(maxh + 25 + diff);

    }else{
        qreal diff = miny - 25;
        foreach (QGraphicsItem *it, itemsList) {
            qreal newPos = it->pos().y() - diff;
            it->setPos(it->pos().x() ,newPos);
        }
        mainRect.setHeight(maxh + 25 - diff);
    }


    prepareGeometryChange();
    boundingR = QRectF(mainRect);
    update();
}

void ApplicationItem::findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                                          InputData* &input_, OutputData* &output_)
{
    input_ = NULL;
    output_ = NULL;
    string strTo = cnn.to();
    string strFrom = cnn.from();

    ModulePIterator itr;
    for(itr=modules.begin(); itr!=modules.end(); itr++)
    {
       Module* module = (*itr);
       //if(module->owner() == application)
       {
           for(int i=0; i<module->inputCount(); i++)
           {
               InputData &input = module->getInputAt(i);
               string strInput = string(module->getPrefix()) + string(input.getPort());
               if(strTo == strInput)
               {
                   input_ = &input;
                   break;
               }
           }

           for(int i=0; i<module->outputCount(); i++)
           {
               OutputData &output = module->getOutputAt(i);
               string strOutput = string(module->getPrefix()) + string(output.getPort());
               if(strFrom == strOutput)
               {
                   output_ = &output;
                   break;
               }
           }
       }
    }
}

PortItem* ApplicationItem::findModelFromOutput(OutputData* output)
{
    for(int i=0; i<itemsList.count(); i++)
    {
        QGraphicsItem *it = itemsList.at(i);
        if(it->type() == (QGraphicsItem::UserType + (int)ApplicationItemType)){
            ApplicationItem *application = (ApplicationItem*)it;
            for(int j=0; j<application->getModulesList()->count(); j++){
                if(application->getModulesList()->at(j)->type() == QGraphicsItem::UserType + ModuleItemType){
                    ModuleItem *module = (ModuleItem*)application->getModulesList()->at(j);
                    for(int k=0;k<module->oPorts.count();k++){
                        PortItem *port = module->oPorts.at(k);
                        //QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getBasePrefix());

                        //if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == prefix)  {
                        if(port->outData == output)
                            return port;
                        }
                    }
                }
            }



        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            ModuleItem *module = (ModuleItem*)it;
            for(int k=0;k<module->oPorts.count();k++){
                PortItem *port = module->oPorts.at(k);
                //if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix()))  {
                if(port->outData == output){
                    return port;
                }
            }

        }
    }



    return NULL;
}

PortItem*  ApplicationItem::findModelFromInput(InputData* input)
{
    for(int i=0; i<itemsList.count(); i++)
    {
        QGraphicsItem *it = itemsList.at(i);

        if(it->type() == (QGraphicsItem::UserType + (int)ApplicationItemType)){
            ApplicationItem *application = (ApplicationItem*)it;
            for(int j=0;j<application->getModulesList()->count();j++){
                if(application->getModulesList()->at(j)->type() == QGraphicsItem::UserType + ModuleItemType){
                    ModuleItem *module = (ModuleItem*)application->getModulesList()->at(j);
                    for(int k=0;k<module->iPorts.count();k++){
                        PortItem *port = module->iPorts.at(k);
                        //QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getBasePrefix());
                        //if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == prefix)  {
                        if(port->inData == input){
                            return port;
                        }
                    }
                }
            }

        }

        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            ModuleItem *module = (ModuleItem*)it;
            for(int k=0;k<module->iPorts.count();k++){
                PortItem *port = module->iPorts.at(k);
                //if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix())){
                if(port->inData == input){
                    return port;
                }
            }

        }
    }


    return NULL;
}


void ApplicationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //QGraphicsItem::paint(painter,option,widget);
    painter->save();
    QPainterPath path;
    path.moveTo(mainRect.x(),mainRect.y());
    path.lineTo(mainRect.x(),mainRect.y() - 30);
    path.lineTo(mainRect.x() + textWidth + 50,mainRect.y() - 30);
    path.lineTo(mainRect.x() + textWidth + 50,mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(),mainRect.y());
    path.lineTo(mainRect.x() + mainRect.width(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x(),mainRect.y() + mainRect.height());
    path.lineTo(mainRect.x(),mainRect.y());
    //path.addRoundedRect( mainRect, 5.0, 5.0 );



    painter->setFont(customFont);
    painter->setPen(QPen(QBrush(QColor(Qt::black)),1));
    painter->drawText(QRectF(mainRect.x(),mainRect.y() - 30,
                             textWidth + 50,30
                             ),Qt::AlignCenter,itemName);

    if(isSelected()){
        painter->setBrush(QBrush(QColor(0,0,220,80)));
    }else{
        painter->setBrush(QBrush(QColor(156,156,255,80)));
    }
    painter->drawPath(path);
    painter->restore();

}

QList <QGraphicsItem*>* ApplicationItem::getModulesList()
{
    return &itemsList;
}

void ApplicationItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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

void ApplicationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    //setZValue(zValue() + 10);
    QGraphicsItem::mousePressEvent(event);
    sigHandler->applicationSelected(this);
}
void ApplicationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    pressed = false;
    moved = false;
    //setZValue(zValue() - 10);
    QGraphicsItem::mouseReleaseEvent(event);
}


QVariant ApplicationItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (QGraphicsItem *it, itemsList) {
            if(it->type() == QGraphicsItem::UserType + ModuleItemType){
                ModuleItem *mIt = ((ModuleItem*)it);
                foreach (PortItem *p, mIt->oPorts) {
                    p->updateConnections();
                }
                foreach (PortItem *p, mIt->iPorts) {
                    p->updateConnections();
                }
            }
            if(it->type() == QGraphicsItem::UserType + SourcePortItemType){
                SourcePortItem *sIt = ((SourcePortItem *)it);
                sIt->updateConnections();
            }
            if(it->type() == QGraphicsItem::UserType + DestinationPortItemType){
                DestinationPortItem *dIt = ((DestinationPortItem *)it);
                dIt->updateConnections();
            }

        }

        if(snap && !isInApp){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }

    return value;
}


void ApplicationItem::setModuleRunning(bool running, int id)
{
    for(int i=0;i<itemsList.count();i++){
        if(itemsList.at(i)->type() == (QGraphicsItem::UserType + ModuleItemType)){
            ModuleItem *m = (ModuleItem *)itemsList.at(i);
            if(m->getId() == id){
                m->setRunning(running);
                break;
            }
        }
        if(itemsList.at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
            ApplicationItem *a = (ApplicationItem *)itemsList.at(i);
            a->setModuleRunning(running,id);
        }
    }
}

void ApplicationItem::setSelectedConnections(QList<int>selectedIds)
{
    foreach (QGraphicsItem *it, itemsList) {
        if(it->type() == QGraphicsItem::UserType + ConnectionItemType){
            Arrow *arrow = (Arrow*)it;
            if(selectedIds.contains(arrow->getId())){
                arrow->setConnectionSelected(true);
            }else{
                arrow->setConnectionSelected(false);
            }
        }else
            if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
                ApplicationItem *app = (ApplicationItem*)it;
                app->setSelectedConnections(selectedIds);
            }
    }
}

void ApplicationItem::setSelectedModules(QList<int>selectedIds)
{
    foreach (QGraphicsItem *it, itemsList) {
        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            ModuleItem *mod = (ModuleItem*)it;
            if(selectedIds.contains(mod->getId())){
                mod->setModuleSelected(true);
            }else{
                mod->setModuleSelected(false);
            }
        }else
            if(it->type() == QGraphicsItem::UserType + ApplicationItemType){
                ApplicationItem *app = (ApplicationItem*)it;
                app->setSelectedModules(selectedIds);
            }
    }
}


void ApplicationItem::setConnectionConnected(bool connected, QString from, QString to)
{
    for(int i=0;i<itemsList.count();i++){
        if(itemsList.at(i)->type() == (QGraphicsItem::UserType + ConnectionItemType)){
            Arrow *m = (Arrow *)itemsList.at(i);
            if(m->getFrom() == from && m->getTo() == to){
                m->setConnected(connected);
                break;
            }
        }
        if(itemsList.at(i)->type() == (QGraphicsItem::UserType + ApplicationItemType)){
            ApplicationItem *a = (ApplicationItem *)itemsList.at(i);
            a->setConnectionConnected(connected,from,to);
        }
    }
}

void ApplicationItem::setOutputPortAvailable(QString oData, bool available)
{
    foreach (QGraphicsItem *it, itemsList) {
        BuilderItem *item = (BuilderItem *)it;
        if(item->type() == QGraphicsItem::UserType + ModuleItemType){
            ModuleItem *mod = (ModuleItem*)item;

            foreach (PortItem *oPort, mod->oPorts) {
                QString strPort = QString("%1%2").arg(mod->getInnerModule()->getPrefix()).arg(oPort->outData->getPort());

                if(strPort == oData ){
                    oPort->setAvailable(available);
                }
            }


        }else
        if(item->type() == QGraphicsItem::UserType + ApplicationItemType){
            ApplicationItem *app = (ApplicationItem*)item;
            app->setOutputPortAvailable(oData, available);
        }else
            if(item->type() == QGraphicsItem::UserType + SourcePortItemType){
                SourcePortItem *source = (SourcePortItem*)item;
                QString strPort = QString("%1").arg(source->getItemName());
                if(strPort == oData){
                    source->setAvailable(available);
                }
            }
    }
}

void ApplicationItem::setInputPortAvailable(QString iData, bool available)
{


    foreach (QGraphicsItem *it, itemsList) {
        BuilderItem *item = (BuilderItem *)it;
        if(item->type() == QGraphicsItem::UserType + ModuleItemType){
            ModuleItem *mod = (ModuleItem*)item;

            foreach (PortItem *iPort, mod->iPorts) {
                QString strPort = QString("%1%2").arg(mod->getInnerModule()->getPrefix()).arg(iPort->inData->getPort());
                if(strPort == iData){
                    iPort->setAvailable(available);
                }
            }


        }else
            if(item->type() == QGraphicsItem::UserType + ApplicationItemType){
                ApplicationItem *app = (ApplicationItem*)item;
                app->setInputPortAvailable(iData,available);
            }
            else{
                if(item->type() == QGraphicsItem::UserType + DestinationPortItemType){
                    DestinationPortItem *dest = (DestinationPortItem*)item;

                    QString strPort = QString("%1").arg(dest->getItemName());
                    if(strPort == iData){
                        dest->setAvailable(available);
                    }
                }
            }
    }
}

