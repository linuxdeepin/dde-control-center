#include "displaycontrolpage.h"

using namespace dcc::display;

DisplayControlPage::DisplayControlPage(QWidget *parent)
    : QWidget(parent),

      m_displayModel(new DisplayModel),
      m_displayWorker(new DisplayWorker(m_displayModel))
{

}
