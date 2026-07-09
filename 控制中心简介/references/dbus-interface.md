# DBus 接口参考

## 服务标识

| 属性 | 值 |
|------|-----|
| 服务名称 | `org.deepin.dde.ControlCenter1` |
| 对象路径 | `/org/deepin/dde/ControlCenter1` |
| 接口名称 | `org.deepin.dde.ControlCenter1` |
| 总线类型 | 会话总线 (Session Bus) |

## 接口方法

| 方法 | 签名 | 说明 |
|------|------|------|
| `Show` | `void Show()` | 显示并激活主窗口 |
| `Hide` | `void Hide()` | 隐藏主窗口 |
| `Toggle` | `void Toggle()` | 切换窗口可见性 |
| `Exit` | `void Exit()` | 终止应用程序 |
| `ShowHome` | `void ShowHome()` | 导航至根页面（回到首页） |
| `ShowPage` | `void ShowPage(QString url)` | 导航至指定 URL 路径。支持 `?indicator=true` 查询参数（指示器模式，不显示窗口） |
| `ShowPage` (已弃用) | `void ShowPage(QString module, QString page)` | 两参数版本。拼接 `module/page` 后委托给单参数版本 |
| `ShowModule` (已弃用) | `void ShowModule(QString module)` | 委托给 `ShowPage(module)` |
| `GetAllModule` | `QString GetAllModule()` | 返回所有已注册模块的 JSON 列表（异步回复） |

## 接口属性

| 属性 | 类型 | 说明 |
|------|------|------|
| `Rect` | `QRect` | 主窗口几何信息（位置与大小） |
| `Page` | `QString` | 当前活动页面的内部对象路径（斜杠分隔的 name，不含 "root"） |
| `Path` | `QString` | 当前活动页面的本地化显示路径（斜杠分隔的 displayName） |

> 属性变化时发射 `org.freedesktop.DBus.PropertiesChanged` 信号。

## 全局搜索接口

接口名称：`org.deepin.dde.ControlCenter1.GrandSearch`

| 方法 | 签名 | 说明 |
|------|------|------|
| `Search` | `QString Search(QString json)` | 执行搜索查询。输入 JSON 格式：`{"cont":"关键词","ver":1,"mID":"test"}`。返回匹配项的 JSON 结果 |
| `Stop` | `bool Stop(QString json)` | 停止正在进行的搜索。始终返回 `true` |
| `Action` | `bool Action(QString json)` | 执行搜索结果动作（导航至该项）。输入 JSON 需包含 `"action":"openitem"` 和 `"item":"URL路径"` |

**自动退出机制**：全局搜索适配器构造时启动 10 秒定时器，每次 `Search`/`Stop`/`Action` 调用重启定时器。超时且主窗口不可见时自动退出。

## 命令行调用示例

```bash
# 显示控制中心
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.Show

# 隐藏控制中心
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.Hide

# 切换显示/隐藏
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.Toggle

# 导航到指定模块页面
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.ShowPage \
  "privacy/camera"

# 回到首页
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.ShowHome

# 全局搜索（搜索蓝牙相关设置）
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.GrandSearch.Search \
  '{"cont":"bluetooth","ver":1,"mID":"test"}'

# 获取所有模块列表
gdbus call --session \
  --dest=org.deepin.dde.ControlCenter1 \
  --object-path /org/deepin/dde/ControlCenter1 \
  --method org.deepin.dde.ControlCenter1.GetAllModule

# 监听属性变化信号
gdbus monitor --session \
  --dest=org.deepin.dde.ControlCenter1
```