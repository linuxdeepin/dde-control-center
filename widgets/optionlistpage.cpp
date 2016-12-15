#include "optionlistpage.h"

using namespace dcc::widgets;

OptionListPage::OptionListPage(const QStringList &options, QWidget *parent)
    : ContentWidget(parent),

      m_options(options)
{

}
