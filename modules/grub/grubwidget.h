#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include <libdui/darrowlineexpand.h>
#include <libdui/dheaderline.h>
#include <libdui/libdui_global.h>
#include <libdui/dbuttonlist.h>

#include "grubbackground.h"
#include "dbusgrub.h"

DUI_USE_NAMESPACE

class GrubWidget : public QFrame
{
public:
    explicit GrubWidget(QWidget *parent = 0);
    ~GrubWidget();

private:
    QVBoxLayout *m_layout;
    DHeaderLine *m_header;
    DArrowLineExpand *m_arrowDefaultBoot;
    DArrowLineExpand *m_arrowBootDelay;
    DArrowLineExpand *m_arrowTextColor;
    DArrowLineExpand *m_arrowSelectedTextColor;
    GrubThemeDbus *m_themeDbus;
    GrubDbus *m_grubDbus;
    GrubBackground *m_grubBackground;
    DButtonList *m_bootEntryList, *m_bootMenuTitle;
    QHBoxLayout *m_selectTimeoutLayout;
    QLabel *m_tooltip;

    void init();
    Q_SLOT void setDefaultEntry(const QString &entry);
    Q_SLOT void updatingChanged(bool updating);
    Q_SLOT void resetTooltip();
};

#endif // GRUBWIDGET_H
