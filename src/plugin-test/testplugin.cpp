#include "testplugin.h"

#include <QLabel>

QString Plugin::name() const
{
    return QStringLiteral("plugin1");
}

ModuleObject* Plugin::module()
{
    ModuleObject *moduleRoot = new ModuleObject("firstmenu1", tr("主菜单1"), tr("我是主菜单1"), QIcon::fromTheme("preferences-system"), this);
    qDebug() << "name:" << moduleRoot->name();
    moduleRoot->setChildType(ModuleObject::ChildType::HList);

    for (int i = 1; i < 4; i++) {
        ModuleObject *module = new ModuleObject(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);
        module->setChildType(ModuleObject::ChildType::Page);

        for (int j = 0; j < 4; j++) {
            LabelModule *labelModule = new LabelModule(QString("main%1menu%2").arg(i).arg(j), QString("具体页面%1的第%2个page的标题").arg(i).arg(j), module);
            labelModule->setText(QString("我是具体页面%1的第%2个page").arg(i).arg(j));
            module->appendChild(labelModule);
        }

        moduleRoot->appendChild(module);
    }

    return moduleRoot;
}

QWidget *LabelModule::page()
{
    return new QLabel(text());
}

void LabelModule::setText(const QString &text)
{
    m_text = text;
}
