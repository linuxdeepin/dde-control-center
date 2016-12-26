#include "fontlistwidget.h"
#include "settingsgroup.h"
#include "optionitem.h"
#include "../../widget/fontitem.h"
#include "personalization/personalizationmodel.h"
#include "personalization/model/fontmodel.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

FontListWidget::FontListWidget(const QString &title, QWidget *parent)
    :ContentWidget(parent)
{
    m_mainLayout = new QVBoxLayout;
    m_mainGroup  = new SettingsGroup;
    m_widget     = new TranslucentFrame;

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    m_widget->setLayout(m_mainLayout);
    setTitle(title);
    setContent(m_widget);
}

void FontListWidget::setModel(FontModel *const model)
{
    connect(model, &FontModel::listChanged, this, &FontListWidget::setList);
    connect(model, &FontModel::defaultFontChanged, this, &FontListWidget::setDefault);

    setList(model->getFontList());
    setDefault(model->getFontName());
}

void FontListWidget::setList(const QList<QJsonObject> &list)
{
    for (QJsonObject item : list) {
        OptionItem *w = new OptionItem;
        FontItem   *font = new FontItem(item["Id"].toString());
        w->setTitleWidget(font);
        m_mainGroup->appendItem(w);
        m_valueMap.insert(w, item);
        connect(w, &OptionItem::selectedChanged, this, &FontListWidget::onItemClicked);
    }
}

void FontListWidget::setDefault(const QString &name)
{
    QMap<OptionItem *, QJsonObject>::const_iterator i = m_valueMap.constBegin();
    while (i != m_valueMap.constEnd()) {
        OptionItem *w = i.key();
        if (i.value()["Id"].toString() == name) {
            w->setSelected(true);
        } else {
            w->setSelected(false);
        }
        ++i;
    }
}

void FontListWidget::onItemClicked(const bool selected)
{
    if(selected) {
        OptionItem *item = qobject_cast<OptionItem *>(sender());
        Q_ASSERT(m_valueMap.contains(item));

        emit requestSetDefault(m_valueMap[item]);
    }
}

