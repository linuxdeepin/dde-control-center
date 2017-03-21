#include "comboboxwidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "optionitem.h"
#include "lineeditwidget.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::widgets;

ComboBoxWidget::ComboBoxWidget(QFrame *parent)
    : NextPageWidget(parent),

      m_custom(new LineEditWidget),
      m_optionsGroup(new SettingsGroup),
      m_contentPage(nullptr),
      m_lastSelectedItem(nullptr)
{
    connect(this, &NextPageWidget::clicked, this, &ComboBoxWidget::onNextPageClicked);
    connect(m_custom->textEdit(), &QLineEdit::editingFinished, [=] {
        const QString txt = m_custom->textEdit()->text();
        NextPageWidget::setValue(txt);
        emit dataChanged(txt);
    });
}

void ComboBoxWidget::appendOption(const QString &name, const QVariant &value)
{
    OptionItem *item = new OptionItem;
    item->setContentsMargins(20, 0, 10, 0);
    item->setTitle(name);

    connect(item, &OptionItem::selectedChanged, this, &ComboBoxWidget::onItemClicked);

    m_optionsGroup->appendItem(item);
    m_options[item] = value;
}

void ComboBoxWidget::setCurrent(const QVariant &value)
{
    OptionItem *item = m_options.key(value);
    if (!item)
        return NextPageWidget::setValue(QString());

    item->blockSignals(true);
    item->setSelected(true);
    item->blockSignals(false);
    NextPageWidget::setValue(item->title());

    m_lastSelectedItem = item;
}

void ComboBoxWidget::setEditable(const bool editable)
{
    if (editable)
    {
        m_custom->setTitleVisible(false);
        m_optionsGroup->appendItem(m_custom);
    }
}

void ComboBoxWidget::onNextPageClicked()
{
    Q_ASSERT(m_contentPage.isNull());

    m_contentPage = new ContentWidget;

    TranslucentFrame *mainWidget = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(m_optionsGroup);

    m_contentPage->setContent(mainWidget);
    m_optionsGroup->setVisible(true);

    connect(m_contentPage, &ContentWidget::destroyed, this, &ComboBoxWidget::onContentDesktory);

    emit requestPage(m_contentPage.data());
}

void ComboBoxWidget::onContentDesktory()
{
    m_optionsGroup->setVisible(false);
    m_optionsGroup->setParent(this);
}

void ComboBoxWidget::onItemClicked()
{
    OptionItem *item = static_cast<OptionItem *>(sender());
    Q_ASSERT(item);

    if (item == m_lastSelectedItem)
        return;

    NextPageWidget::setValue(item->title());

    if (m_options.contains(item))
        emit dataChanged(m_options.value(item));
    emit m_contentPage->back();

    if (m_lastSelectedItem)
    {
        m_lastSelectedItem->blockSignals(true);
        m_lastSelectedItem->setSelected(false);
        m_lastSelectedItem->blockSignals(false);
    }

    m_lastSelectedItem = item;
}
