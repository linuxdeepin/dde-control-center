#include "testplugin.h"
#include "interface/moduleobject.h"
#include "testlayout.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QThread>

QString Test1Plugin::name() const
{
    return QStringLiteral("plugin-test1");
}

ModuleObject *Test1Plugin::module()
{
    QThread::sleep(2);
    //-----------创建根节点----------
    ModuleObject *moduleRoot = new ModuleObject("firstmenu", tr("主菜单"), tr("我是主菜单"), QIcon::fromTheme("preferences-system"), this);
    moduleRoot->setChildType(ModuleObject::HList);

    //-----------正常树构建----------
    { // LabelModule页面
        int i = 1;
        ModuleObject *module = new ModuleObject(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);
        module->setChildType(ModuleObject::Page);

        for (int j = 0; j < 30; j++) {
            LabelModule *labelModule = new LabelModule(QString("main%1menu%2").arg(i).arg(j), QString("具体页面%1的第%2个page的标题").arg(i).arg(j), module);
            labelModule->setText(QString("我是具体页面%1的第%2个page").arg(i).arg(j));
            module->appendChild(labelModule);
        }

        moduleRoot->appendChild(module);
    }
    { // ButtonModule页面
        int i = 2;
        ModuleObject *module = new ModuleObject(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);
        module->setChildType(ModuleObject::Page);

        for (int j = 0; j < 30; j++) {
            ButtonModule *buttonModule = new ButtonModule(QString("main%1menu%2").arg(i).arg(j), QString("具体页面%1的第%2个page的标题").arg(i).arg(j), module);
            buttonModule->setText(QString("我是具体页面%1的第%2个page").arg(i).arg(j));
            module->appendChild(buttonModule);
        }

        moduleRoot->appendChild(module);
    }
    //-----------自定义布局页面----------
    { // ButtonModule页面，同上面ButtonModule页面，但使用自定义布局，显示效果和上面不同
        int i = 4;
        ModuleObject *module = new ModuleObject(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);
        module->setChildType(TESTLAYOUT_KEY); // TESTLAYOUT_KEY为定义布局的key

        for (int j = 0; j < 30; j++) {
            ButtonModule *buttonModule = new ButtonModule(QString("main%1menu%2").arg(i).arg(j), QString("自定义布局页面%1的第%2个page的标题").arg(i).arg(j), module);
            buttonModule->setText(QString("我是页面%1的第%2个按钮").arg(i).arg(j));
            module->appendChild(buttonModule);
        }
        module->children(1)->setHiden(true);
        module->children(2)->setDisabled(true);

        moduleRoot->appendChild(module);
    }

    //-------特殊按钮及多及嵌套示例-----------
    ButtonModule *module = new ButtonModule(QString("menu%1").arg(5), tr("菜单%1").arg(5), moduleRoot);
    module->setChildType(ModuleObject::VList);
    module->setText("测试按钮");
    moduleRoot->appendChild(module); // 主菜单添加带有附加按钮的菜单

    // 添加VList子项，先添加一个正常子项
    ModuleObject *lstModule1 = new ModuleObject(QString("menuSpeci1"), tr("特殊菜单1"), module);
    lstModule1->setChildType(ModuleObject::Page);
    module->appendChild(lstModule1);

    // 正常子项的Page
    LabelModule *labelModule1 = new LabelModule(QString("pageSpeci1"), QString("特殊页面1"), lstModule1);
    labelModule1->setText("特殊页面内容1");
    lstModule1->appendChild(labelModule1);

    // 添加VList子项，再添加一个带有附加按钮的子项
    ButtonModule *lstModule2 = new ButtonModule("menuSpeci2", "特殊菜单2", module);
    lstModule2->setChildType(ModuleObject::Page);
    lstModule2->setText("Page中的测试按钮");
    module->appendChild(lstModule2);

    LabelModule *module2_1 = new LabelModule(QString("pageSpeci2"), QString("特殊页面2"), lstModule2);
    module2_1->setText("特殊页面内容2");
    lstModule2->appendChild(module2_1);

    ButtonModule *module2_2 = new ButtonModule(QString("pageSpeci2"), QString("特殊页面2"), lstModule2);
    module2_2->setText("Page中的测试按钮");
    module2_2->setExtra(); // 设置为附加按钮，父ChildType为ModuleObject::Page
    lstModule2->appendChild(module2_2);

    ButtonModule *ButtonModule3 = new ButtonModule(QString("pageSpeci3"), QString("特殊页面3"), lstModule2);
    ButtonModule3->setChildType(ModuleObject::Page);
    ButtonModule3->setText("测试按钮");
    ButtonModule3->setExtra(); // 设置为附加按钮，父ChildType为ModuleObject::VList
    module->appendChild(ButtonModule3);
    connect(ButtonModule3, &ButtonModule::onButtonClicked, ButtonModule3, &ModuleObject::triggered);

    ButtonModule *module3_1 = new ButtonModule("testPage", "测试页面", module);
    module3_1->setText("附加按钮测试页面");
    ButtonModule3->appendChild(module3_1);

    ButtonModule *module3_2 = new ButtonModule("buttonClose", "关闭", module);
    module3_2->setText("关闭");
    module3_2->setExtra();
    ButtonModule3->appendChild(module3_2);
    connect(module3_2, &ButtonModule::onButtonClicked, lstModule1, &ModuleObject::triggered);

    return moduleRoot;
}

QList<LayoutFactoryBase *> Test1Plugin::layoutFactory()
{
    return { new LayoutFactory<TESTLAYOUT_KEY, testLayout>() };
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
    QPushButton *button = new QPushButton(text());
    button->setMaximumWidth(200);
    connect(button, &QPushButton::clicked, this, &ButtonModule::onButtonClicked);
    return button;
}

void ButtonModule::setText(const QString &text)
{
    m_text = text;
}
