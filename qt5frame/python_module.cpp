
#include "python_module.h"
#include <QDebug>

PythonModule::PythonModule(QObject *parent)
    :QObject(parent)
{
    qDebug() << this->initPythonApi();
}


PythonModule::~PythonModule()
{

}


int PythonModule::initPythonApi()
{
    // 初始化Python
    // 在使用Python系统前，必须使用Py_Initialize对其
    // 进行初始化。它会载入Python的内建模块并添加系统路
    // 径到模块搜索路径中。这个函数没有返回值，检查系统
    // 是否初始化成功需要使用Py_IsInitialized。

    // 初始化python
    Py_Initialize();

    // 检测是否初始化成功
    if(!Py_IsInitialized())
    {
        return -1;
    }

    // 添加当前路径
    // 把输入的字符串作为Python代码直接运行，返回0
    // 表示成功，-1表示有错。大多时候错误都是因为字符串
    // 中有语法错误。

    QString scripts_path = APPSHAREDIR"/scripts";
    QString append_path_str = "sys.path.append('" + scripts_path + "')";
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(append_path_str.toUtf8().constData());
    PyObject *pName, *pModule, *pDict, *pFunc, *pArgs;

    PyObject *pClass, *pObj;

    // 载入脚本
    pName = PyString_FromString("ip_utils");
    pModule = PyImport_Import(pName);
    if(!pModule)
    {
        printf("can't find test.py");
        return -1;
    }

    return 0;
}
