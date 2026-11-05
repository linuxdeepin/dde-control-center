---
name: dcc-modulepaths
description: "在 dde-control-center 中通过 DConfig subpath 接入或排查 DccObject 的显示、禁用策略。用于新增可配置设置项、登记 modulePaths、排查 dconfig/override 不生效，以及检查 QML 策略联动。"
---

# DccObject subpath 配置接入

> **关键规则**：先确认对象的完整路径，再登记 modulePaths，最后配置该路径的 visible/disable。当前实现启动时读取，修改后需要完全重启控制中心。

## 范围与目标

用于设置项的显示、禁用策略。完成接入时，应能说明：界面上控制什么、对象路径是什么、配置从哪里读取、如何验证和恢复。

## 先读

以下路径均相对仓库根目录；按目标插件选择对应文件。

| 文件 | 确认内容 |
| --- | --- |
| `src/plugin-*/qml/` 中的目标组件 | DccObject 的 `name`、`parentName`，业务可见条件和提交回调 |
| `src/plugin-*/misc/configs/<小写插件名>/org.deepin.dde.control-center.modulepaths.json` | 目标完整路径是否已经登记 |
| `misc/configs/org.deepin.dde.control-center.module.json` | `visible`、`disable` 的默认值和权限 |
| `src/dde-control-center/dccmanager.cpp` | `readModulePaths`、`loadModuleConfigValues`、`refreshModuleConfigState`、`updateConfigState` |
| `misc/DdeControlCenterPluginMacros.cmake` | 插件目标、配置元数据的安装规则 |

不要从显示文字或 QML 的物理嵌套推测对象路径；以 `name`、`parentName` 构成的逻辑树为准。

## 两种 subpath

应用 ID 固定为 `org.deepin.dde.control-center`。以声音插件的音效总开关为例：

| 配置 ID | subpath | 配置内容 |
| --- | --- | --- |
| `org.deepin.dde.control-center.modulepaths` | `/sound` | `modulePaths` 数组包含 `root/system/sound/soundEffectsPage/soundEffects` |
| `org.deepin.dde.control-center.module` | `/root/system/sound/soundEffectsPage/soundEffects` | `visible`、`disable` |

清单中的路径统一写成 `root/...`；查询、设置单对象配置时，`-s` 使用 `/root/...`。`visible=false` 表示隐藏，`disable=true` 表示禁用，默认分别为 `true`、`false`。

## 配置优先级

同一用户、应用 ID、配置 ID、subpath 和键下，优先级从高到低为：

**命令行 `dde-dconfig set` 写入的用户值 > override 默认值 > 元数据 JSON 默认值。**

例如 override 设置 `visible=false`，命令行设置 `visible=true` 后，有效值为 `true`。执行 `dde-dconfig reset` 清除用户值后，会回到 override 的 `false`；没有适用的 override 时，才回到元数据默认值。修改 override 不会清除已有用户值，排查时先用 `get` 确认有效值。

这是同一个配置键的取值优先级；界面仍受下文的父节点、业务条件和主配置限制。单路径 override 不会自动登记 modulePaths。

## 接入步骤

1. **确定粒度**：明确控制整页、整组、单行还是一个按钮。已有 DccObject 就复用；没有独立节点才补充，不能把隐藏父页当作完成单项配置。
2. **确定路径**：使用稳定的 `name`，保持大小写。同一逻辑父节点下不能重名；不同父节点下可以同名。`name` 不含 `/`，完整路径不使用 `*` 或 `.`、`..` 路径段；动态项也需要可匹配的稳定路径。
3. **登记清单**：在目标插件 JSON 的 `contents.modulePaths.value` 中追加路径，保留原有字段和路径。当前账户/其他账户、电池/插电等不同路径按需求分别登记。仅登记父节点不会自动读取子项配置，也不要求每个祖先都登记。
4. **检查联动和提交**：保留硬件、权限等业务条件。标题和按钮若是兄弟节点，需要按需求联动；独立弹窗、延迟任务提交前检查关联策略。`visibleToApp`、`enabledToApp` 表示节点自身的有效状态，不包含完整祖先状态；自定义控件还需考虑实际界面的可见、启用状态。
5. **更新运行环境**：仅改 JSON 时更新已安装元数据；改 QML 时还要编译并使用新版插件。`--spec` 只指定插件目录，不会让 DConfig 自动读取源码 JSON。元数据安装位置由 `${CMAKE_INSTALL_PREFIX}/share/dsg/configs/<应用 ID>/<小写插件名>/` 决定，以 `dde-dconfig get` 的结果确认实际来源。

接入 subpath 不要求统一更换事件信号。若属性变化回调会把初始化或后台刷新误当成用户提交，再考虑 `onToggled`、`onActivated`、`onMoved`。替换后检查程序赋值、鼠标/键盘操作及重复选择；需要时比较后台当前值，避免重复提交。

## 生效逻辑

```text
collectModulePathSubpaths → readModulePaths
  根清单 + 本次插件清单 → 校验、合并去重
loadModuleConfigValues
  配置线程读取各路径的 visible/disable，与 QML 加载并行
finishModuleConfigItem → refreshModuleConfigState
  主线程缓存结果，全部读取请求结束后统一应用
addObject → updateConfigState
  后创建的对象继续使用缓存
```

- 根清单是 `misc/configs/org.deepin.dde.control-center.modulepaths.json`。插件没有对应子路径元数据时，由 DConfig 回退查找根元数据。
- 单路径配置实例读取后释放，当前不持续监听这些键，修改清单或值都需要重启。
- 配置限制与主配置 `hideModule` / `disableModule` 合并；业务条件和父节点仍会影响界面。`visible=true`、`disable=false` 不会强制显示或启用所有控件。

## 最小验证

下面使用已登记的音效总开关；实际接入时替换插件名和完整对象路径。每组设置后重启，逐项观察目标和相邻设置。

```bash
# 1. 确认清单，再确认单路径有效值
dde-dconfig get -a org.deepin.dde.control-center -r org.deepin.dde.control-center.modulepaths -s /sound -k modulePaths
dde-dconfig get -a org.deepin.dde.control-center -r org.deepin.dde.control-center.module -s /root/system/sound/soundEffectsPage/soundEffects -k visible
dde-dconfig get -a org.deepin.dde.control-center -r org.deepin.dde.control-center.module -s /root/system/sound/soundEffectsPage/soundEffects -k disable

# 2. 隐藏：目标消失，相邻设置保持预期状态
dde-dconfig set -a org.deepin.dde.control-center -r org.deepin.dde.control-center.module -s /root/system/sound/soundEffectsPage/soundEffects -k visible -v false

# 3. 禁用：先恢复显示，再确认目标不能操作
dde-dconfig set -a org.deepin.dde.control-center -r org.deepin.dde.control-center.module -s /root/system/sound/soundEffectsPage/soundEffects -k visible -v true
dde-dconfig set -a org.deepin.dde.control-center -r org.deepin.dde.control-center.module -s /root/system/sound/soundEffectsPage/soundEffects -k disable -v true

# 4. 恢复：同时清除本节点 visible/disable 的用户设置
dde-dconfig reset -a org.deepin.dde.control-center -r org.deepin.dde.control-center.module -s /root/system/sound/soundEffectsPage/soundEffects
```

涉及交互时，补查恢复后能否提交、模型刷新是否误写回；有弹窗或延迟任务时，检查策略限制下的提交行为。静态路径核对不等同于界面验证，模拟后端测试不等同于真实业务写入验证。

## 按现象排查

| 现象 | 优先检查 |
| --- | --- |
| 源码已登记，`get modulePaths` 仍没有 | 安装前缀、插件名 subpath、旧的清单 override；是否运行了对应插件目录 |
| `get visible/disable` 与预期不符 | 是否查询同一用户、同一完整 subpath；是否有优先于 override 的命令行用户值，必要时 reset 后再查 |
| 配置值正确，界面没有变化 | 是否完全重启；是否加载新版 QML；对象实际路径和配置粒度是否正确 |
| 未登记也隐藏了，或恢复后仍不显示 | 路径是否已被根清单或其他已读取的插件清单登记；父节点、业务条件、`hideModule` 是否限制了它 |
| 只隐藏内容，标题/添加按钮还在 | 它们是否是逻辑兄弟节点；是否遗漏登记或联动 |
| 已禁用，回调仍能提交 | 自定义按钮、独立弹窗、计时器是否检查策略；属性同步回调是否直接写后台 |

日志中优先搜索：`Failed to read module paths config`、`Expected a modulePaths list`、`Invalid module path`、`Failed to init module config`。

override 的单路径文件位置通常为 `/etc/dsg/configs/overrides/org.deepin.dde.control-center/org.deepin.dde.control-center.module/<root/...完整路径>/<文件名>.json`。排查时同时检查此默认值与用户设置，不能把 reset 当成删除 override。

## 输出要求

按插件列出：**界面项 → 完整路径 → 清单文件 → 验证命令及预期 → 实测结果/未验证条件**。区分“已有对象但未登记”“需要补独立对象或联动”“受业务/硬件限制”，不要统一写成“不支持”。
