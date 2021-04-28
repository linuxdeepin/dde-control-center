#include "customfloatingmessage.h"
#include "window/modules/unionid/notificationmanager.h"

#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>

CustomFloatingMessage::CustomFloatingMessage(MessageType type, DWidget *parent)
    : DWidget(parent),
      m_MessageType(type)
{
    m_MessageType = type;

    m_icon = new DLabel(this);
    m_icon->setMinimumSize(24, 24);
    m_icon->setContentsMargins(0, 0, 0, 0);

    m_toast = new DLabel(this);
    m_toast->setMinimumSize(60, 22);
    QPalette pe;
    QColor color(255, 255, 255);
    color.setAlphaF(0.65);
    pe.setColor(QPalette::WindowText, color);
    m_toast->setPalette(pe);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addSpacing(16);
    hlayout->addStretch();
    hlayout->addWidget(m_icon);
    hlayout->addWidget(m_toast);
    hlayout->addStretch();
    hlayout->addSpacing(16);
    hlayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(16, 0, 0, 16);

    setLayout(vlayout);
    setMinimumSize(120, 50);

    m_type = DGuiApplicationHelper::instance()->themeType();
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CustomFloatingMessage::onThemeTypeChanged);

    m_show = new QTimer;
    connect(m_show, &QTimer::timeout, this, &CustomFloatingMessage::onTimer);

    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->showNormal();
}

CustomFloatingMessage::~CustomFloatingMessage()
{
    Q_EMIT signal_setFlag();
}

void CustomFloatingMessage::setDuration(const int m)
{
    m_show->start(m);
    show();
}

void CustomFloatingMessage::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (DGuiApplicationHelper::LightType == m_type) {
        QColor color(0, 0, 0);
        color.setAlphaF(0.65);
//        painter.fillRect(rect(), color);
        painter.setPen(QColor(0, 0, 0, 0));
        painter.setBrush(QBrush(color));
    } else if (DGuiApplicationHelper::DarkType == m_type) {
        QColor color(31, 31, 31);
        color.setAlphaF(0.85);
//        painter.fillRect(rect(), color);
        painter.setPen(QColor(31, 31, 31, 0));
        painter.setBrush(QBrush(color));
    }
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    // rect: 绘制区域  15：圆角弧度
    painter.drawRoundedRect(rect, 5, 5);
    return QWidget::paintEvent(event);
}

void CustomFloatingMessage::setMessage(const QString &error)
{
    m_toast->setText(error);
}

void CustomFloatingMessage::setIcon(const QString &path)
{
    QImage img(path);
    img.load(path);
    m_icon->setFixedSize(img.size());
    m_icon->setPixmap(QPixmap::fromImage(img));
}

void CustomFloatingMessage::CustomFloatingMessage::onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType)
{
    m_type = themeType;
    update();
}

void CustomFloatingMessage::onTimer()
{
    m_show->stop();
    if (TransientType == m_MessageType)
        this->close();
    Q_EMIT signal_setFlag();
}

