#include "addfingerpage.h"
#include "translucentframe.h"
#include <QVBoxLayout>
#include <QDebug>

using namespace dcc;
using namespace dcc::accounts;
using namespace dcc::widgets;

AddFingerPage::AddFingerPage(const QString &thumb, QWidget *parent)
    : ContentWidget(parent)
    , m_buttonTuple(new ButtonTuple)
    , m_fingerWidget(new FingerWidget)
    , m_frequency(1)
    , m_thumb(thumb)
{
    setTitle(tr("Add Fingerprint"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(20);

    layout->addWidget(m_fingerWidget);
    layout->addSpacing(10);
    layout->addWidget(m_buttonTuple);

    m_buttonTuple->leftButton()->setText(tr("Re-enter"));
    m_buttonTuple->rightButton()->setText(tr("Done"));

    m_buttonTuple->setVisible(false);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    connect(m_buttonTuple->leftButton(), &QPushButton::clicked, this, &AddFingerPage::reEnrollStart);
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &AddFingerPage::saveThumb);
}

void AddFingerPage::setFingerModel(FingerModel *model)
{
    m_model = model;

    connect(model, &FingerModel::enrollStatusChanged, this, &AddFingerPage::onEnrollStatusChanged);
    onEnrollStatusChanged(model->enrollStatus());
}

void AddFingerPage::setUsername(const QString &name)
{
    m_username = name;
}

void AddFingerPage::saveThumb()
{
    emit requestSaveThumb(m_username);

    emit back();
}

void AddFingerPage::reEnrollStart()
{
    m_buttonTuple->setVisible(false);

    emit requestReEnrollStart(m_thumb);
}

void AddFingerPage::onEnrollStatusChanged(FingerModel::EnrollStatus status)
{
    if (status == FingerModel::EnrollStatus::Ready) {
        m_fingerWidget->setFrequency(tr("Put the finger in fingerprint reader to add"));
        return;
    }

    if (status == FingerModel::EnrollStatus::Next) {
        QString tip(tr("Entering"));
        if (m_frequency == 1)
            m_fingerWidget->setFrequency(tip);
        else
            m_fingerWidget->setFrequency(tip + " " + QString("%1 times").arg(m_frequency));

        ++m_frequency;

        return;
    }

    if (status == FingerModel::EnrollStatus::Finished) {
        m_fingerWidget->setFrequency(tr("Add successfully"));
        m_buttonTuple->setVisible(true);

        m_frequency = 1;
        return;
    }
}
