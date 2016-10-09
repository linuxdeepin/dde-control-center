#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include "frameproxyinterface.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

using dde::FrameProxyInterface;

class ModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWidget(FrameProxyInterface *frame);

    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);

    void pushWidget(ContentWidget * const w);

protected:
    QLabel *m_moduleIcon;
    QLabel *m_moduleTitle;
    QVBoxLayout *m_centeralLayout;

private:
    FrameProxyInterface *m_frameProxy;
};

#endif // MODULEWIDGET_H
