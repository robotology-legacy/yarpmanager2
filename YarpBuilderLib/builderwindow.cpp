#include "builderwindow.h"
//#include "ui_builderwindow.h"
#include <QHBoxLayout>
#include <QSplitter>

BuilderWindow::BuilderWindow(Application *app, Manager *lazyManager, SafeManager *safeManager,  QWidget *parent) :
    QWidget(parent)/*,
    ui(new Ui::BuilderWindow)*/
{
    //ui->setupUi(this);
    this->lazyManager = lazyManager;
    this->app = app;
    this->safeManager = safeManager;

    scene = new BuilderScene(this);
    scene->setSceneRect(QRectF(0, 0, 2000, 2000));
    scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    connect(scene,SIGNAL(addedModule(void*,QPointF)),this,SLOT(onAddedModule(void*,QPointF)));
    connect(scene,SIGNAL(addNewConnection(void*,void*))
            ,this,SLOT(onAddNewConnection(void*,void*)));

    view = new MyView();
    connect(view,SIGNAL(pressedNullItem()),this,SLOT(initApplicationTab()));


    view->setMouseTracking(true);
    view->setScene(scene);
    view->centerOn(scene->sceneRect().width()/2,scene->sceneRect().height()/2);

    propertiesTab = new QTabWidget(this);

    QHBoxLayout *layout = new QHBoxLayout;
    QSplitter *splitter = new QSplitter(this);
    layout->addWidget(splitter);
    splitter->addWidget(view);
    splitter->addWidget(propertiesTab);

    layout->setMargin(0);
    view->centerOn(0,0);
    setLayout(layout);


    appProperties  = new QTreeWidget();
    moduleProperties  = new QTreeWidget();
    moduleDescription = new QTreeWidget();
    appProperties->setColumnCount(2);
    moduleProperties->setColumnCount(2);
    moduleDescription->setColumnCount(2);
    appProperties->setHeaderLabels(QStringList() << "Property" << "Value");
    moduleProperties->setHeaderLabels(QStringList() << "Property" << "Value");
    moduleDescription->setHeaderLabels(QStringList() << "Item" << "Value");

    initApplicationTab();




}

BuilderWindow::~BuilderWindow()
{
    //delete ui;
}

void BuilderWindow::setModuleRunning(bool running, int id)
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
            a->setModuleRunning(id,running);
        }
    }
}

void BuilderWindow::setConnectionConnected(bool connected, QString from, QString to)
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

void BuilderWindow::init(bool refresh)
{
    index = 0;

//    KnowledgeBase* lazy_kb = lazyManager->getKnowledgeBase();


//        ModulePContainer mods =  lazy_kb->getModules();
//        for(ModulePIterator itr=mods.begin(); itr!=mods.end(); itr++)
//            safeManager->getKnowledgeBase()->addModule((*itr));

//        ResourcePContainer res =  lazy_kb->getResources();
//        for(ResourcePIterator itr=res.begin(); itr!=res.end(); itr++)
//            safeManager->getKnowledgeBase()->addResource((*itr));


//        ApplicaitonPContainer apps =  lazy_kb->getApplications();
//        for(ApplicationPIterator itr=apps.begin(); itr!=apps.end(); itr++)
//            safeManager->getKnowledgeBase()->addApplication((*itr));


    bool ret = true;//safeManager->loadApplication(app->getName());
    if(ret){

        Application* mainApplication = safeManager->getKnowledgeBase()->getApplication();
        ModulePContainer modules = safeManager->getKnowledgeBase()->getModules(mainApplication);
        CnnContainer connections = safeManager->getKnowledgeBase()->getConnections(mainApplication);
        ApplicaitonPContainer applications = safeManager->getKnowledgeBase()->getApplications(mainApplication);
        /*************************************/
        ExecutablePContainer exes = safeManager->getExecutables();
        yarp::manager::ExecutablePIterator moditr;

        //CnnContainer connections = safeManager->getConnections();

        /*************************************/

        ApplicationPIterator appItr;
        for(appItr=applications.begin(); appItr!=applications.end(); appItr++)
        {
            Application* application = (*appItr);
            ApplicationItem *appItem = new ApplicationItem(application,lazyManager,safeManager);
            connect(appItem->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
            connect(appItem->signalHandler(),SIGNAL(applicationSelected(QGraphicsItem*)),this,SLOT(onApplicationSelected(QGraphicsItem*)));
            scene->addItem(appItem);
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
        }

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
        ModulePContainer allModules = safeManager->getKnowledgeBase()->getSelModules();
        int id = 0;
        for(citr=connections.begin(); citr<connections.end(); citr++){
            Connection baseCon = *citr;
//            if(baseCon.owner()->getLabel() != mainApplication->getLabel()){
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
                        qDebug() << "APPLICATION CONN";
                    }
                }
                if(!bExist){
                    sourcePort = new SourcePortItem((*citr).from());
                    connect(sourcePort->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
                    itemsList.append(sourcePort);
                    scene->addItem(sourcePort);
                    source = sourcePort;

                    if(model.points.size() > 1){
                        source->setPos(model.points[1].x + source->boundingRect().width()/2,
                                       model.points[1].y + source->boundingRect().height()/2);
                    }else{
                        source->setPos(10 + source->boundingRect().width()/2, index);
                    }
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
                        qDebug() << "APPLICATION CONN";
                    }
                }
                if(!bExist){
                    destPort = new DestinationPortItem((*citr).to());
                    itemsList.append(destPort);
                    dest = destPort;

                    scene->addItem(destPort);
                    size_t size = model.points.size();
                    if(size > 2){
                        dest->setPos(model.points[size-1].x + dest->boundingRect().width()/2,
                                       model.points[size-1].y + dest->boundingRect().height()/2);
                    }else{
                        dest->setPos(400 + dest->boundingRect().width()/2, index);
                    }
                }


             }

            Arrow *arrow;
            // check for arbitrators
            string strCarrier = baseCon.carrier();
            if((strCarrier.find("recv.priority") != std::string::npos)){
                // TODO
            }else{
                if(source && dest){
                    arrow = new Arrow(source, dest, baseCon,id);
                    arrow->setColor(QColor(Qt::red));
                    source->addArrow(arrow);
                    dest->addArrow(arrow);
                    scene->addItem(arrow);
                    arrow->setZValue(-1000.0);
                    arrow->updatePosition();
                    itemsList.append(arrow);
                }
            }
            id++;
        }
    }

    if(!refresh){
        QRectF rr = scene->itemsBoundingRect();
        view->fitInView(rr,Qt::KeepAspectRatio);
    }


}

void  BuilderWindow::onAddNewConnection(void *startItem ,void *endItem)
{
    Arrow *arrow = new Arrow((BuilderItem*)startItem, (BuilderItem*)endItem, safeManager);
    arrow->setColor(QColor(Qt::red));
    ((BuilderItem*)startItem)->addArrow(arrow);
    ((BuilderItem*)endItem)->addArrow(arrow);
    scene->addItem(arrow);
    arrow->setZValue(-1000.0);
    arrow->updatePosition();
    itemsList.append(arrow);

    Application* mainApplication = safeManager->getKnowledgeBase()->getApplication();
    safeManager->getKnowledgeBase()->removeApplication(mainApplication);
    safeManager->getKnowledgeBase()->addApplication(mainApplication);

    itemsList.clear();
    scene->clear();


    refreshApplication();
    init(true);

}

//BuilderItem * BuilderWindow::addModule(QString itemName, QStringList inputPorts, QStringList outputPorts,QPointF pos, BuilderItem *parent)
//{
//    BuilderItem *it = new ModuleItem(itemName,inputPorts,outputPorts);
//    scene->addItem(it);
//    it->setPos(pos);
//    connect(it->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
//    connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
//    return it;
//}

BuilderItem * BuilderWindow::addModule(Module *module,int moduleId)
{
    ModuleItem *it = new ModuleItem(module,moduleId);
    connect(it->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
    connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
    itemsList.append(it);
    scene->addItem(it);
    if(module->getModelBase().points.size()>0){
        it->setPos(module->getModelBase().points[0].x + it->boundingRect().width()/2,
                module->getModelBase().points[0].y + it->boundingRect().height()/2);
    }else{
        it->setPos(index%900+10 + it->boundingRect().width()/2,
                   (index/900)*100+10 + it->boundingRect().height()/2);
        index += 300;
    }
    return it;
}

void BuilderWindow::onAddedModule(void *mod,QPointF pos)
{

    ModuleInterface imod((const char*)((Module*)mod)->getName());
    GraphicModel modBase;
    GyPoint p;
    p.x = pos.x();
    p.y = pos.y();
    modBase.points.push_back(p);
    imod.setModelBase(modBase);

    Application* mainApplication = safeManager->getKnowledgeBase()->getApplication();
    Module* module = safeManager->getKnowledgeBase()->addIModuleToApplication(mainApplication, imod, true);

    if(module){
        string strPrefix = string("/") + module->getLabel();
        module->setBasePrefix(strPrefix.c_str());
        string strAppPrefix = safeManager->getKnowledgeBase()->getApplication()->getBasePrefix();
        safeManager->getKnowledgeBase()->setModulePrefix(module, (strAppPrefix+module->getBasePrefix()).c_str(), false);


        safeManager->getKnowledgeBase()->removeApplication(mainApplication);
        safeManager->getKnowledgeBase()->addApplication(mainApplication);



//        ModuleItem *it = new ModuleItem(module,-1);
//        connect(it->signalHandler(),SIGNAL(moduleSelected(QGraphicsItem*)),this,SLOT(onModuleSelected(QGraphicsItem*)));
//        connect(it->signalHandler(),SIGNAL(requestNewConnection(QPointF,QGraphicsItem*)),scene,SLOT(onNewConnectionRequested(QPointF,QGraphicsItem*)));
//        itemsList.append(it);
//        scene->addItem(it);
//        it->setPos(pos);

        itemsList.clear();
        scene->clear();


        refreshApplication();
        init(true);
    }


}


void BuilderWindow::onModuleSelected(QGraphicsItem *it)
{
    initModuleTab((ModuleItem*)it);
}

void BuilderWindow::onApplicationSelected(QGraphicsItem* it)
{
    initApplicationTab((ApplicationItem*)it);
}

void BuilderWindow::initApplicationTab(ApplicationItem *application)
{
    Application *auxApp = app;
    if(application){
        auxApp = application->getInnerApplication();
    }
    propertiesTab->clear();
    propertiesTab->addTab(appProperties,"Application Properties");
    appProperties->clear();
    if(appProperties->topLevelItemCount() <=0){
        QTreeWidgetItem *appName = new QTreeWidgetItem(appProperties,QStringList() << "Name" << auxApp->getName());
        QTreeWidgetItem *appDescr = new QTreeWidgetItem(appProperties,QStringList() << "Description" << auxApp->getDescription());
        QTreeWidgetItem *appVersion = new QTreeWidgetItem(appProperties,QStringList() << "Version" << auxApp->getVersion());
        QString authors;
        for(int i=0;i<auxApp->authorCount();i++){
            authors = QString("%1%2;").arg(authors).arg(auxApp->getAuthorAt(i).getName());
        }
        QTreeWidgetItem *appAuthors = new QTreeWidgetItem(appProperties,QStringList() << "Name" << authors);

        appProperties->addTopLevelItem(appName);
        appProperties->addTopLevelItem(appDescr);
        appProperties->addTopLevelItem(appVersion);
        appProperties->addTopLevelItem(appAuthors);
    }

}

void BuilderWindow::initModuleTab(ModuleItem *it)
{
    propertiesTab->clear();
    propertiesTab->addTab(moduleProperties,"Module Properties");
    propertiesTab->addTab(moduleDescription,"Description");
    moduleProperties->clear();
    moduleDescription->clear();

    QTreeWidgetItem *modName = new QTreeWidgetItem(moduleProperties,QStringList() << "Name" << it->getInnerModule()->getName());
    QTreeWidgetItem *modNode = new QTreeWidgetItem(moduleProperties,QStringList() << "Node" << it->getInnerModule()->getHost());
    QTreeWidgetItem *modStdio = new QTreeWidgetItem(moduleProperties,QStringList() << "Stdio" << it->getInnerModule()->getStdio());
    QTreeWidgetItem *modWorkDir = new QTreeWidgetItem(moduleProperties,QStringList() << "Workdir" << it->getInnerModule()->getWorkDir());
    QTreeWidgetItem *modPrefix = new QTreeWidgetItem(moduleProperties,QStringList() << "Prefix" << it->getInnerModule()->getPrefix());
    QTreeWidgetItem *modDeployer = new QTreeWidgetItem(moduleProperties,QStringList() << "Deployer");
    QTreeWidgetItem *modParams = new QTreeWidgetItem(moduleProperties,QStringList() << "Parameters" << it->getInnerModule()->getParam());

    modNode->setFlags(modNode->flags() | Qt::ItemIsEditable);
    modStdio->setFlags(modStdio->flags() | Qt::ItemIsEditable);

    moduleProperties->addTopLevelItem(modName);
    moduleProperties->addTopLevelItem(modNode);
    moduleProperties->addTopLevelItem(modStdio);
    moduleProperties->addTopLevelItem(modWorkDir);
    moduleProperties->addTopLevelItem(modPrefix);
    moduleProperties->addTopLevelItem(modDeployer);
    moduleProperties->addTopLevelItem(modParams);


    QTreeWidgetItem *nameItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Name" << it->getInnerModule()->getName());
    QTreeWidgetItem *versionItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Version" << it->getInnerModule()->getVersion());
    QTreeWidgetItem *descriptionItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Description" << it->getInnerModule()->getDescription());
    QTreeWidgetItem *parametersItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Parameters");
    for(int i=0;i<it->getInnerModule()->argumentCount();i++){
        Argument a = it->getInnerModule()->getArgumentAt(i);
        QTreeWidgetItem *it = new QTreeWidgetItem(parametersItem,QStringList() << a.getParam() << a.getDescription());
        Q_UNUSED(it);
    }

    QTreeWidgetItem *authorsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Authors" << it->getInnerModule()->getName());
    for(int i=0;i<it->getInnerModule()->authorCount();i++){
        Author a = it->getInnerModule()->getAuthorAt(i);
        QTreeWidgetItem *it = new QTreeWidgetItem(authorsItem,QStringList() << a.getName() << a.getEmail());
        Q_UNUSED(it);
    }

    QTreeWidgetItem *inputsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Inputs" << it->getInnerModule()->getName());
    for(int i=0;i<it->getInnerModule()->inputCount();i++){
        InputData a = it->getInnerModule()->getInputAt(i);

        QTreeWidgetItem *type = new QTreeWidgetItem(inputsItem,QStringList() << "Type" << a.getName());
        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
        QTreeWidgetItem *req = new QTreeWidgetItem(type,QStringList() << "Required" << (a.isRequired() ? "yes" : "no"));
        Q_UNUSED(port);
        Q_UNUSED(desc);
        Q_UNUSED(req);
    }

    QTreeWidgetItem *outputsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Outputs" << it->getInnerModule()->getName());
    for(int i=0;i<it->getInnerModule()->outputCount();i++){
        OutputData a = it->getInnerModule()->getOutputAt(i); //TODO controllare

        QTreeWidgetItem *type = new QTreeWidgetItem(outputsItem,QStringList() << "Type" << a.getName());
        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
        Q_UNUSED(port);
        Q_UNUSED(desc);
    }

    moduleDescription->addTopLevelItem(nameItem);
    moduleDescription->addTopLevelItem(versionItem);
    moduleDescription->addTopLevelItem(descriptionItem);
    moduleDescription->addTopLevelItem(parametersItem);
    moduleDescription->addTopLevelItem(authorsItem);
    moduleDescription->addTopLevelItem(inputsItem);
    moduleDescription->addTopLevelItem(outputsItem);
}

void BuilderWindow::onRestoreZoom()
{
    view->resetMatrix();
}

void BuilderWindow::onSnap(bool checked)
{
    scene->snapToGrid(checked);
}

void BuilderWindow::findInputOutputData(Connection& cnn,  ModulePContainer &modules,
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

PortItem* BuilderWindow::findModelFromOutput(OutputData* output,QString modulePrefix)
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
                        QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getPrefix());
                        if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == prefix)  {
                            return port;
                        }
                    }
                }
            }

        }

        if(it->type() == (QGraphicsItem::UserType + (int)ModuleItemType)){
            ModuleItem *module = (ModuleItem*)it;
            for(int k=0;k<module->oPorts.count();k++){
                PortItem *port = module->oPorts.at(k);
                if(!strcmp(port->outData.getPort(), (*output).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix()))  {
                    return port;
                }
            }

        }
    }


    return NULL;
}

PortItem*  BuilderWindow::findModelFromInput(InputData* input,QString modulePrefix)
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
                        QString prefix = QString("%1%2").arg(application->getInnerApplication()->getPrefix()).arg(module->getInnerModule()->getPrefix());
                        if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == prefix)  {
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
                if(!strcmp(port->inData.getPort(),(*input).getPort()) && modulePrefix == QString("%1").arg(module->getInnerModule()->getPrefix())){
                    return port;
                }
            }

        }
    }


    return NULL;
}
