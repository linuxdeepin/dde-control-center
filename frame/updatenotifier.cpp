#include "updatenotifier.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>

static const QString IgnoredPkgsKey = "IgnoredPackages";

UpdateNotifier::UpdateNotifier(QWidget *parent)
    : QFrame(parent),
      m_icon(new QLabel),
      m_title(new NormalLabel(tr("Updates"))),
      m_content(new TipsLabel(tr("Updates detected, click to view"))),
      m_closeButton(new DImageButton(this)),
      m_settings(new QSettings("deepin", "dcc-update-notifier")),
      m_updaterInter(new com::deepin::lastore::Updater("com.deepin.lastore",
                                                       "/com/deepin/lastore",
                                                       QDBusConnection::systemBus(), this))
{
    setVisible(false);
    setMouseTracking(true);
    setFixedSize(360, 80);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QIcon icon = QIcon::fromTheme("system-updated", QIcon::fromTheme("application-default-icon"));
    m_icon->setFixedSize(48, 48);
    m_icon->setPixmap(icon.pixmap(m_icon->size()));

    m_closeButton->setFixedSize(14, 14);

    m_closeButton->setObjectName("NotifyCloseButton");
    m_closeButton->move(x() + width() - 14 - 10, y() + 10);
    m_closeButton->setVisible(false);

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentLayout->setMargin(0);

    contentLayout->addStretch();
    contentLayout->addWidget(m_title);
    contentLayout->addSpacing(4);
    contentLayout->addWidget(m_content);
    contentLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainLayout->addSpacing(40);
    mainLayout->addWidget(m_icon);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(contentLayout);
    mainLayout->addSpacing(40);
    mainLayout->addStretch();

    setLayout(mainLayout);

    m_updaterInter->setSync(false);
    updatablePkgsChanged(m_updaterInter->updatablePackages());
    connect(m_updaterInter, &com::deepin::lastore::Updater::UpdatablePackagesChanged,
            this, &UpdateNotifier::updatablePkgsChanged);

    connect(m_closeButton, &DImageButton::clicked, this, &UpdateNotifier::ignoreUpdates);
}

void UpdateNotifier::mouseReleaseEvent(QMouseEvent *event)
{
    QFrame::mouseReleaseEvent(event);

    ignoreUpdates();

    emit clicked();
}

void UpdateNotifier::enterEvent(QEvent *event)
{
    QFrame::enterEvent(event);

    m_closeButton->setVisible(true);
}

void UpdateNotifier::leaveEvent(QEvent *event)
{
    QFrame::enterEvent(event);

    m_closeButton->setVisible(false);
}

void UpdateNotifier::ignoreUpdates()
{
    setVisible(false);
    m_settings->setValue(IgnoredPkgsKey, m_updatablePkgs);
}

void UpdateNotifier::updatablePkgsChanged(const QStringList &value)
{
    if (value.length() == 0) {
        setVisible(false);
        return;
    }
    m_updatablePkgs = value;

    QStringList pkgs = m_settings->value(IgnoredPkgsKey).toStringList();
    setVisible(!comparePkgLists(pkgs, value));
}

bool UpdateNotifier::comparePkgLists(QStringList val1, QStringList val2)
{
    if (val1.length() != val2.length()) return false;

    qSort(val1.begin(), val1.end());
    qSort(val2.begin(), val2.end());

    return val1 == val2;
}
