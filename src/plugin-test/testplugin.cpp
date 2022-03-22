#include "testplugin.h"
#include "interface/moduleobject.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>

QString Plugin::name() const
{
    return QStringLiteral("plugin1");
}

ModuleObject* Plugin::module()
{
    //-----------创建根节点----------
    ModuleObject *moduleRoot = new ModuleObject("firstmenu", tr("主菜单"), tr("我是主菜单"), QIcon::fromTheme("preferences-system"), this);
    moduleRoot->setChildType(ModuleObject::ChildType::HList);

    //-----------正常树构建----------
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

    //-------特殊按钮及多及嵌套示例-----------
    ButtonModule *module = new ButtonModule(QString("menu%1").arg(4), tr("菜单%1").arg(4), moduleRoot);
    module->setChildType(ModuleObject::ChildType::VList);
    module->setText("测试按钮");
    moduleRoot->appendChild(module);// 主菜单添加带有附加按钮的菜单，需实现其extraButton虚函数

    // 添加VList子项，先添加一个正常子项
    ModuleObject *lstModule1 =new ModuleObject(QString("menuSpeci1"), tr("特殊菜单1"), module);
    lstModule1->setChildType(ModuleObject::ChildType::Page);
    module->appendChild(lstModule1);

    // 正常子项的Page
    LabelModule *labelModule1 = new LabelModule(QString("pageSpeci1"), QString("特殊页面1"), lstModule1);
    labelModule1->setText("特殊页面内容1");
    lstModule1->appendChild(labelModule1);

    // 添加VList子项，再添加一个带有附加按钮的子项
    ButtonModule *lstModule2 = new ButtonModule("menuSpeci2", "特殊菜单2", module);
    lstModule2->setChildType(ModuleObject::ChildType::Page);
    lstModule2->setText("Page中的测试按钮");
    module->appendChild(lstModule2);

    LabelModule *labelModule2 = new LabelModule(QString("pageSpeci2"), QString("特殊页面2"), lstModule2);
    labelModule2->setText("特殊页面内容2");
    lstModule2->appendChild(labelModule2);

    connect(module, &ButtonModule::onButtonClicked, module, [module] {
        module->activeChild(0);
    });

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


QWidget *ButtonModule::page()
{
    QWidget *page = new QWidget;
    QVBoxLayout *vlayout = new QVBoxLayout(page);
    page->setLayout(vlayout);
    QLabel *label = new QLabel("附加按钮测试页面");
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setText("关闭");
    vlayout->addWidget(label, 0);
    vlayout->addWidget(closeButton, 1);

    connect(closeButton, &QPushButton::clicked, this, &ButtonModule::onButtonClicked);
    return page;
}

QWidget *ButtonModule::extraButton()
{
    QPushButton *button = new QPushButton(text());
    connect(button, &QPushButton::clicked, this, &ButtonModule::extraButtonClicked);
    return button;
}

void ButtonModule::setText(const QString &text)
{
    m_text = text;
}
