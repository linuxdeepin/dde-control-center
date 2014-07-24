#ifndef PYTHON_MODULE_H
#define PYTHON_MODULE_H

#include <iostream>
#include <string>
#include <Python.h>
#include <QObject>

class PythonModule: public QObject{
    Q_OBJECT

public:
    PythonModule(QObject* parent = 0);
    ~PythonModule();

    int initPythonApi();
};

#endif // PYTHON_MODULE_H
