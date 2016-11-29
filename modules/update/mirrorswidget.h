#ifndef MIRRORSWIDGET_H
#define MIRRORSWIDGET_H

#include "contentwidget.h"
#include "mirroritem.h"
#include "settingsgroup.h"

#include <types/mirrorinfolist.h>

using namespace dcc;

class MirrorsWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit MirrorsWidget(QWidget* parent = 0);
    void setDefaultMirror(const QString& mirror);
    void setMirrorInfoList(const MirrorInfoList& list);

signals:
    void mirrorName(const QString& name, const QString& src);

public slots:
    void setCurItem(MirrorItem* item);

private:
    MirrorItem* m_curItem;
    SettingsGroup* m_group;
    QString m_defaultMirror;
};

#endif // MIRRORSWIDGET_H
