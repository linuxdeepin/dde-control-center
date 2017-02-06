
#include "plugininterface.h"
#include "exampleplugin.h"

ExamplePlugin::ExamplePlugin(QObject *parent)
    : QObject(parent),

      m_exampleLbl(nullptr)
{

}

void ExamplePlugin::initialize(FrameProxyInterface *proxy)
{
    m_proxyInter = proxy;

    m_exampleLbl = new QLabel;
    m_exampleLbl->setText("Example");
    m_exampleLbl->setStyleSheet("color:while;"
                                "background-color:cyan;");
}

QWidget *ExamplePlugin::centralWidget()
{
    return m_exampleLbl;
}
