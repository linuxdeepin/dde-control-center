#include "optionwidget.h"


dcc::OptionWidget::OptionWidget(QWidget *): m_delete(new DImageButton)
{
    m_checkedIconLabel = new QLabel;
    m_checkedIconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_optionIcon = new QLabel;
    m_optionIcon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_displayName = new NormalLabel;
    m_displayName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_execPath = new QLabel;
    m_execPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_delete->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //send delete dbus
    connect(m_delete, &DImageButton::clicked, [ = ] {
        emit removeItem(m_CategoryItem);
    });

    m_delete->setFixedSize(17, 17);
    m_optionIcon->setFixedSize(17, 17);
    m_checkedIconLabel->setFixedSize(17, 17);

    QHBoxLayout *m_leftOption = new QHBoxLayout;
    m_leftOption->setSpacing(1);
    m_leftOption->addWidget(m_optionIcon,  0, Qt::AlignLeft);
    m_leftOption->addWidget(m_displayName, 0, Qt::AlignLeft);
    m_leftOption->addWidget(m_execPath,        0, Qt::AlignLeft);
    m_leftOption->addStretch();

    QHBoxLayout *m_rightOption = new QHBoxLayout;
    m_rightOption->setSpacing(0);
    m_rightOption->addStretch();
    m_rightOption->addWidget(m_checkedIconLabel, 0, Qt::AlignLeft);
    m_rightOption->addWidget(m_delete,           0, Qt::AlignLeft);
    m_checkedIconLabel->setVisible(false);
    m_delete->setVisible(false);

    m_mainlayout = new QHBoxLayout;
    m_mainlayout->setSpacing(0);
    m_mainlayout->addLayout(m_leftOption, Qt::AlignLeft);
    m_mainlayout->addStretch();
    m_mainlayout->addLayout(m_rightOption, Qt::AlignRight);
    m_mainlayout->setContentsMargins(5, 0, 5, 0);

    setLayout(m_mainlayout);
    setObjectName("OptionWidget");
}

void dcc::OptionWidget::setItem(const QJsonObject &item)
{
    m_CategoryItem = item;
    if (item["DisplayName"].toString().isEmpty()) {
        m_displayName->setText(item["Name"].toString());
    } else {
        m_displayName->setText(item["DisplayName"].toString());
    }
    m_optionIcon->setPixmap(QPixmap(item["Icon"].toString()).scaled(17,17,Qt::KeepAspectRatio));
}
void dcc::OptionWidget::setDelete(const bool delchecked)     //删除
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

void dcc::OptionWidget::setMime(const QString &mime)
{
    m_mime = mime;
}

void dcc::OptionWidget::setUserCheck(const bool check)
{
    m_userCheck = check;
}

const QString dcc::OptionWidget::id() const
{
    return m_CategoryItem["Id"].toString();
}

const QString dcc::OptionWidget::name() const
{
    return m_CategoryItem["Name"].toString();
}

const QString dcc::OptionWidget::displayName() const
{
    return m_displayName->text();
}

const QString dcc::OptionWidget::mime() const
{
    return m_mime;
}

bool dcc::OptionWidget::checked() const
{
    return m_checked;
}

void dcc::OptionWidget::setChecked(const bool checked)      //判断是否显示设置图标
{
    m_checkedIconLabel->setVisible(checked);
    m_delete->setVisible(false);
    setCheckedIcon(QPixmap(":/defapp/icons/select.png"));
    m_checked = checked;
    emit checkedChanged(m_checked);
}

void dcc::OptionWidget::setCheckedIcon(const QPixmap &icon)
{
    //set default checkIcon
    m_checkedIconLabel->setPixmap(icon);
    checkedIconChanged(icon);
}

bool dcc::OptionWidget::userCheck() const
{
    return m_userCheck;
}

QHBoxLayout *dcc::OptionWidget::layout()
{
    return m_mainlayout;
}

void dcc::OptionWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (QMouseEvent::MouseButtonRelease) {
        //do someting
        setChecked(true);
        emit setDefault(m_CategoryItem["Id"].toString());
    }
}
