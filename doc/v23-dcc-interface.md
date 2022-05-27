# V23控制中心接口开发文档

## V23控制中心新特性

1. V23控制中心只负责框架设计，具体功能全部由插件实现
2. V23控制中心支持多级插件系统，支持插件插入到任意位置中
3. 更方便、更精确的搜索功能
4. 高度可定制，可定制任意插件是否显示，若插件支持，可定制任意插件内容是否显示

## V23控制中心设计思想

1. V23控制中心以ModuleObject为核心，将插件中所有页面元素（包括菜单、设置项等）均看作ModuleObject。每个ModuleObject拥有若干个子项（子项也为ModuleObject），最终由一个“根ModuleObject”关联所有ModuleObject从而形成树型结构（这种树型结构后面则称之为‘模块树’）。
2. 每个ModuleObject拥有一个ChildType，ChildType是由框架预设和插件扩展，ChildType用于指定其childrens（子项）以何种方式进行布局。
3. 每个ModuleObject拥有一些基本信息，主窗体通过其ChildType及基本信息进行页面布局，当页面布局不正确时，可能是ChildType或基本信息未设置正确。
4. 每个插件必须提供一个ModuleObject作为其根节点，根节点管理该插件中其他所有ModuleObject，最终由框架指定一个“根ModuleObject”作为所有插件的父级。模块树的终结点（末项）需至少指定一个页面，该页面将由框架添加到页面中进行显示，终结点的页面不做任何限制。

## V23控制中心开发接口说明

1. ModuleObject类用于构建每个页面元素，其是插件的核心
1. LayoutBase类是布局的基类，如果框架提供的布局不能满足需求，可通过插件开发自定义布局
3. PluginInterface类用于规范插件信息，每个插件必须提供一个ModuleObject对象。

### ModuleObject基本信息说明

| 名称        | 数据类型      | 说明 |
|-------------|-------------|---|
| name       | QString     | 名称，作为每个模块的唯一标识，**必须设置** |
| displayName | QString     | 显示名称，如菜单的名称，页面的标题等，为空则不显示 |
| description | QString     | 描述，如主菜单的描述信息 |
| contentText | QStringList | 上下文数据，参与搜索，只可用于终结点 |
| icon        | QIcon       | 图标，如主菜单的图标 |
| badge       | int         | 主菜单中的角标, 默认为0不显示，大于0显示 |

基本信息都有对应的set函数，修改时有moduleDataChanged信号

### ModuleObject接口说明

1. ChildType预设类型

| 名称     | 说明 |
|----------|---|
| Main | 主菜单模式，插件无需使用 |
| HList    | 子项将以横向菜单列表显示 |
| VList    | 子项将以纵向菜单列表显示 |
| Page     | 子项将显示具体页面 |

2. ModuleObject虚函数

| 名称          | 说明  |
|---------------|---|
| active()      | 模块激活时被调用，可重写此方法实现后端数据获取 |
| deactive()    | 离开模块时被调用，可用于释放资源 |
| page()        | 终结点的模块必须实现此方法，用于显示页面，并且每次调用需new新的Widget |

3. ModuleObject公有方法

| 名称                             | 说明 |
|----------------------------------|---|
| trigger() | 触发该ModuleObject，该函数会触发triggered信号，框架收到信号会切换到该ModuleObject页 |
| isHiden()                        | 是否为隐藏，默认不隐藏                                       |
| setHiden(hiden)                  | 设置为隐藏，对应ModuleObject隐藏应通过该函数设置，不要自行设置QWidget的隐藏 |
| isDisabled()                     | 是否为禁用，默认为启用                                       |
| setDisabled(disabled)            | 设置为禁用，对应ModuleObject禁用应通过该函数设置，不要自行设置QWidget的禁用 |
| setChildType(type)               | 设置ChildType,值为uint32_t类型 |
| childType()                      | 获取ChildType值 |
| findChild(child)                 | 查找子项，广度搜索优先，返回子项相对于当前模块所在的层级，-1为未找到，0为自己，>0为子项层级 |
| hasChildrens()                   | 是否拥有子项 |
| childrens()                      | 子项列表，由ModuleObject组成 |
| getChildrenSize()                | 获取子项列表大小 |
| appendChild(child)               | 添加子项 |
| removeChild(child/index)         | 删除子项 |
| insertChild(before/index, child) | 插入子项 |

```
小提示：当模块无需实现虚函数时，可不用继承，直接设置其基本信息即可。
```
### LayoutBase接口说明

LayoutBase类是布局的基类，用户可通过插件开发自定义布局

LayoutBase类需实现ModuleObject的子项的布局，处理其添加、删除子项、子项状态变更等信号

1.LayoutBase虚函数列表:

| 名称         | 说明                                                         |
| ------------ | ------------------------------------------------------------ |
| layoutModule | 完成module布局，将ModuleObject的子项在给的QWidget中布局      |
| setCurrent   | 设置当前项，并定位到该项                                     |
| autoExpand   | 自动展开项，children为空时需返回一个子项，若返回为空则表示不再展开，默认为自动展开第0项 |
| checkUpdate  | 检查从oldChild转到newChild是否要更新布局，需要则layoutModule,setCurrent会被调用，否则只调用setCurrent，默认不更新布局 |

2.LayoutBase静态方法:

| 名称           | 说明                                             |
| -------------- | ------------------------------------------------ |
| IsHiden        | 判断ModuleObject是否隐藏，判断包括设置值和配置值 |
| IsHidenFlag    | 是否为隐藏标志，                                 |
| IsDisabled     | 判断ModuleObject是否禁用，判断包括设置值和配置值 |
| IsDisabledFlag | 是否为禁用标志                                   |

自定义布局可以通过插件layoutFactory接口给出

LayoutFactoryBase类是布局工厂的基类，layoutbase.h中提供了LayoutFactory模板类，方便实现布局工厂类

### PluginInterface接口说明  

1.PluginInterface虚函数列表：

| 名称            | 说明                                                         |
| --------------- | ------------------------------------------------------------ |
| module()        | 模块对象，每个插件必须提供一个根模块，该模块管理插件中所有子项 |
| name()          | 插件名称，插件标识，需具有唯一性                             |
| follow()        | 插件必须知道其需要跟随的父ModuleObject的url ,默认为空则为一级插件 |
| location()      | 插件位置索引，相同索引则按加载顺序进行排序，先加载的往后顺延，默认追加到最后 |
| layoutFactory() | 自定义布局工厂，插件可根据需要提供多个自定义布局             |

一个标准的插件开发流程：

1. 继承PluginInterface，实现其虚函数。
2. 实例化一个根模块，根模块在初始化时不允许有耗时操作，若有耗时操作，应继承ModuleObject然后实现active方法，将耗时操作放入其中。
3. 若根模块的子项是横向菜单列表，则可使用List储存其基础信息，设置根模块的ChildType为HList，然后循环使用appendChild方法将菜单添加到根模块中。
4. 若根模块的子项是纵向菜单列表，则可使用List储存其基础信息，设置根模块的ChildType为VList，然后循环使用appendChild方法将菜单添加到根模块中。
5. 以此类推，具体的某个子项菜单同样再次添加菜单列表，直到菜单列表的ChildType为Page时为止。
6. 准备一个以上的PageModule继承自ModuleObject，并实现其page()方法，注意，page()方法中需返回新的QWidget对象。
7. 当某个菜单的ChildType设置为Page时，使用其appendChild方法将上方的PageModule添加到其子项中，此时，控制中心会根据page的大小添加滚动条，并将多个page进行垂直排列进行显示。
8. 若某个垂直菜单或Page页面需要附加按钮时，可调其子项ModuleObject的setExtra，该ModuleObject的page提供按钮，这样该ModuleObject将显示在垂直菜单或Page页面的最下方。

### 代码示例：

1. 准备Page，LabelModule继承自ModuleObject
```C++
QWidget *LabelModule::page()
{
    return new QLabel(text());
}

void LabelModule::setText(const QString &text)
{
    m_text = text;
}
```

2. 准备附加按钮，ButtonModule继承自ModuleObject
```C++
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
```

3. 实现PluginInterface接口

```c++
class Plugin : public PluginInterface
{
    Q_OBJECT
    // IID会用于去重，需唯一
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Plugin_test" FILE "plugin-test.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QList<LayoutFactoryBase *> layoutFactory() override;
};
```
```C++
QString Plugin::name() const
{
    return QStringLiteral("plugin1");
}

ModuleObject* Plugin::module()
{
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
```
4. 二级插件与一级插件不同的是，需要实现其follow方法
```C++
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
    // 注意这里返回的是上级的url
    return QStringLiteral("firstmenu");
}

int Plugin::location() const
{
    return 2;
}

```
5.自定义布局

要实现自定义布局，需继承LayoutBase类

```c++
#include "interface/layoutbase.h"
// 定义布局的key，在使用自定义布局时会用到，注册布局时，如果key已被使用会对应报错，并注册失败
// key通常用预设的DCC_LAYOUT_TYPE或(|)上一个数字，在使用时没找到该布局则会根据key值使用一个接近的预设DCC_LAYOUT_TYPE，如果没接近的则使用ModuleObject::Page
// key或上的数字取值范围(0x00000001 ~ 0x00FFFFFF)，key应唯一，如果注册时已存在会对应报错
#define TESTLAYOUT_KEY (DCC_NAMESPACE::ModuleObject::Page | 1)
class QFormLayout;
class QScrollArea;
class testLayout : public DCC_NAMESPACE::LayoutBase
{
public:
    testLayout();

    virtual void setCurrent(DCC_NAMESPACE::ModuleObject *const child);
    virtual QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, const QList<DCC_NAMESPACE::ModuleObject *> &children);

private:
    void removeChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void addChild(DCC_NAMESPACE::ModuleObject *const childModule);

private:
    QFormLayout *m_layout;
    QList<DCC_NAMESPACE::ModuleObject *> m_modules;
    DCC_NAMESPACE::ModuleObject *m_module;
    QScrollArea *m_area;
};
```

```c++
// 设置并定位到当前项，如果是ListView则setCurrentIndex，如果是滚动区域，则需要滚动到对应项
void testLayout::setCurrent(ModuleObject *const child)
{
    int index = m_modules.indexOf(child);
    if (index == -1)
        return;
    QWidget *w = m_layout->itemAt(index, QFormLayout::FieldRole)->widget();
    QPoint p = w->mapTo(w->parentWidget(), QPoint());
    m_area->verticalScrollBar()->setSliderPosition(p.y());
}

QWidget *testLayout::layoutModule(ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    Q_UNUSED(children)
    // 所保存的变量尽量与parent关联，该类会在界面切换时delete
    m_module = module;
    m_area = new QScrollArea(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    parent->setLayout(mainLayout);
    mainLayout->addWidget(m_area);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    QHBoxLayout *m_hlayout = new QHBoxLayout();
    m_hlayout->setMargin(0);
    m_hlayout->setSpacing(0);
    mainLayout->addLayout(m_hlayout);
    m_area->setFrameShape(QFrame::NoFrame);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidgetResizable(true);

    QWidget *areaWidget = new QWidget(m_area);
    m_area->setWidget(areaWidget);
    m_layout = new QFormLayout;
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    areaWidget->setLayout(m_layout);

    for (auto tmpChild : module->childrens()) {
        // 隐藏、禁用判断使用IsHiden、IsDisabled，该函数包含设置项和配置项结果
        if (LayoutBase::IsHiden(tmpChild))
            continue;
        auto page = tmpChild->page();
        if (page) {
            // 有用到ModuleObject的基本信息时需要监听moduleDataChanged信号去动态修改
            m_layout->addRow(tmpChild->displayName(), page);
            m_modules.append(tmpChild);
            page->setDisabled(LayoutBase::IsDisabled(tmpChild));
        }
        tmpChild->active();
    }
    auto addModuleSlot = [this](ModuleObject *const tmpChild) {
        addChild(tmpChild);
    };

    // 监听子项的添加、删除、状态变更，动态的更新界面
    QObject::connect(module, &ModuleObject::insertedChild, m_area, addModuleSlot);
    QObject::connect(module, &ModuleObject::appendedChild, m_area, addModuleSlot);
    QObject::connect(module, &ModuleObject::removedChild, m_area, [this](ModuleObject *const childModule) { removeChild(childModule); });
    QObject::connect(module, &ModuleObject::childStateChanged, m_area, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
        if (LayoutBase::IsHidenFlag(flag)) {
            if (state)
                removeChild(tmpChild);
            else
                addChild(tmpChild);
        } else if (LayoutBase::IsDisabledFlag(flag)) {
            int index = m_modules.indexOf(tmpChild);
            m_layout->itemAt(index, QFormLayout::FieldRole)->widget()->setDisabled(state);
        }
    });
    QObject::connect(areaWidget, &QWidget::destroyed, module, [module] {
        for (auto tmpChild : module->childrens()) {
            tmpChild->deactive();
        }
        module->deactive();
    });
    // 如果子项需要继续布局则需返回一个QWidget,否则返回空
    return nullptr;
}
// 动态删除子项
void testLayout::removeChild(ModuleObject *const childModule)
{
    int index = m_modules.indexOf(childModule);
    if (index != -1) {
        m_layout->removeRow(index);
        m_modules.removeAt(index);
    }
}
// 动态添加子项
void testLayout::addChild(ModuleObject *const childModule)
{
    if (LayoutBase::IsHiden(childModule))
        return;

    int index = 0;
    for (auto &&child : m_module->childrens()) {
        if (child == childModule)
            break;
        if (!LayoutBase::IsHiden(child))
            index++;
    }
    auto newPage = childModule->page();
    if (newPage) {
        m_layout->insertRow(index, childModule->displayName(), newPage);
        newPage->setDisabled(LayoutBase::IsDisabled(childModule));
        m_modules.insert(index, childModule);
    }
}
```

最后在layoutFactory中把布局工厂返回出去

```
QList<LayoutFactoryBase *> Plugin::layoutFactory()
{
    return { new LayoutFactory<TESTLAYOUT_KEY, testLayout>() };
}
```

示例效果图：
[示例效果图](images/plugin-demo.gif)
