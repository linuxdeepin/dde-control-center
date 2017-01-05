#include "grubbackgrounditem.h"
#include <QMimeDatabase>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>

namespace dcc{
namespace systeminfo{

GrubBackgroundItem::GrubBackgroundItem(QFrame *parent)
    :SettingsItem(parent)
{
    m_themeDbus = new GrubThemeDbus("com.deepin.daemon.Grub2",
                                    "/com/deepin/daemon/Grub2/Theme",
                                    QDBusConnection::sessionBus(), this);

    updateBackground(m_themeDbus->background());
//    connect(m_themeDbus, &GrubThemeDbus::BackgroundChanged, this, &GrubBackgroundItem::updateBackground);
    connect(m_themeDbus, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(onProperty(QString,QVariant)));

    setAcceptDrops(true);
}

void GrubBackgroundItem::paintEvent(QPaintEvent *e)
{
    if(!m_background.isNull()){
        QPainter pa(this);
        pa.drawPixmap(rect(), m_background);
        if(m_isDrop){
            pa.fillRect(rect(), QColor(0, 0, 0, 100));
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

    QString path = e->mimeData()->urls()[0].toLocalFile();
    if(path!=""){
        m_themeDbus->SetBackgroundSourceFile(path);
        e->acceptProposedAction();
        m_isDrop = false;
        update();
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
