#ifndef NAVGATIONDELEGATE_H
#define NAVGATIONDELEGATE_H

#include <QItemDelegate>
#include <QIcon>
#include <QApplication>

static QPixmap getPixmap(const QString &path) {
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (devicePixelRatio > ratio) {
        pixmap.load(qt_findAtNxFile(path, devicePixelRatio, &ratio));

        pixmap = pixmap.scaled(devicePixelRatio / ratio * pixmap.width(),
                               devicePixelRatio / ratio * pixmap.height(),
                               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pixmap.setDevicePixelRatio(devicePixelRatio);
    } else {
        pixmap.load(path);
    }

    return pixmap;
}

namespace dcc {

class NavgationDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit NavgationDelegate(QObject *parent = 0);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}

#endif // NAVGATIONDELEGATE_H
