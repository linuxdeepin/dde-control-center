# 调试技巧与常见问题

## 单插件调试（--spec）

最常用的调试手段，加载构建目录中的单个插件，无需安装：

```bash
dde-control-center --spec ./build/lib/plugins_v1.1/
```

## 不能混用 ASAN

控制中心本身没有使用 AddressSanitizer 编译时，不能加载使用 ASAN 编译的插件（加载会失败）。

## 线程安全

`DccFactory::create()` 在线程池中执行，完成后 data 对象移动到主线程。构造函数中创建的所有 QObject 必须纳入 QObject 树（以 data 对象为 parent 或其子孙），否则不会随主线程移动。

```cpp
// 错误
MyModule::MyModule(QObject *parent) : QObject(parent) {
    m_worker = new Worker();  // 无 parent，停留在线程池线程
}

// 正确
MyModule::MyModule(QObject *parent) : QObject(parent) {
    m_worker = new Worker(this);  // 纳入 QObject 树
}
```

## 常见问题

### 插件不显示

1. 检查 hideModule：
   ```bash
   dde-dconfig get org.deepin.dde.control-center -r org.deepin.dde.control-center hideModule
   ```
2. 检查 `{Name}.qml` 中 `visible` 是否为 `false`
3. 检查 DBus 条件是否满足
4. 检查插件目录结构：`ls plugins_v1.1/myplugin/`

### dccData 为 undefined

1. 确认是否在 `{Name}.qml` 中访问（C++ 插件此时未加载）
2. 确认 `DCC_FACTORY_CLASS(YourClass)` 正确调用
3. 确认 `#include "yourclass.moc"` 是 `.moc` 不是 `.h`
4. 确认 CMake 链接了 `Dde::Control-Center`

### QML 修改不生效

1. QML 文件名必须首字母大写
2. 不能用 `main.qml`（已废弃），必须 `{Name}Main.qml`
3. 清除 QML 缓存：`rm -rf ~/.cache/dde-control-center/qmlcache/`
4. 确认重新编译了 QML 模块：`cmake --build . --target myplugin_qml`

### 搜索不到设置项

1. 确认 `canSearch` 为 `true`（默认值）
2. 确认 `displayName`/`description` 包含关键字
3. DccSettingsView 中的设置项默认参与搜索

### 图标不显示

1. 图标文件必须在 `qml/` 目录下
2. 图标格式为 `.dci`
3. `icon` 属性值与文件名匹配（不含扩展名）

### DBus 属性不更新

1. 检查 `connection` 类型（`SessionBus` vs `SystemBus`）
2. 验证 DBus 服务存在：
   ```bash
   busctl --user introspect org.deepin.dde.Appearance1 /org/deepin/dde/Appearance1
   ```
3. 属性名与 QML 保留字冲突时使用 `suffix`

## 诊断命令

```bash
# 查看 QML 运行时错误
dde-control-center --spec /path/to/plugins 2>&1 | grep -i "error\|warning"

# 验证 .so 插件包含工厂接口
strings /path/to/myplugin.so | grep DccFactory

# 验证安装路径
ls /usr/lib/x86_64-linux-gnu/dde-control-center/plugins_v1.1/myplugin/

# 重启 DConfig daemon
systemctl --user restart dde-dconfig-daemon.service
```

## 完整检查清单

```
[ ] CMake PLUGIN_NAME 与 {Name}.qml 中 name 一致
[ ] 插件目录名与插件名一致: src/plugin-{name}/
[ ] {Name}.qml 存在且 name 正确
[ ] {Name}Main.qml 存在（非 main.qml）
[ ] .so 文件编译成功且包含 DCC_FACTORY_CLASS
[ ] qmldir 文件存在
[ ] 不在 DConfig hideModule 列表中
[ ] {Name}.qml 中 visible 不为 false
[ ] 所有 QML 文件名首字母大写
[ ] 所有 QObject 子对象有 parent
```
