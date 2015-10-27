#ifndef MIRRORSCONTROLWIDGET_H
#define MIRRORSCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QMap>

#include <libdui/dswitchbutton.h>
#include <libdui/dtextbutton.h>

#include "mirroritemwidget.h"

DUI_USE_NAMESPACE

class MirrorsControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MirrorsControlWidget(QWidget *parent = 0);
    ~MirrorsControlWidget();

private:
    void loadMirrorList();

private:
    DSwitchButton *m_updateSwitchBtn;
    DTextButton *m_changeMirrorBtn;
    DTextButton *m_cancelBtn;
    DTextButton *m_applyBtn;
    QListWidget *m_mirrorsList;
    QMap<QListWidgetItem *, MirrorItemWidget *> *m_mirrorItems;
};

#endif // MIRRORSCONTROLWIDGET_H
