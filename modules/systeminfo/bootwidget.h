#ifndef BOOTWIDGET_H
#define BOOTWIDGET_H

#include "contentwidget.h"
#include "switchwidget.h"
#include <QListWidget>
using namespace dcc::widgets;

namespace dcc{
namespace systeminfo{

class BootWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit BootWidget(QWidget* parent = 0);
    void setDefaultEntry(const QString& value);

signals:
    void enableTheme(bool value);
    void bootdelay(bool value);
    void defaultEntry(const QString& item);

public slots:
    void setChecked(bool value);
    void setEntryList(const QStringList& list);
    void onItemActivated(QListWidgetItem* item);

private:
    QString m_defaultEntry;
    SwitchWidget* m_boot;
    SwitchWidget* m_theme;
    QListWidget* m_bootList;
};

}
}
#endif // BOOTWIDGET_H
