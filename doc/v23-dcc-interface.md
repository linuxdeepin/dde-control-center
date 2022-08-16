# V23控制中心接口开发文档

## V23控制中心新特性

1. V23控制中心只负责框架设计，具体功能全部由插件实现
2. V23控制中心支持多级插件系统，支持插件插入到任意位置中
3. 更方便、更精确的搜索功能
4. 高度可定制，可定制任意插件是否显示，若插件支持，可定制任意插件内容是否显示

## V23控制中心设计思想

1. V23控制中心以ModuleObject为核心，将插件中所有页面元素（包括菜单、设置项等）均看作ModuleObject。每个ModuleObject拥有若干个子项（子项也为ModuleObject），最终由一个“根ModuleObject”关联所有ModuleObject从而形成树型结构（这种树型结构后面则称之为‘模块树’）。
2. 每个ModuleObject提供一个page，page以子项数据进行页面布局组织成一个QWidget或提供一个QWidget
3. 每个插件必须提供一个ModuleObject作为其根节点，根节点管理该插件中其他所有ModuleObject，最终由框架指定一个“根ModuleObject”作为所有插件的父级。模块树的终结点（末项）需至少指定一个页面，该页面将由框架添加到页面中进行显示，终结点的页面不做任何限制。

## V23控制中心开发接口说明

1. ModuleObject类用于构建每个页面元素，其是插件的核心
1. PluginInterface类用于规范插件信息，每个插件必须提供一个ModuleObject对象。

### ModuleObject基本信息说明

| 名称        | 数据类型      | 说明 |
|-------------|-------------|---|
| name       | QString     | 名称，作为每个模块的唯一标识，**必须设置** |
| displayName | QString     | 显示名称，如菜单的名称，页面的标题等，为空则不显示 |
| description | QString     | 描述，如主菜单的描述信息 |
| contentText | QStringList | 上下文数据，参与搜索，只可用于终结点 |
| icon        | QVariant | 图标，如主菜单的图标，可以为QIcon、DDciIcon、QString(会根据资源路径去找) |
| badge       | int         | 主菜单中的角标, 默认为0不显示，大于0显示 |

基本信息都有对应的set函数，修改时有moduleDataChanged信号

### ModuleObject接口说明

1. ModuleObject虚函数

| 名称          | 说明  |
|---------------|---|
| active()      | 模块激活时被调用，可重写此方法实现后端数据获取 |
| deactive()    | 离开模块时被调用，可用于释放资源 |
| page()        | 终结点的模块必须实现此方法，用于显示页面，并且每次调用需new新的Widget |
| activePage(autoActive) | 激活并返回page，控制激活流程，不建议重写 |

2. ModuleObject公有方法

| 名称                             | 说明 |
|----------------------------------|---|
| trigger() | 触发该ModuleObject，该函数会触发triggered信号，框架收到信号会切换到该ModuleObject页 |
| currentModule() | 当前激活的子项 |
| setCurrentModule(child) | 设置当前激活项，由框架调用。子项变化时会触发currentModuleChanged信号 |
| defultModule() | 默认激活的子项（如第二级激活时，会根据该值展开到第三级、第四级），如果返回为nullptr则不向下展开 |
| isHiden()                        | 是否为隐藏，默认不隐藏                                       |
| setHiden(hiden)                  | 设置为隐藏，对应ModuleObject隐藏应通过该函数设置，不要自行设置QWidget的隐藏 |
| isDisabled()                     | 是否为禁用，默认为启用                                       |
| setDisabled(disabled)            | 设置为禁用，对应ModuleObject禁用应通过该函数设置，不要自行设置QWidget的禁用 |
| findChild(child)                 | 查找子项，广度搜索优先，返回子项相对于当前模块所在的层级，-1为未找到，0为自己，>0为子项层级 |
| hasChildrens()                   | 是否拥有子项 |
| childrens()                      | 子项列表，由ModuleObject组成 |
| getChildrenSize()                | 获取子项列表大小 |
| appendChild(child)               | 添加子项 |
| removeChild(child/index)         | 删除子项 |
| insertChild(before/index, child) | 插入子项 |

3. ModuleObject静态方法

| 名称           | 说明                                         |
| -------------- | -------------------------------------------- |
| IsHiden        | 返回module是否显示，判断了配置项和程序设置项 |
| IsHidenFlag    | 判断标志是否为隐藏标志                       |
| IsDisabled     | 返回module是否可用，判断了配置项和程序设置项 |
| IsDisabledFlag | 判断标志是否为禁用标志                       |

```
小提示：当模块无需实现虚函数时，可不用继承，直接设置其基本信息即可。
```
### PluginInterface接口说明  

1.PluginInterface虚函数列表：

| 名称       | 说明                                                         |
| ---------- | ------------------------------------------------------------ |
| module()   | 模块对象，每个插件必须提供一个根模块，该模块管理插件中所有子项 |
| name()     | 插件名称，插件标识，需具有唯一性                             |
| follow()   | 插件必须知道其需要跟随的父ModuleObject的url ,默认为空则为一级插件 |
| location() | 插件位置索引或前一个ModuleObject的name，相同索引则按加载顺序进行排序，先加载的往后顺延，默认追加到最后 |

一个标准的插件开发流程：

1. 继承PluginInterface，实现其虚函数。
2. 实例化一个根模块，根模块在初始化时不允许有耗时操作，若有耗时操作，应继承ModuleObject然后实现active方法，将耗时操作放入其中。
3. 若根模块的子项是横向菜单列表，则可使用List储存其基础信息，继承或使用HListModule类，然后循环使用appendChild方法将菜单添加到根模块中。
4. 若根模块的子项是纵向菜单列表，则可使用List储存其基础信息，继承或使用VListModule类，然后循环使用appendChild方法将菜单添加到根模块中。
5. 以此类推，具体的某个子项菜单同样再次添加菜单列表，直到菜单列表的子项为PageModule时为止。
6. 准备一个以上的Module继承自ModuleObject，并实现其page()方法，然后添加到PageModule中，注意，page()方法中需返回新的QWidget对象。
7. 当某个菜单为PageModule时，使用其appendChild方法将上方的Module添加到其子项中，此时，控制中心会根据page的大小添加滚动条，并将多个page进行垂直排列进行显示。PageModule持支嵌套，并且其有默认边距，如果嵌套使用，嵌套的PageModule边距建议设置为0（ getContentsMargins(0, 0, 0, 0)）
8. 若某个VListModule或PageModule页面需要附加按钮时，可调其子项ModuleObject的setExtra，该ModuleObject的page提供按钮，这样该ModuleObject将显示在VListModule或PageModule页面的最下方。

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
};
```
```C++
QString Plugin::name() const
{
    return QStringLiteral("plugin1");
}

ModuleObject *Test1Plugin::module()
{
    // 返回模块根节点
    return new Test1ModuleObject();
}

Test1ModuleObject::Test1ModuleObject()
    : HListModule("firstmenu", tr("主菜单"), tr("我是主菜单"), QIcon::fromTheme("preferences-system"))
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
        ModuleObject *module = new PageModule(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);

        for (int j = 0; j < 30; j++) {
            ButtonModule *buttonModule = new ButtonModule(QString("main%1menu%2").arg(i).arg(j), QString("自定义布局页面%1的第%2个page的标题").arg(i).arg(j), module);
            buttonModule->setText(QString("我是页面%1的第%2个按钮").arg(i).arg(j));
            module->appendChild(buttonModule);
        }
        module->children(1)->setHiden(true);
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

QString Plugin::location() const
{
    // 返回对应位置或前一个兄弟节点的name
    return "2";
}

```
5.自定义布局

要实现类似PageModule的自定义布局，需继承ModuleObject类实现其page函数

```c++
#include "interface/moduleobject.h"

class QFormLayout;
class QScrollArea;
class FormModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit FormModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);

    QWidget *page() override;

private Q_SLOTS:
    void onCurrentModuleChanged(ModuleObject *child);

private:
    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void clearData();

private:
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QScrollArea *m_area;
    QFormLayout *m_layout;
};
```

```c++
FormModule::FormModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , m_area(nullptr)
    , m_layout(nullptr)
{
    // 响应子类激活信号
    connect(this, &FormModule::currentModuleChanged, this, &FormModule::onCurrentModuleChanged);
}

QWidget *FormModule::page()
{
    // page函数在parentWidget上布局窗口并返回
    QWidget *parentWidget = new QWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    parentWidget->setLayout(mainLayout);
    m_layout = new QFormLayout();
    // 在parentWidget析构后需要清理缓存数据，可以监听信号处理，或放deactive函数中
    connect(parentWidget, &QObject::destroyed, this, [this]() { clearData(); });

    QWidget *areaWidget = new QWidget();

    m_area = new QScrollArea(parentWidget);
    m_area->setFrameShape(QFrame::NoFrame);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidgetResizable(true);
    areaWidget->setParent(m_area);
    m_area->setWidget(areaWidget);
    mainLayout->addWidget(m_area);

    areaWidget->setLayout(m_layout);

    for (auto &&tmpChild : childrens()) {
        auto page = tmpChild->activePage();
        if (page) {
            m_layout->addRow(tmpChild->displayName(), page);
            m_mapWidget.insert(tmpChild, page);
            page->setDisabled(ModuleObject::IsDisabled(tmpChild));
        }
    }

    auto addModuleSlot = [this](ModuleObject *const tmpChild) {
        onAddChild(tmpChild);
    };
    // 监听子项的添加、删除、状态变更，动态的更新界面
    connect(this, &ModuleObject::insertedChild, areaWidget, addModuleSlot);
    connect(this, &ModuleObject::appendedChild, areaWidget, addModuleSlot);
    connect(this, &ModuleObject::removedChild, areaWidget, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
    connect(this, &ModuleObject::childStateChanged, areaWidget, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
        if (ModuleObject::IsHidenFlag(flag)) { // 显示隐藏同增加删除处理
            if (state)
                onRemoveChild(tmpChild);
            else
                onAddChild(tmpChild);
        } else if (ModuleObject::IsDisabledFlag(flag)) {
            if (m_mapWidget.contains(tmpChild)) {
                m_mapWidget.value(tmpChild)->setDisabled(state);
            }
        }
    });
    // 处理子激活项
    onCurrentModuleChanged(currentModule());
    return parentWidget;
}
// 处理子激活项
void FormModule::onCurrentModuleChanged(dccV23::ModuleObject *child)
{
    // 激活子项处理是删除之前激活项的QWidget,并用当前激活项的activePage替代
    // 该例子中子项都为叶子节点，此处为滚动到对应项
    // 获取窗口坐标需要在窗口显示后，所以此处稍作延时
    QTimer::singleShot(10, m_area, [this, child]() {
        if (m_area && m_mapWidget.contains(child)) {
            QWidget *w = m_mapWidget.value(child);
            if (-1 != m_layout->indexOf(w)) {
                QPoint p = w->mapTo(w->parentWidget(), QPoint());
                m_area->verticalScrollBar()->setSliderPosition(p.y());
            }
        }
    });
}
// 动态的添加子项
void FormModule::onAddChild(dccV23::ModuleObject *const childModule)
{
    if (ModuleObject::IsHiden(childModule) || m_mapWidget.contains(childModule))
        return;

    int index = 0;
    for (auto &&child : childrens()) {
        if (child == childModule)
            break;
        if (!ModuleObject::IsHiden(child))
            index++;
    }
    auto newPage = childModule->activePage();
    if (newPage) {
        m_layout->insertRow(index, childModule->displayName(), newPage);

        newPage->setDisabled(ModuleObject::IsDisabled(childModule));
        m_mapWidget.insert(childModule, newPage);
    }
}
// 动态的删除子项
void FormModule::onRemoveChild(dccV23::ModuleObject *const childModule)
{
    if (m_mapWidget.contains(childModule)) {
        QWidget *w = m_mapWidget.value(childModule);
        int index = m_layout->indexOf(w);
        if (-1 != index) {
            w->deleteLater();
            delete m_layout->takeAt(index);
            m_mapWidget.remove(childModule);
            return;
        }
    }
}
// 清理缓存数据
void FormModule::clearData()
{
    m_layout = nullptr;
    m_area = nullptr;
    m_mapWidget.clear();
}
```

示例效果图：
[示例效果图](images/plugin-demo.gif)