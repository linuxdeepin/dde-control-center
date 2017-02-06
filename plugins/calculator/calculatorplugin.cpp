#include "calculatorplugin.h"

CalculatorPlugin::CalculatorPlugin(QObject *parent)
    : QObject(parent),

      m_exampleLbl(nullptr)
{

}

void CalculatorPlugin::initialize(FrameProxyInterface *proxy)
{
    m_proxyInter = proxy;

    m_exampleLbl = new QLabel;
    m_exampleLbl->setText("calculator");
    m_exampleLbl->setStyleSheet("color:while;"
                                "background-color:red;");
}

QWidget *CalculatorPlugin::centralWidget()
{
    return m_exampleLbl;
}
