#include "optionwidget.h"


dcc::OptionWidget::OptionWidget(QWidget *):m_delete(new DImageButton(":/list_delete_normal.png",
                                                                     ":/list_delete_hover.png",
                                                                     ":/list_delete_press.png",nullptr)) {



    m_checkedIconLabel = new QLabel;
    m_checkedIconLabel->setMinimumSize(0, 0);
    m_checkedIconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_optionIcon = new QLabel;
    m_optionIcon->setMinimumSize(0, 0);
    m_optionIcon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_displayName = new QLabel;
    m_displayName->setObjectName("OptionName");
    m_displayName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_delete->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_delete->setFixedSize(17,17);
    m_optionIcon->setFixedSize(17,17);
    m_checkedIconLabel->setFixedSize(17,17);

    QHBoxLayout *m_leftOption = new QHBoxLayout;
    m_leftOption->addWidget(m_optionIcon, Qt::AlignLeft);
    m_leftOption->addWidget(m_displayName, Qt::AlignLeft);

    QHBoxLayout *m_rightOption = new QHBoxLayout;
    m_rightOption->addWidget(m_checkedIconLabel, Qt::AlignLeft);
    m_rightOption->addWidget(m_delete,           Qt::AlignLeft);
    m_checkedIconLabel->setVisible(false);
    m_delete->setVisible(false);

    m_mainlayout = new QHBoxLayout;
    m_mainlayout->addLayout(m_leftOption,  Qt::AlignLeft);
    m_mainlayout->addStretch();
    m_mainlayout->addLayout(m_rightOption, Qt::AlignRight);
    m_mainlayout->setContentsMargins(5, 0, 5, 0);

    setLayout(m_mainlayout);
}

void dcc::OptionWidget::setName(const QString &name) {
    m_optionName = name;
}

void dcc::OptionWidget::setDisplayName(const QString &displayname)
{
    m_displayName->setText(displayname);
}

void dcc::OptionWidget::setId(const QString &id)
{
        m_optionid = id;
}

void dcc::OptionWidget::setIcon(const QString &icon) {
    setIcon(QPixmap(icon));
}

void dcc::OptionWidget::setIcon(const QPixmap &icon) {
    m_optionIcon->setPixmap(icon);
}

void dcc::OptionWidget::setDelete() {   //删除
    m_delete->setVisible(true);
    m_checkedIconLabel->setVisible(false);
    //send delete dbus

    connect(m_delete, &DImageButton::clicked, [=]{
        emit deleteClicked(id());
    });
}

void dcc::OptionWidget::setMime(const QString &mime)
{
    m_mime = mime;
}

const QString dcc::OptionWidget::id() const
{
      return m_optionid;
}

const QString dcc::OptionWidget::name() const {
    return m_optionName;
}

const QString dcc::OptionWidget::displayName() const
{
    return m_displayName->text();
}

const QString dcc::OptionWidget::mime() const
{
    return m_mime;
}

bool dcc::OptionWidget::checked() const {
    return m_checked;
}

void dcc::OptionWidget::setChecked(const bool checked) {    //判断是否显示设置图标
    m_checkedIconLabel->setVisible(checked);
    m_delete->setVisible(false);
    setCheckedIcon(QPixmap(":/select.png"));
    m_checked = checked;
    emit checkedChanged(m_checked);
}

void dcc::OptionWidget::setCheckedIcon(const QPixmap &icon) {
    //set default checkIcon
    m_checkedIconLabel->setPixmap(icon);
    checkedIconChanged(icon);
}

QHBoxLayout *dcc::OptionWidget::layout() {
    return m_mainlayout;
}

void dcc::OptionWidget::mouseReleaseEvent(QMouseEvent *) {
    if(QMouseEvent::MouseButtonRelease) {
        //do someting
        emit set(m_mime,m_optionid);
        setChecked(true);
    }
}
