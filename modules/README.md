### 导入模块class的方法：
1. 注意在../frame/main.py中有两个用注释包起来的区域
2. 导入class，然后初始化，设置相应的context
Note: 导入的class是全局的，其实在任何模块都是可用的，为了防止命名冲突，从模块中
导入的class，在qml中使用的全局变量名称统一加上"module_"的前缀
