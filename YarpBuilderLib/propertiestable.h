#ifndef PROPERTIESTABLE_H
#define PROPERTIESTABLE_H

#include <QWidget>
#include <QTreeWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QSignalMapper>
#include <yarp/manager/application.h>
#include <yarp/manager/module.h>
#include <yarp/manager/manager.h>


using namespace std;
using namespace yarp::manager;

#include <QComboBox>
class PropertiesTable : public QWidget
{
    Q_OBJECT
public:
    explicit PropertiesTable(Manager *manager, QWidget *parent = 0);
    void showApplicationTab(Application *application);
    void showModuleTab(Module *it);
    void addModules(Module *);


private:
    Application *currentApplication;
    Module *currentModule;
    QTabWidget *propertiesTab;
    QTreeWidget *appProperties;
    QTreeWidget *moduleProperties;
    QTreeWidget *moduleDescription;

    QTreeWidgetItem *appName;
    QTreeWidgetItem *appDescr;
    QTreeWidgetItem *appVersion;
    QTreeWidgetItem *appAuthors;

    QTreeWidgetItem *modName;
    QTreeWidgetItem *modNode;
    QTreeWidgetItem *modStdio;
    QTreeWidgetItem *modWorkDir;
    QTreeWidgetItem *modPrefix;
    QTreeWidgetItem *modDeployer;
    QTreeWidgetItem *modParams;

    Manager *manager;

//    QTreeWidgetItem *nameItem;
//    QTreeWidgetItem *versionItem;
//    QTreeWidgetItem *descriptionItem;
//    QTreeWidgetItem *parametersItem;
//    QTreeWidgetItem *authorsItem;
//    QTreeWidgetItem *inputsItem;
//    QTreeWidgetItem *outputsItem;

    QSignalMapper *paramsSignalMapper;
    QComboBox *nodeCombo;
    QComboBox *deployerCombo;

    QString lastPrefix;

    QList <Module *> modules;


signals:
    void modified();


private slots:
    void onComboChanged(QWidget *);
    void onAppItemDoubleClicked(QTreeWidgetItem*,int);
    void onAppItemChanged(QTreeWidgetItem*,int);
    void onModItemDoubleClicked(QTreeWidgetItem*,int);
    void onModItemChanged(QTreeWidgetItem*,int);
};

#endif // PROPERTIESTABLE_H
