#include <QIcon>
#include "optionwidget.h"
using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;
OptionWidget::OptionWidget(const bool state, QWidget *parent)
    :SettingsItem(parent),
      m_delete(new DImageButton)
{
    m_checkedIconLabel = new QLabel;
    m_checkedIconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_optionIcon = new QLabel;
    m_optionIcon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_displayName = new NormalLabel;
    m_displayName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_execPath = new QLabel;
    m_execPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_execPath->setVisible(state);

    m_delete->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //send delete dbus
    connect(m_delete, &DImageButton::clicked, [ = ] {
        emit removeItem(m_CategoryItem);
    });

    m_delete->setFixedSize(17, 17);
    m_optionIcon->setFixedSize(17, 17);
    m_checkedIconLabel->setFixedSize(17, 17);

    QHBoxLayout *m_leftOption = new QHBoxLayout;
    m_leftOption->addWidget(m_optionIcon,  0, Qt::AlignCenter);
    m_leftOption->addSpacing(14);
    m_leftOption->addWidget(m_displayName, 0, Qt::AlignCenter);
    m_leftOption->addWidget(m_execPath,        0, Qt::AlignCenter);
    m_leftOption->addStretch();
    m_leftOption->setContentsMargins(10,0,0,0);

    QHBoxLayout *m_rightOption = new QHBoxLayout;
    m_rightOption->setSpacing(0);
    m_rightOption->addStretch();
    m_rightOption->addWidget(m_checkedIconLabel, 0, Qt::AlignCenter);
    m_rightOption->addWidget(m_delete,           0, Qt::AlignCenter);
    m_checkedIconLabel->setVisible(false);
    m_delete->setVisible(false);

    m_mainlayout = new QHBoxLayout;
    m_mainlayout->setSpacing(0);
    m_mainlayout->addLayout(m_leftOption, Qt::AlignLeft);
    m_mainlayout->addStretch();
    m_mainlayout->addLayout(m_rightOption, Qt::AlignRight);
    m_mainlayout->setContentsMargins(0, 0, 10, 0);

    setLayout(m_mainlayout);
    setObjectName("OptionWidget");
}

void OptionWidget::setItem(const QJsonObject &item)
{
    m_CategoryItem = item;

    if (item["Name"].toString().isEmpty()) {
        m_displayName->setText(item["DisplayName"].toString());
    } else {
        m_displayName->setText(item["Name"].toString());
    }
    const QIcon &icon = QIcon::fromTheme(item["Icon"].toString());

    if (!icon.isNull()) {
        m_optionIcon->setPixmap(icon.pixmap(QSize(17,17)));
    } else {
        const QIcon &icon = QIcon::fromTheme("application-x-desktop");
        m_optionIcon->setPixmap(icon.pixmap(QSize(17, 17)));
    }
    m_execPath->setText(" ("+item["Exec"].toString()+ ")");
    setAccessibleName(item["Id"].toString());
}
void OptionWidget::setDelete(const bool delchecked)     //删除
{
    if (m_userCheck) {
        if (delchecked) {
            m_delete->setVisible(true);
            m_checkedIconLabel->setVisible(false);
        } else {
            m_delete->setVisible(false);
            if (m_checked) {
                m_checkedIconLabel->setVisible(true);
            }
        }
    }
}

void OptionWidget::setMime(const QString &mime)
{
    m_mime = mime;
}

void OptionWidget::setUserCheck(const bool check)
{
    m_userCheck = check;
}

const QString OptionWidget::id() const
{
    return m_CategoryItem["Id"].toString();
}

const QString OptionWidget::name() const
{
    return m_CategoryItem["Name"].toString();
}

const QString OptionWidget::displayName() const
{
    return m_displayName->text();
}

const QString OptionWidget::mime() const
{
    return m_mime;
}

const QString OptionWidget::exec() const
{
    return m_execPath->text();
}

bool OptionWidget::checked() const
{
    return m_checked;
}

void OptionWidget::setChecked(const bool checked)      //判断是否显示设置图标
{
    m_checkedIconLabel->setVisible(checked);
    m_delete->setVisible(false);
    setCheckedIcon(QPixmap(":/defapp/icons/select.png"));
    m_checked = checked;
    emit checkedChanged(m_checked);
}

void OptionWidget::setCheckedIcon(const QPixmap &icon)
{
    //set default checkIcon
    m_checkedIconLabel->setPixmap(icon);
}

bool OptionWidget::userCheck() const
{
    return m_userCheck;
}

QHBoxLayout *OptionWidget::layout()
{
    return m_mainlayout;
}

void OptionWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (QMouseEvent::MouseButtonRelease) {
        //do someting
        setChecked(true);
        emit setDefault();
    }
}
