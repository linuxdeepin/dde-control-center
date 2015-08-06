#include "grubbackground.h"
#include "dbustheme.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>

GrubBackground::GrubBackground(GrubThemeDbus *themeDbus, QWidget *parent) :
    QFrame(parent),
    m_themeDbus(themeDbus),
    m_isDrop(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    updateBackground(m_themeDbus->background());
    connect(m_themeDbus, &GrubThemeDbus::backgroundChanged, this, &GrubBackground::updateBackground);

    setAcceptDrops(true);
}

QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT void qt_blurImage( QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0 );
QT_END_NAMESPACE

void GrubBackground::paintEvent(QPaintEvent *e)
{
    if(!m_background.isNull()){
        QPainter pa(this);

        if(m_isDrop){
            QImage image = m_background.toImage();
            QImage scrImage = image;
            QPainter p(&image);
            qt_blurImage(&p, scrImage, 30, true, false);
            pa.drawImage(rect(), scrImage);
            pa.fillRect(rect(), QColor(0, 0, 0, 10));
        }else{
            pa.drawPixmap(rect(), m_background);
        }
    }

    QFrame::paintEvent(e);
}

void GrubBackground::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls()){
        e->acceptProposedAction();
        m_isDrop = true;
        update();
    }
}

void GrubBackground::dragLeaveEvent(QDragLeaveEvent*)
{
    m_isDrop = false;
    update();
}

void GrubBackground::dropEvent(QDropEvent *e)
{
    QString path = e->mimeData()->urls()[0].toLocalFile();
    if(path!=""){
        m_themeDbus->SetBackgroundSourceFile(path);
        e->acceptProposedAction();
        m_isDrop = false;
        update();
    }
}

bool GrubBackground::updateBackground(const QString &filename)
{
    bool result = m_background.load(filename);
    if(result){
        setMinimumSize(m_background.size());
        update();
    }
    return result;
}

