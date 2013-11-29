## deepin-system-settings 
Linux Deepin 2014 的新设置中心

### 统一一下模块id:
* notification            系统消息
* display                 显示
* desktop                 桌面
* individuation           个性化
* sound                   声音
* date_time               日期和时间
* power                   电源
* shortcuts               快捷键
* keyboard                键盘
* mouse                   鼠标*
* touchpad                触摸板
* network                 网络
* bluetooth               蓝牙
* disk_mount              磁盘挂载
* account                 账户设置
* default_applications    默认程序
* system_info             系统信息
* shutdown                关机

模块的形式为 modules/{module_id}/main.qml，需要注册新的py对象的时候，根据需要
讨论增加吧

Debug自己的模块: python ./frame/main.py {module_id}

### 每个模块可引用的全局变量:
* childColor 默认的背景色
* textColor 默认背景下白色的文字颜色
* languageCode  当前启动的界面，可用于中英文不同的排版

### 国际化翻译

* 在qml中使用 dsTr 代替qt默认的 qsTr 方法来包裹字符串
* 编译./tools/dupdate.tar.gz，使用dupdate/bin/lupdate覆盖系统默认的lupdate
* 新项目编辑locale_config.ini，执行./create_po.py，./update_po.py，这里只执行
./update_po.py就可以了
