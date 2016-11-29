#include "resolutiondetailpage.h"

using namespace dcc;

ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : ContentWidget(parent),

      m_resolutions(new SettingsGroup)
{
    setTitle(tr("Resolution"));
    setContent(m_resolutions);
}

void ResolutionDetailPage::setModel(DisplayModel *model)
{
    m_model = model;

    // delete old items
    for (auto item : m_options.values())
    {
        m_resolutions->removeItem(item);
        item->deleteLater();
    }

    const Monitor *mon = model->monitorList().first();
    const auto modes = mon->modeList();

    bool first = true;
    for (auto m : modes)
    {
        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        OptionItem *item = new OptionItem;

        if (first)
        {
            first = false;
            item->setTitle(res + tr(" (Recommended)"));
        } else {
            item->setTitle(res);
        }

        m_resolutions->appendItem(item);
    }
}
