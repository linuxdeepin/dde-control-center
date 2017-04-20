#include "grubbackgrounditem.h"
#include <QMimeDatabase>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QRect>

namespace dcc{
namespace systeminfo{

GrubBackgroundItem::GrubBackgroundItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_themeDbus = new GrubThemeDbus("com.deepin.daemon.Grub2",
                                    "/com/deepin/daemon/Grub2/Theme",
                                    QDBusConnection::systemBus(), this);

    updateBackground(m_themeDbus->background());
    connect(m_themeDbus, &GrubThemeDbus::BackgroundChanged, this, &GrubBackgroundItem::updateBackground);
    connect(m_themeDbus, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(onProperty(QString,QVariant)));

    setAcceptDrops(true);
}

void GrubBackgroundItem::setThemeEnable(const bool state)
{
    m_themeEnable = state;
    update();
}

void GrubBackgroundItem::paintEvent(QPaintEvent *e)
{
    if(!m_background.isNull()){
        QPainter painter(this);
        painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);
        int arcR = 5;
        QRect rect = this->rect();
        QPainterPath path;
        path.moveTo(arcR, 0);
        path.arcTo(0, 0, arcR * 2, arcR * 2, 90.0f, 90.0f);
        path.lineTo(0, rect.height());
        path.lineTo(rect.width(), rect.height());
        path.lineTo(rect.width(), arcR);
        path.arcTo(rect.width() - arcR * 2, 0, arcR * 2, arcR * 2, 0.0f, 90.0f);
        path.lineTo(arcR, 0);
        painter.save();
        painter.setClipPath(path);

        if (m_themeEnable)
            painter.drawPixmap(this->rect(), m_background);
        else
            painter.fillRect(this->rect(), Qt::black);

        painter.restore();
        painter.end();
        if(m_isDrop){
            painter.fillRect(this->rect(), QColor(0, 0, 0, 100));
        }
    }

    SettingsItem::paintEvent(e);
}

void GrubBackgroundItem::dragEnterEvent(QDragEnterEvent *e)
{
    QMimeDatabase b;
    foreach (QUrl url, e->mimeData()->urls()) {
        QString mime = b.mimeTypeForUrl(url).name();

        if(!mime.contains("image/"))
            continue;

        if(e->mimeData()->hasUrls()){
            e->acceptProposedAction();
            m_isDrop = true;
            update();
        }

        return;
    }
}

void GrubBackgroundItem::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);

    m_isDrop = false;
    update();
}

void GrubBackgroundItem::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->urls().isEmpty())
        return;

    QList<QUrl> urls = e->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString path = urls[0].toLocalFile();
        if(!path.isEmpty()){
            m_themeDbus->SetBackgroundSourceFile(path);
            e->acceptProposedAction();
            m_isDrop = false;
            update();
            emit requestEnableTheme(true);
        }
    }
}

bool GrubBackgroundItem::updateBackground(const QString &filename)
{
    bool result = m_background.load(filename);
    if(result){
        setMinimumWidth(m_background.width());
        setFixedHeight(m_background.height());
        update();
    }

    return result;
}

void GrubBackgroundItem::onProperty(const QString &property, const QVariant &variant)
{
//    qDebug()<<Q_FUNC_INFO<<property<<variant;
    if (property == "Background")
        updateBackground(variant.toString());
}

}
}
