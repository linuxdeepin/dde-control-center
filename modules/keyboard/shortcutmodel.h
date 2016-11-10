#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#include <QObject>

class ShortcutModel : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutModel(QObject *parent = 0);

signals:

public slots:
};

#endif // SHORTCUTMODEL_H