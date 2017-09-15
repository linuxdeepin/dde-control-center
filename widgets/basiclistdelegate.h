#ifndef BASICLISTDELEGATE_H
#define BASICLISTDELEGATE_H

#include <QAbstractItemDelegate>
#include <QIcon>
#include <QApplication>

static QPixmap loadPixmap(const QString &path) {
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

namespace widgets {

class BasicListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit BasicListDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}

}

#endif // BASICLISTDELEGATE_H
