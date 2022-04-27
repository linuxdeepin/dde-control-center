# V23控制中心接口开发文档

## V23控制中心新特性

1. V23控制中心只负责框架设计，具体功能全部由插件实现
2. V23控制中心支持多级插件系统，支持插件插入到任意位置中
3. 更方便、更精确的搜索功能
4. 高度可定制，可定制任意插件是否显示，若插件支持，可定制任意插件内容是否显示

## V23控制中心设计思想

1. V23控制中心以ModuleObject为核心，将插件中所有页面元素（包括菜单、设置项等）均看作ModuleObject。每个ModuleObject拥有若干个子项（子项也为ModuleObject），最终由一个“根ModuleObject”关联所有ModuleObject从而形成树型结构（这种树型结构后面则称之为‘模块树’）。
2. 每个ModuleObject拥有一个ChildType，ChildType是由框架预设的枚举，ChildType用于指定其childrens（子项）以何种方式进行布局。
3. 每个ModuleObject拥有一个ModuleData储存其基本信息，主窗体通过其ChildType及ModuleData进行页面布局，当页面布局不正确时，可能是ChildType或ModuleData未设置正确。
4. 每个插件必须提供一个ModuleObject作为其根节点，根节点管理该插件中其他所有ModuleObject，最终由框架指定一个“根ModuleObject”作为所有插件的父级。模块树的终结点（末项）需至少指定一个页面，该页面将由框架添加到页面中进行显示，终结点的页面不做任何限制。

## V23控制中心开发接口说明

1. ModuleData类用于设置ModuleObject的基础信息，基础信息可选择性设置，不设置则不使用。
2. ModuleObject类用于构建每个页面元素，其是插件的核心
3. PluginInterface类用于规范插件信息，每个插件必须提供一个ModuleObject对象。

### ModuleData接口说明

| 名称        | 数据类型      | 说明 |
|-------------|-------------|---|
| Name        | QString     | 名称，作为每个模块的唯一标识，必须设置 |
| DisplayName | QString     | 显示名称，如菜单的名称，页面的标题等，为空则不显示 |
| Description | QString     | 描述，如主菜单的描述信息 |
| ContentText | QStringList | 上下文数据，参与搜索，只可用于终结点 |
| Icon        | QIcon       | 图标，如主菜单的图标 |
| Badge       | int         | 主菜单中的角标, 默认为0不显示，大于0显示 |

ModuleData中的字段比较简单，均可以选择性赋值，如不赋值则不使用。

### ModuleObject接口说明

1. ChildType预设类型

| 名称     | 说明 |
|----------|---|
| MainIcon | 主菜单图标模式，插件无需使用 |
| MainList | 主菜单列表模式，插件无需使用 |
| HList    | 子项将以横向菜单列表显示 |
| VList    | 子项将以纵向菜单列表显示 |
| Page     | 子项将显示具体页面 |

2. ModuleObject虚函数

| 名称          | 说明  |
|---------------|---|
| active()      | 模块激活时被调用，可重写此方法实现后端数据获取 |
| deactive()    | 离开模块时被调用，可用于释放资源 |
| page()        | 终结点的模块必须实现此方法，用于显示页面，并且每次调用需new新的Widget |
| extraButton() | 模块的附加按钮, 没有则不显示，当ChildType为VList时，其在垂直列表最下方，当ChildType为Page时，其在所有page()页面的最下方 |

3. ModuleObject公有方法

| 名称                             | 说明 |
|----------------------------------|---|
| name()                           | 模块名称，取ModuleData.Name值 |
| findChild(child)                 | 查找子项，广度搜索优先，返回子项相对于当前模块所在的层级，-1为未找到，0为自己，>0为子项层级 |
| setChildType(type)               | 设置ChildType,值为枚举类型 |
| childType()                      | 获取ChildType值 |
| setModuleData(data)              | 设置ModuleData,此方法将重新设置基础信息数据 |
| moduleData()                     | 获取基础信息值 |
| hasChildrens()                   | 是否拥有子项 |
| childrens()                      | 子项列表，由ModuleObject组成 |
| getChildrenSize()                | 获取子项列表大小 |
| appendChild(child)               | 添加子项 |
| removeChild(child/index)         | 删除子项 |
| insertChild(before/index, child) | 插入子项 |

```
小提示：当模块无需实现虚函数时，可不用继承，直接设置其ModuleData即可。
```
### PluginInterface接口说明

PluginInterface虚函数列表：

| 名称       | 说明 |
|------------|---- |
| module()   | 模块对象，每个插件必须提供一个根模块，该模块管理插件中所有子项 |
| name()     | 插件名称，插件标识，需具有唯一性 |
| follow()   | 插件必须知道其需要跟随的父插件 name ,默认为空则为一级插件 |
| location() | 插件位置索引，相同索引则按加载顺序进行排序，先加载的往后顺延，默认追加到最后 |

一个标准的插件开发流程：

1. 继承PluginInterface，实现其虚函数。
2. 实例化一个根模块，根模块在初始化时不允许有耗时操作，若有耗时操作，应继承ModuleObject然后实现active方法，将耗时操作放入其中。
3. 若根模块的子项是横向菜单列表，则可使用List储存其基础信息，设置根模块的ChildType为HList，然后循环使用appendChild方法将菜单添加到根模块中。
4. 若根模块的子项是纵向菜单列表，则可使用List储存其基础信息，设置根模块的ChildType为VList，然后循环使用appendChild方法将菜单添加到根模块中。
5. 以此类推，具体的某个子项菜单同样再次添加菜单列表，直到菜单列表的ChildType为Page时为止。
6. 准备一个以上的PageModule继承自ModuleObject，并实现其page()方法，注意，page()方法中需返回新的QWidget对象。
7. 当某个菜单的ChildType设置为Page时，使用其appendChild方法将上方的PageModule添加到其子项中，此时，控制中心会根据page的大小添加滚动条，并将多个page进行垂直排列进行显示。
8. 若某个垂直菜单或Page页面需要附加按钮时，需要实现extraButton()，该方法会将附加按钮显示在垂直菜单或Page页面的最下方。

代码示例：

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
```

3. 实现PluginInterface接口
```C++
QString Plugin::name() const
{
    return QStringLiteral("plugin1");
}

ModuleObject* Plugin::module()
{
    //-----------创建根节点----------
    ModuleObject *moduleRoot = new ModuleObject("firstmenu", tr("主菜单"), tr("我是主菜单"), QIcon::fromTheme("preferences-system"), this);

    // 根节点的子项为横向菜单显示
    moduleRoot->setChildType(ModuleObject::ChildType::HList);

    //-----------正常树构建----------
    for (int i = 1; i < 4; i++) {
        ModuleObject *module = new ModuleObject(QString("menu%1").arg(i), tr("菜单%1").arg(i), this);
        module->setChildType(ModuleObject::ChildType::Page);

        for (int j = 0; j < 4; j++) {
            LabelModule *labelModule = new LabelModule(QString("main%1menu%2").arg(i).arg(j), QString("具体页面%1的第%2个page的标题").arg(i).arg(j), module);
            labelModule->setText(QString("我是具体页面%1的第%2个page").arg(i).arg(j));
            // 每个子项菜单添加一个Page
            module->appendChild(labelModule);
        }

        // 根节点添加子项
        moduleRoot->appendChild(module);
    }

    //-------特殊按钮及多级嵌套示例-----------
    ButtonModule *module = new ButtonModule(QString("menu%1").arg(4), tr("菜单%1").arg(4), moduleRoot);
    module->setChildType(ModuleObject::ChildType::VList);
    module->setText("测试按钮");
    moduleRoot->appendChild(module);// 主菜单添加带有附加按钮的菜单，菜单需实现其extraButton虚函数

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
    // 注意这里返回的是上级插件plugin->module()->name(), 而不是plugin->name();
    return QStringLiteral("firstmenu");
}

int Plugin::location() const
{
    return 2;
}

```
示例效果图：
[示例效果图](images/plugin-demo.gif)
