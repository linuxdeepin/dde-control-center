#include "monitorcontrolwidget.h"

#include <QVBoxLayout>

MonitorControlWidget::MonitorControlWidget(QWidget *parent)
    : QWidget(parent),

      m_recognize(new QPushButton),
      m_split(new QPushButton),
      m_join(new QPushButton)
{
    m_recognize->setText(tr("Recognize"));
    m_split->setText(tr("Split"));
    m_join->setText(tr("Merge"));

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_recognize);
    btnsLayout->addWidget(m_split);
    btnsLayout->addWidget(m_join);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(btnsLayout);

    setLayout(mainLayout);
}
