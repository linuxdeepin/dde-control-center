#include "syncstateicon.h"

#include <QMatrix>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::sync;

SyncStateIcon::SyncStateIcon(QWidget* parent) : QLabel(parent), m_centerLbl(new QLabel)
{
    m_rotateAni.setStartValue(0);
    m_rotateAni.setEndValue(360);
    m_rotateAni.setDuration(2000);
    m_rotateAni.setLoopCount(-1);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_centerLbl, 0, Qt::AlignHCenter);

    setLayout(layout);

    connect(&m_rotateAni, &QVariantAnimation::valueChanged, this,
            [=](const QVariant& value) -> void {
                QMatrix matrix;
                matrix.rotate(value.toUInt());
                m_centerLbl->setPixmap(
                    m_rotatePixmap.transformed(matrix, Qt::SmoothTransformation));
            });
}

SyncStateIcon::~SyncStateIcon() {}

void SyncStateIcon::setRotatePixmap(const QPixmap& pixmap)
{
    m_rotatePixmap = pixmap;
    m_centerLbl->setPixmap(pixmap);
}

void SyncStateIcon::play()
{
    m_rotateAni.start();
}

void SyncStateIcon::stop()
{
    m_rotateAni.stop();
}
