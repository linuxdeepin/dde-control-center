#include "fingerpage.h"
#include "settingsgroup.h"
#include "labels/tipslabel.h"
#include "translucentframe.h"
#include "widgets/fingeritem.h"
#include "settingsheaderitem.h"
#include "fingermodel.h"

#include <QVBoxLayout>
#include <QPushButton>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;

FingerPage::FingerPage(User *user, QWidget *parent)
    : ContentWidget(parent)
    , m_listGrp(new SettingsGroup(tr("Fingerprint List")))
    , m_user(user)
{
    setTitle(tr("Fingerprint Password"));

    m_addBtn = new QPushButton;
    m_addBtn->setText(tr("Add Fingerprint"));

    m_cleanBtn = new QPushButton;
    m_cleanBtn->setText(tr("Clear Fingerprint"));
    m_cleanBtn->setObjectName("FingerClean");

    TipsLabel *tip = new TipsLabel;
    tip->setText(tr("Fingerprint password is used in system login, application authorization and other scenes. Scan the fingerprint when password needed."));
    tip->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    layout->addWidget(m_listGrp);
    layout->addWidget(m_cleanBtn);
    layout->addWidget(m_addBtn);
    layout->addWidget(tip);
    layout->addStretch();

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    connect(m_addBtn, &QPushButton::clicked, this, &FingerPage::addThumb);
    connect(m_cleanBtn, &QPushButton::clicked, this, &FingerPage::cleanThumbs);
}

void FingerPage::setFingerModel(FingerModel *model)
{
    m_model = model;

    connect(model, &FingerModel::thumbsListChanged, this, &FingerPage::onThumbsListChanged);

    onThumbsListChanged(model->thumbsList());
}

void FingerPage::onThumbsListChanged(const QList<FingerModel::UserThumbs> &thumbs)
{
    QStringList thumb = thumbsLists;

    m_listGrp->clear();

    for (const FingerModel::UserThumbs &u : thumbs) {
        if (u.username == m_user->name()) {
            int i = 1;
            foreach (const QString &title, u.userThumbs) {
                FingerItem *item = new FingerItem;
                item->setTitle(tr("Fingerprint") + QString::number(i++));

                m_listGrp->appendItem(item);
                thumb.removeOne(title);
            }

            const int size = thumb.size();
            m_addBtn->setVisible(size);
            m_listGrp->headerItem()->setVisible(size);
            m_cleanBtn->setVisible(true);

            if (!thumb.isEmpty())
                m_notUseThumb = thumb.first();
            return;
        }
    }

    m_addBtn->setVisible(true);
    m_notUseThumb = thumb.first();
    m_listGrp->headerItem()->setVisible(false);
    m_cleanBtn->setVisible(false);
}

void FingerPage::addThumb()
{
    emit requestAddThumbs(m_user->name(), m_notUseThumb);
}

void FingerPage::cleanThumbs()
{
    emit requestCleanThumbs(m_user);
}
