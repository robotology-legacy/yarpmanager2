#include "applicationitem.h"
#include "moduleitem.h"
#include "arrow.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include "builderscene.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>

ApplicationItem::ApplicationItem(Application* application, Manager *lazyManager, Manager *manager,  BuilderItem *parent) :
    BuilderItem(parent)
{
    sigHandler = new ItemSignalHandler();
    this->itemType = ApplicationItemType;
    this->itemName = QString("%1").arg(application->getName());
    this->lazyManager = lazyManager;
    this->application = application;
    //this->itemsList = itemsList;
    this->mainAppManager = manager;
    index = 0;
    minx = miny = maxw = maxh = -1000;

    customFont.setPointSize(font.pointSize() + 5);
    customFont.setBold(true);
    QFontMetrics fontMetric(customFont);
    textWidth = fontMetric.width(itemName);
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
    setFlag(ItemIsMovable,true);
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

    KnowledgeBase* lazy_kb = lazyManager->getKnowledgeBase();

    ModulePContainer mods =  lazy_kb->getModules();
    for(ModulePIterator itr=mods.begin(); itr!=mods.end(); itr++)
        manager.getKnowledgeBase()->addModule((*itr));

    ResourcePContainer res =  lazy_kb->getResources();
    for(ResourcePIterator itr=res.begin(); itr!=res.end(); itr++)
        manager.getKnowledgeBase()->addResource((*itr));

    ApplicaitonPContainer apps =  lazy_kb->getApplications();
    for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
        manager.getKnowledgeBase()->addApplication((*itr));

    bool ret = manager.loadApplication(itemName.toLatin1().data());
    if(ret){

        Application* currApplication = manager.getKnowledgeBase()->getApplication();
        ModulePContainer modules = manager.getKnowledgeBase()->getModules(currApplication);
        CnnContainer connections = manager.getKnowledgeBase()->getConnections(currApplication);
        /*************************************/
        ExecutablePContainer exes = mainAppManager->getExecutables();
        yarp::manager::ExecutablePIterator moditr;
        //CnnContainer connections = mainAppManager->getConnections();

        /*************************************/
        ModulePIterator itr;
        QList <int> usedModulesId;
        for(itr=modules.begin(); itr!=modules.end(); itr++){
            Module* module = (*itr);
            QString moduleId;
            for(moditr=exes.begin(); moditr<exes.end(); moditr++){
                QString id = QString("%1").arg((*moditr)->getID());
                QString command = QString("%1").arg((*moditr)->getCommand());
                QString host = QString("%1").arg((*moditr)->getHost());
                QString param = QString("%1").arg((*moditr)->getParam());
                QString stdio = QString("%1").arg((*moditr)->getStdio());
                QString workDir = QString("%1").arg((*moditr)->getWorkDir());

                if(!strcmp(module->getParam(),param.toLatin1().data()) &&
                   !strcmp(module->getHost(),host.toLatin1().data()) &&
                   !strcmp(module->getStdio(),stdio.toLatin1().data()) &&
                   !strcmp(module->getWorkDir(),workDir.toLatin1().data()) &&
                   !strcmp(module->getName(),command.toLatin1().data())){
                    bool idFound = false;
                    foreach (int usedId, usedModulesId) {
                        if(usedId == id.toInt()){
                            idFound = true;
                        }
                    }
                    if(!idFound){
                        usedModulesId.append(id.toInt());
                        moduleId = id;
                        break;
                    }
                    }


            }
            addModule(module,moduleId.toInt());
        }
        index = (index/900)*100+50;
        CnnIterator citr;
        ModulePContainer allModules = manager.getKnowledgeBase()->getSelModules();
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
            findInputOutputData((*citr), allModules, input, output,&inModulePrefix,&outModulePrefix);
            if(output){
                source = findModelFromOutput(output,outModulePrefix);
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
                dest = findModelFromInput(input,inModulePrefix);
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
                    arrow = new Arrow(source, dest, baseCon,id,this);
                    arrow->setColor(QColor(Qt::red));
                    source->addArrow(arrow);
                    dest->addArrow(arrow);
                    //scene->addItem(arrow);
                    arrow->setZValue(-1000.0);
                    arrow->updatePosition();
                    itemsList.append(arrow);
                }
            }
            id++;
        }

    }







    /***********************************************************/





    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setColor(QColor(80,80,80,80));
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);

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
    itemsList.append(it);
    //scene->addItem(it);
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
                                          InputData* &input_, OutputData* &output_, QString *inModulePrefix,QString *outModulePrefix)
{
  input_ = NULL;
  output_ = NULL;
  string strTo = cnn.to();
  string strFrom = cnn.from();
  qDebug() << "CONNECTION FROM " << strFrom.data() << " TO " << strTo.data();

  ModulePIterator itr;
  for(itr=modules.begin(); itr!=modules.end(); itr++)
  {
      Module* module = (*itr);
      for(int i=0; i<module->inputCount(); i++)
      {
          InputData &input = module->getInputAt(i);
          string strInput = string(module->getPrefix()) + string(input.getPort());
          if(strTo == strInput)
          {
              input_ = &input;
              *inModulePrefix = QString("%1").arg(module->getPrefix());
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
              *outModulePrefix = QString("%1").arg(module->getPrefix());
              break;
          }
      }
  }
}

PortItem* ApplicationItem::findModelFromOutput(OutputData* output,QString modulePrefix)
{
    for(int i=0; i<itemsList.count(); i++)
    {
        QGraphicsItem *it = itemsList.at(i);
        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            ModuleItem *module = (ModuleItem*)it;
            for(int i=0;i<module->oPorts.count();i++){
                PortItem *port = module->oPorts.at(i);
                if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix()))  {
                    return port;
                }
            }

        }
    }


    return NULL;
}

PortItem*  ApplicationItem::findModelFromInput(InputData* input,QString modulePrefix)
{
    for(int i=0; i<itemsList.count(); i++)
    {
        QGraphicsItem *it = itemsList.at(i);
        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            ModuleItem *module = (ModuleItem*)it;
            for(int i=0;i<module->iPorts.count();i++){
                PortItem *port = module->iPorts.at(i);
                if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix())){
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
    setZValue(zValue() + 1);
    QGraphicsItem::mousePressEvent(event);
    sigHandler->applicationSelected(this);
}
void ApplicationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    pressed = false;
    moved = false;
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

        if(snap){
            QPointF newPos = value.toPointF();
            QPointF closestPoint = computeTopLeftGridPoint(newPos);
            return closestPoint+=offset;
        }
    }

    return value;
}


void ApplicationItem::setModuleRunning(int id, bool running)
{
    foreach (QGraphicsItem *it, itemsList) {
        if(it->type() == QGraphicsItem::UserType + ModuleItemType){
            ModuleItem *mod = ((ModuleItem*)it);
            if(mod->getId() == id){
                mod->setRunning(running);
                break;
            }
        }
    }
}

void ApplicationItem::setConnectionConnected(bool connected, QString from, QString to)
{
    foreach (QGraphicsItem *it, itemsList) {
        if(it->type() == QGraphicsItem::UserType + ConnectionItemType){
            Arrow *m= ((Arrow*)it);
            if(m->getFrom() == from.remove(0,1) && m->getTo() == to.remove(0,1)){
                m->setConnected(connected);
                break;
            }
        }
    }
}
