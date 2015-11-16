#include "errorview.h"

#include <QDebug>
#include <QLabel>

#include <libdui/dthememanager.h>

#include "constants.h"
#include "../helper.h"

DUI_USE_NAMESPACE

ErrorView::ErrorView(QWidget* p)
    : AbstractView(p),
      m_text(new QLabel)
{
    setObjectName("ErrorView");
    initialize();
}

QWidget* ErrorView::createMainWidget()
{
    m_text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_text->setFixedSize(DCC::ModuleContentWidth, 70);
    m_text->setAlignment(Qt::AlignCenter);
    m_text->setWordWrap(true);
    setStyleSheet(readStyleSheet("errorview"));
    return m_text;
}

ErrorView* ErrorView::setText(const QString& text)
{
    m_text->setText(text);
    return this;
}
