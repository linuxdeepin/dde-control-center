# DDE Control Center - AI Coding Agent Instructions

## Project Overview
DDE Control Center (深度控制中心) v6.0+ is a Qt6/QML-based desktop control panel for Deepin Desktop Environment. It uses a **plugin-based architecture** where the core framework provides infrastructure and all features are implemented as plugins.

## Critical Architecture Concepts

### Plugin System (V25 Architecture)
- **Two-phase loading**: Plugins load in stages to optimize startup
  1. `{plugin}.qml` - Metadata/menu entry (fast, synchronous)
  2. `{plugin}.so` - C++ backend (threaded, async)
  3. `{plugin}Main.qml` - Full UI (loaded on-demand)
- **Plugin location**: `${CMAKE_INSTALL_LIBDIR}/dde-control-center/plugins_v1.0/{plugin-name}/`
- **Factory pattern**: Use `DCC_FACTORY_CLASS(YourClass)` macro + `#include "yourclass.moc"` to register plugins
- **Thread safety**: Plugin C++ objects created in worker threads are moved to main thread - all child objects MUST be in the same tree hierarchy or they won't be moved

### Core Framework Classes
- **DccObject**: Tree-node base class for all UI elements. Key properties:
  - `name`: Unique ID (no `/` characters, used in URL paths)
  - `parentName`: Parent's URL path (can be hierarchical like `"aa/bb/cc"`)
  - `weight`: Display order (0-65535, use increments of 10)
  - `pageType`: Determines rendering (Menu, Editor, Item)
- **DccApp**: Global singleton accessible from QML, provides `root`, `addObject()`, `removeObject()`, `showPage()` 
- **DccFactory**: Interface for plugin registration (see [include/dccfactory.h](include/dccfactory.h))

### QML-C++ Integration
- C++ exports accessed via `dccData` in QML (automatically injected)
- Use `Q_PROPERTY`, `Q_INVOKABLE`, or slots for QML exposure
- Example: `dccData.calc(a, b)` calls C++ method from QML

## Build & Development Workflows

### Standard Build
```bash
cmake -B build
cmake --build build
# Debug single plugin without installing:
./build/bin/dde-control-center --spec ./build/lib/plugins_v1.0/{plugin-name}
```

### Build Flags
- `BUILD_TESTING=ON`: Enable unit tests (uses GTest)
- `ENABLE_ASAN=OFF`: Address sanitizer (disabled by default due to plugin compat issues)
- `DISABLE_*` options: Conditionally disable plugins (see [CMakeLists.txt](CMakeLists.txt#L40-L46))

### Plugin Development Pattern
See complete example in [examples/plugin-example](examples/plugin-example) and [docs/v25-dcc-interface.zh_CN.md](docs/v25-dcc-interface.zh_CN.md)

**CMakeLists.txt template**:
```cmake
find_package(DdeControlCenter REQUIRED)
add_library(myplugin MODULE src/myplugin.cpp)
target_link_libraries(myplugin PRIVATE Dde::Control-Center Qt6::Core)
dcc_install_plugin(NAME myplugin TARGET myplugin)
dcc_handle_plugin_translation(NAME myplugin) # auto-handles i18n
```

**Plugin registration** ([src/plugin-*/operation/*.cpp](src/plugin-deepinid/operation/deepinidinterface.cpp#L25-L26)):
```cpp
#include "dccfactory.h"
DCC_FACTORY_CLASS(YourPluginClass)
#include "yourpluginclass.moc"
```

## Project Conventions

### File Naming
- Plugins: `plugin-{name}/` directories under [src/](src/)
- QML entry: `{name}.qml` (metadata only)
- QML main: `{name}Main.qml` (full UI implementation) - **renamed from `main.qml` in v6.0.77 to avoid translation conflicts**
- C++ sources: `operation/` or `src/` subdirectories

### Configuration Management
- **DConfig** for settings: `org.deepin.dde.control-center` ([misc/configs/](misc/configs/))
- Check visibility: `dde-dconfig get org.deepin.dde.control-center -r org.deepin.dde.control-center hideModule`
- **DccDBusInterface** QML type for DBus interaction (see [docs/v25-dcc-interface.zh_CN.md](docs/v25-dcc-interface.zh_CN.md#L168-L186))

### Qt Version
- **Qt6 only** - mixing Qt5 libraries will cause crashes
- Uses `Qt6::`, `Dtk6::` namespaces
- CMake: `set(QT_NS Qt6)` and `set(DTK_NS Dtk6)`

## Testing
- Framework: GTest for C++ unit tests
- Location: [tests/](tests/) and [dde-grand-search/tests/](dde-grand-search/tests/)
- Run: Enable `BUILD_TESTING=ON`, tests built to `build/bin/`

## Common Pitfalls
1. **ASAN incompatibility**: Don't use ASAN in plugins if control center wasn't built with it
2. **Thread safety**: Plugin constructor objects must be children of the exported class to be moved to main thread
3. **Name conflicts**: Plugin `name` property must be unique - used for URLs and configuration
4. **QML translation**: Use `qsTr()` in QML, handled by `dcc_handle_plugin_translation()`

## Key Files for Reference
- [include/dccobject.h](include/dccobject.h) - Core object properties/signals
- [include/dccapp.h](include/dccapp.h) - Global API
- [include/dccfactory.h](include/dccfactory.h) - Plugin interface + registration macro
- [misc/DdeControlCenterPluginMacros.cmake](misc/DdeControlCenterPluginMacros.cmake) - CMake helpers
- [docs/v25-dcc-interface.zh_CN.md](docs/v25-dcc-interface.zh_CN.md) - Complete API reference (Chinese)
