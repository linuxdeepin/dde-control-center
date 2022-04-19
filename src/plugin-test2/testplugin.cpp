#include "testplugin.h"
#include "interface/moduleobject.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>

QString Plugin::name() const
{
    return QStringLiteral("plugin-test2");
}

ModuleObject* Plugin::module()
{
    //-----------创建根节点----------
    ModuleObject *moduleRoot = new ModuleObject("menu3", tr("菜单3"), tr("我是菜单3"), QIcon::fromTheme("preferences-system"), this);
    moduleRoot->setChildType(ModuleObject::ChildType::Page);

    for (int j = 0; j < 4; j++) {
        LabelModule *labelModule = new LabelModule(QString("main%1menu%2").arg(3).arg(j), QString("具体页面%1的第%2个page的标题").arg(3).arg(j), moduleRoot);
        labelModule->setText(QString("我是具体页面%1的第%2个page").arg(3).arg(j));
        moduleRoot->appendChild(labelModule);
    }

    return moduleRoot;
}

QString Plugin::follow() const
{
    // 注意这里返回的是上级插件plugin->module()->name(), 而不是plugin->name();
    return QStringLiteral("firstmenu");
}

int Plugin::location() const
{
    return 2;
}

QWidget *LabelModule::page()
{
    return new QLabel(text());
}

void LabelModule::setText(const QString &text)
{
    m_text = text;
}

