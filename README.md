deepin-system-settings 
======================

### 统一一下模块id:
notification            系统消息
display                 显示
desktop                 桌面
individuation           个性化
sound                   声音
date_time               日期和时间
power                   电源
shortcuts               快捷键
keyboard                键盘
mouse                   鼠标*
touchpad                触摸板
network                 网络
bluetooth               蓝牙
disk_mount              磁盘挂载
account                 账户设置
default_applications    默认程序
system_info             系统信息
shutdown                关机


### 导入模块class的方法：
1. 注意在frame/main.py中有两个用注释包起来的区域
2. 导入class，然后初始化，设置相应的context

Note: 导入的class是全局的，其实在任何模块都是可用的，为了防止命名冲突，从模块中
导入的class，在qml中使用的全局变量名称统一加上"module_"的前缀

### 添加系统设置新模块内容的方法：

请在frame/DssLaunchPad.qml的头部的列表中按照例子添加相应的入口

三个参数的意义：

列表中显示名称，入口qml，图标名称（这个全称为preferences-*.png
可以在../frame/images/文件夹中添加和查看）
