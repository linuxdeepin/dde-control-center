# DConfig 配置参考

控制中心利用 DTK 的 **DConfig** 配置框架管理各模块的运行时行为。所有配置项以 JSON 格式定义在 [`misc/configs/`](../misc/configs/) 目录下，构建时通过 `dconfig2cpp` 工具自动生成类型安全的 C++ 访问类。

> 说明：以下 `appid` 统一为 `org.deepin.dde.control-center`（区域格式除外），`resource` 为 JSON 文件名（不含 `.json` 后缀）。
> 如需查阅更多未列出的项，可以用 `dde-dconfig --list -a <appid>` 自查。

## 配置项索引

### 全局配置（resource: `org.deepin.dde.control-center`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `hideModule` | `string[]` | `[]` | 隐藏模块。被列出的模块在 UI 中不可见（仍在后台加载） | `misc/configs/org.deepin.dde.control-center.json` |
| `disableModule` | `string[]` | `[]` | 禁用模块。被列出的模块完全跳过加载 | 同上 |
| `width` | `double` | `800` | 主窗口默认宽度 | 同上 |
| `height` | `double` | `600` | 主窗口默认高度 | 同上 |
| `sidebarWidth` | `double` | `-1` | 侧边栏宽度（-1 自动适应） | 同上 |

### 账户模块（resource: `org.deepin.dde.control-center.accounts`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `avatarPath` | `string` | `""` | 用户头像默认路径 | `misc/configs/org.deepin.dde.control-center.accounts.json` |

### 日期时间模块（resource: `org.deepin.dde.control-center.datetime`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `customNtpServer` | `string` | `""` | 自定义 NTP 服务器地址 | `misc/configs/org.deepin.dde.control-center.datetime.json` |

### 显示模块（resource: `org.deepin.dde.control-center.display`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `minBrightnessValue` | `double` | `0.1` | 亮度调节最小值 | `misc/configs/org.deepin.dde.control-center.display.json` |

### 电源模块（resource: `org.deepin.dde.control-center.power`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `linePowerScreenBlackDelay` | `string[]` | `["1m","5m","10m","15m","30m","1h"]` | 插入电源—关闭显示器的时间选项 | `misc/configs/org.deepin.dde.control-center.power.json` |
| `linePowerSleepDelay` | `string[]` | `["10m","15m","30m","1h","2h","3h"]` | 插入电源—进入待机的时间选项 | 同上 |
| `linePowerLockDelay` | `string[]` | `["1m","5m","10m","15m","30m","1h"]` | 插入电源—自动锁屏的时间选项 | 同上 |
| `batteryScreenBlackDelay` | `string[]` | `["1m","5m","10m","15m","30m","1h"]` | 使用电池—关闭显示器的时间选项 | 同上 |
| `batterySleepDelay` | `string[]` | `["10m","15m","30m","1h","2h","3h"]` | 使用电池—进入待机的时间选项 | 同上 |
| `batteryLockDelay` | `string[]` | `["1m","5m","10m","15m","30m","1h"]` | 使用电池—自动锁屏的时间选项 | 同上 |
| `showHibernate` | `bool` | `true` | 下拉框是否显示休眠选项 | 同上 |
| `showShutdown` | `bool` | `true` | 下拉框是否显示关机选项 | 同上 |
| `showSuspend` | `bool` | `true` | 下拉框是否显示待机选项 | 同上 |
| `enableScheduledShutdown` | `string` | `"Enabled"` | 定时关机显示状态：`Enabled` 正常 / `Disabled` 置灰 / `Hidden` 隐藏 | 同上 |

### 声音模块（resource: `org.deepin.dde.control-center.sound`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `showDeviceManager` | `bool` | `true` | 是否显示设备管理（用于启用端口自动切换） | `misc/configs/org.deepin.dde.control-center.sound.json` |

### 个性化模块（resource: `org.deepin.dde.control-center.personalization`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `hideIconThemes` | `string[]` | `["Papirus","Papirus-Dark",...]` | 需要隐藏的图标主题列表 ⚠️ 只读 | `misc/configs/org.deepin.dde.control-center.personalization.json` |
| `titleBarHeightStatus` | `string` | `"Enabled"` | 标题栏高度状态：Enabled/Disabled | 同上 |
| `titleBarHeightSupportCompactDisplay` | `bool` | `true` | 标题栏高度是否联动紧凑模式 | 同上 |
| `scrollbarPolicyStatus` | `string` | `"Enabled"` | 滚动条显示策略状态 | 同上 |
| `compactDisplayStatus` | `string` | `"Enabled"` | 紧凑模式状态 | 同上 |

### 通用信息模块（resource: `org.deepin.dde.control-center.commoninfo`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `showReadOnlyProtection` | `bool` | `true` | 开发者模式中是否显示只读保护选项 | `misc/configs/org.deepin.dde.control-center.commoninfo.json` |
| `bootWallpaperEnabled` | `bool` | `true` | GRUB 启动菜单背景图片是否可编辑 | 同上 |
| `bootGrubUserNameVisible` | `bool` | `true` | 启动菜单验证弹窗中是否显示用户名 | 同上 |

### 区域格式（appid: `org.deepin.region-format`, resource: `org.deepin.region-format`）

| key | 类型 | 默认值 | 说明 | 来源文件 |
|-----|------|--------|------|---------|
| `country` | `string` | `""` | 国家/地区 | `misc/configs/common/org.deepin.region-format.json` |
| `languageRegion` | `string` | `""` | 语言与区域格式 | 同上 |
| `localeName` | `string` | `""` | 语言名称（如 `zh_CN.UTF-8`） | 同上 |
| `firstDayOfWeek` | `int` | `1` | 每周第一天（1=周一，7=周日） | 同上 |
| `use24HourFormat` | `bool` | `true` | 是否使用 24 小时制 | 同上 |
| `currency` | `string` | `""` | 货币符号 | 同上 |
| `shortDateFormat` | `string` | `""` | 短日期格式 | 同上 |
| `longDateFormat` | `string` | `""` | 长日期格式 | 同上 |
| `shortTimeFormat` | `string` | `""` | 短时间格式 | 同上 |
| `longTimeFormat` | `string` | `""` | 长时间格式 | 同上 |
| `numericFormat` | `string` | `""` | 数字格式（含小数点与千分位规则） | 同上 |
| `paperSize` | `string` | `""` | 纸张大小（如 `A4`） | 同上 |

## 配置生命周期

1. **定义阶段**：开发者编写 JSON schema 文件放入 `misc/configs/`
2. **代码生成**：构建时 `dconfig2cpp` 工具为每个 JSON 生成 `*_control-center.hpp` C++ 类，暴露 Q_PROPERTY 属性
3. **运行时**：DccManager 在初始化时读取 `hideModule`/`disableModule`，DccPluginLoader 根据配置决定插件是否加载或隐藏
4. **动态更新**：配置变化时通过 DConfig 信号触发，控制中心实时响应（无需重启）

## 常用命令示例

```

# 列出控制中心全量 dconfig
dde-dconfig --list -a org.deepin.dde.control-center

# 读取 hideModule
dde-dconfig --get -a org.deepin.dde.control-center \
  -r org.deepin.dde.control-center -k hideModule

# 设置 hideModule 隐藏指定模块
dde-dconfig --set -a org.deepin.dde.control-center \
  -r org.deepin.dde.control-center -k hideModule -v '["display"]'

# 读取电源模块 showHibernate
dde-dconfig --get -a org.deepin.dde.control-center \
  -r org.deepin.dde.control-center.power -k showHibernate

# 读取区域格式配置（独立 appid）
dde-dconfig --get -a org.deepin.region-format \
  -r org.deepin.region-format -k country

```