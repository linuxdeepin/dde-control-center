//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "formmodule.h"
#include "testplugin.h"
#include "interface/moduleobject.h"
#include "interface/pagemodule.h"
#include "widgets/listviewmodule.h"
#include "widgets/moduleobjectitem.h"
#include "widgets/horizontalmodule.h"
#include "widgets/settingsgroupmodule.h"
#include "widgets/itemmodule.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QThread>
#include <QLineEdit>
#include <QApplication>
#include <DStyle>
#include <QComboBox>
using namespace DCC_NAMESPACE;
QString Test1Plugin::name() const
{
    return QStringLiteral("plugin-test1");
}

ModuleObject *Test1Plugin::module()
{
    QThread::sleep(2);
    // 返回模块根节点
    return new Test1ModuleObject();
}

Test1ModuleObject::Test1ModuleObject()
    : HListModule("firstmenu", tr("主菜单很长很长很长很长很长很长很长很长很长........................"), tr("我是主菜单"), QIcon::fromTheme("preferences-system"))
{
    // 根节点继承于HListModule
    //-----------正常树构建----------
    { // LabelModule页面
        int i = 1;
        ModuleObject *module = new PageModule(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);

        for (int j = 0; j < 5; j++) {
            LabelModule *labelModule = new LabelModule(QString("main%1menu%2").arg(i).arg(j), QString("具体页面%1的第%2个page的标题").arg(i).arg(j), module);
            labelModule->setText(QString("我是具体页面%1的第%2个page").arg(i).arg(j));
            module->appendChild(labelModule);
        }

        appendChild(module);
        addTestModule(module);
    }
    { // ButtonModule页面
        int i = 2;
        ModuleObject *module = new PageModule(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);

        for (int j = 0; j < 30; j++) {
            ButtonModule *buttonModule = new ButtonModule(QString("main%1menu%2").arg(i).arg(j), QString("具体页面%1的第%2个page的标题").arg(i).arg(j), module);
            buttonModule->setText(QString("我是具体页面%1的第%2个page").arg(i).arg(j));
            module->appendChild(buttonModule);
        }

        appendChild(module);
    }
    //-----------自定义布局页面----------
    { // ButtonModule页面，同上面ButtonModule页面，但使用自定义布局，显示效果和上面不同
        int i = 4;
        ModuleObject *module = new FormModule(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);

        for (int j = 0; j < 30; j++) {
            ButtonModule *buttonModule = new ButtonModule(QString("main%1menu%2").arg(i).arg(j), QString("自定义布局页面%1的第%2个page的标题").arg(i).arg(j), module);
            buttonModule->setText(QString("我是页面%1的第%2个按钮").arg(i).arg(j));
            module->appendChild(buttonModule);
        }
        module->children(1)->setHidden(true);
        module->children(2)->setDisabled(true);

        appendChild(module);
    }

    //-------特殊按钮及多及嵌套示例-----------
    VListModule *module = new VListModule(QString("menu%1").arg(5), tr("菜单%1").arg(5));
    appendChild(module); // 主菜单添加带有附加按钮的菜单

    // 添加VList子项，先添加一个正常子项
    ModuleObject *lstModule1 = new PageModule(QString("menuSpeci1"), tr("特殊菜单1"), module);
    module->appendChild(lstModule1);

    // 正常子项的Page
    LabelModule *labelModule1 = new LabelModule(QString("pageSpeci1"), QString("特殊页面1"), lstModule1);
    labelModule1->setText("特殊页面内容1");
    lstModule1->appendChild(labelModule1);

    // 添加VList子项，再添加一个带有附加按钮的子项
    PageModule *lstModule2 = new PageModule("menuSpeci2", "特殊菜单2", module);
    module->appendChild(lstModule2);

    LabelModule *module2_1 = new LabelModule(QString("pageSpeci2"), QString("特殊页面2"), lstModule2);
    module2_1->setText("特殊页面内容2");
    lstModule2->appendChild(module2_1);

    ButtonModule *module2_2 = new ButtonModule(QString("pageSpeci2"), QString("特殊页面2"), lstModule2);
    module2_2->setText("Page中的测试按钮");
    module2_2->setExtra(); // 设置为附加按钮，父ChildType为ModuleObject::Page
    lstModule2->appendChild(module2_2);

    ButtonModule *ButtonModule3 = new ButtonModule(QString("pageSpeci3"), QString("特殊页面3"), lstModule2);
    ButtonModule3->setText("测试按钮");
    ButtonModule3->setExtra(); // 设置为附加按钮，父ChildType为ModuleObject::VList
    module->appendChild(ButtonModule3);
    connect(ButtonModule3, &ButtonModule::onButtonClicked, ButtonModule3, &ModuleObject::triggered);
    PageModule *page3 = new PageModule(QString("pageSpeci3"), QString("特殊页面3"), ButtonModule3);
    ButtonModule3->appendChild(page3); // extra项激活时，会激活其第一个子项的page

    ButtonModule *module3_1 = new ButtonModule("testPage", "测试页面", module);
    module3_1->setText("附加按钮测试页面");
    page3->appendChild(module3_1);

    ButtonModule *module3_2 = new ButtonModule("buttonClose", "关闭", module);
    module3_2->setText("关闭");
    module3_2->setExtra();
    page3->appendChild(module3_2);
    connect(module3_2, &ButtonModule::onButtonClicked, lstModule1, &ModuleObject::triggered);
}

QWidget *Test1ModuleObject::initButton()
{
    return new QPushButton();
}
void Test1ModuleObject::addTestModule(ModuleObject *parent)
{
    // ItemModule测试
    // ItemModule提供一个回调函数接口,方便窗口与ModuleObject结合
    // 左边是displayName字符串，右则是回调函数返回的widget，回调函数可以是匿名函数，也可以是成员函数。其参数是ModuleObject*，返回值为创建的QWidget*
    // 同时ItemModule提供一些接口如setLeftVisible设置左则是否显示，setClickable是否处理点击。具体参考ItemModule头文件
    // @warning 回调函数返回的widget生命周期只是对应窗口显示的时候，即模块切换时就会被析构。ItemModule的生命周期是从控制中心启动到关闭。
    ItemModule *item = new ItemModule("item", tr("Title"));
    item->setRightWidget([](ModuleObject *item) {
        Q_UNUSED(item)
        return new QPushButton(Dtk::Widget::DStyle::standardIcon(qApp->style(), Dtk::Widget::DStyle::SP_EditElement), "");
    });
    parent->appendChild(item);
    ItemModule *itemButton = new ItemModule("itemButton", tr("Button:"), false);
    itemButton->setRightWidget(this, &Test1ModuleObject::initButton);
    itemButton->setBackground(true);
    parent->appendChild(itemButton);
    // SettingsGroupModule测试
    // SettingsGroupModule提供一个基于SettingsGroup的ModuleObject,可实现SettingsGroup的窗口背景处理
    SettingsGroupModule *groupModule = new SettingsGroupModule("group", tr("group Module"));
    groupModule->appendChild(new ItemModule("groupItem1", tr("group PushButton"), [](ModuleObject *module) {
        Q_UNUSED(module)
        return new QPushButton(); 
    }));
    groupModule->appendChild(new ItemModule("groupItem2", tr("group LineEdit"), [](ModuleObject *module) {
        Q_UNUSED(module)
        return new QLineEdit(); 
    }));
    groupModule->appendChild(new ItemModule("groupItem3", tr("group ComboBox"), [](ModuleObject *module) {
        Q_UNUSED(module)
        return new QComboBox();
    }));
    parent->appendChild(groupModule);

    // HorizontalModule测试
    // HorizontalModule提供一个横向布局的ModuleObject，与PageModule(纵向布局)类似
    HorizontalModule *hor = new HorizontalModule("hor", tr("Horizontal Module"));
    hor->setStretchType(HorizontalModule::AllStretch);
    parent->appendChild(hor);
    ItemModule *hlabel = new ItemModule(
            "hlabel", tr("Horizontal Edit"), [](ModuleObject *module) {
                QLabel *label = new QLabel(module->displayName());
                connect(module, &ModuleObject::displayNameChanged, label, &QLabel::setText);
                return label;
            },
            false);
    connect(hlabel, &ModuleObject::displayNameChanged, hlabel, [hlabel]() {
        hlabel->setHidden(false);
    });

    ItemModule *hedit = new ItemModule(
            "hedit", tr("Horizontal Edit"), [hlabel](ModuleObject *module) {
                QLineEdit *edit = new QLineEdit(module->displayName());
                edit->setFixedHeight(32);
                connect(module, &ModuleObject::displayNameChanged, edit, &QLineEdit::setText);
                connect(edit, &QLineEdit::editingFinished, [edit, hlabel, module]() {
                    QString text = edit->text();
                    if (!text.isEmpty()) {
                        hlabel->setDisplayName(text);
                        module->setDisplayName(text);
                    }
                    module->setHidden(true);
                });
                return edit;
            },
            false);
    hedit->setHidden(true);
    ItemModule *hbutton = new ItemModule(
            "hbutton", tr("Horizontal QPushButton"), [hlabel, hedit](ModuleObject *module) {
                QPushButton *but = new QPushButton();
                but->setIcon(Dtk::Widget::DStyle::standardIcon(qApp->style(), Dtk::Widget::DStyle::SP_EditElement));
                but->setFixedSize(32, 32);
                connect(but, &QPushButton::clicked, module, [hlabel, hedit, module]() {
                    hlabel->setHidden(true);
                    hedit->setHidden(false);
                    module->setHidden(true);
                });
                return but;
            },
            false);
    connect(hedit, &ModuleObject::stateChanged, hedit, [hlabel, hbutton, hedit]() {
        hlabel->setHidden(!hedit->isHidden());
        hbutton->setHidden(!hedit->isHidden());
    });
    hor->appendChild(hlabel);
    hor->appendChild(hedit);
    hor->appendChild(hbutton);
    // ListViewModule测试
    // ListViewModule提供一个以DListView为界面的ModuleObject，子项可用ModuleObjectItem或ModuleObject
    ListViewModule *listmodule = new ListViewModule("listView", tr("List View"));
    connect(listmodule, &ListViewModule::clicked, this, [](ModuleObject *module) {
        qInfo() << __FILE__ << __LINE__ << "clicked:" << module->displayName();
        QString display = module->displayName();
        if (display.contains("click")) {
            display.remove("click");
            module->setDisplayName(display);
        } else {
            module->setDisplayName(display + "click");
        }
    });
    parent->appendChild(listmodule);
    ModuleObjectItem *listitem0 = new ModuleObjectItem("item0", tr("listitem 0"));
    listmodule->appendChild(listitem0);
    ModuleObjectItem *item1 = new ModuleObjectItem("item1", tr("listitem 1"));
    item1->setRightIcon(Dtk::Widget::DStyle::SP_ArrowEnter);
    listmodule->appendChild(item1);

    ModuleObjectItem *item2 = new ModuleObjectItem("item2", tr("listitem 2"));
    item2->setRightText("right Text", -2);
    item2->setRightIcon(Dtk::Widget::DStyle::SP_ArrowEnter);
    listmodule->appendChild(item2);
    connect(item2, &ModuleObjectItem::clicked, this, [item1]() {
        qInfo() << __FILE__ << __LINE__ << "clicked ModuleObjectItem:" << tr("listitem 2");
        item1->setHidden(!item1->isHidden());
        static int pix = Dtk::Widget::DStyle::SP_ForkElement;
        item1->setRightIcon((Dtk::Widget::DStyle::StandardPixmap)(pix));
        pix++;
        if (pix >= (int)(Dtk::Widget::DStyle::SP_Title_SS_ShowNormalButton))
            pix = Dtk::Widget::DStyle::SP_ForkElement;
    });
    ModuleObject *listitem03 = new ModuleObject("listitem03", tr("listitem 3"));
    listmodule->appendChild(listitem03);
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
