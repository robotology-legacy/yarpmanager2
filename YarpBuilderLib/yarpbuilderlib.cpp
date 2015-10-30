#include "yarpbuilderlib.h"


YarpBuilderLib::YarpBuilderLib()
{
}

BuilderWindow *YarpBuilderLib::getBuilder(Application *app, Manager *lazyManager, SafeManager *manager)
{
    return new BuilderWindow(app,lazyManager,manager);
}
