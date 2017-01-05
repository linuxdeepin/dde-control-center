#ifndef SYSTEMINFOWIDGET_H
#define SYSTEMINFOWIDGET_H

#include "modulewidget.h"
#include "systeminfomodel.h"

using namespace dcc;

namespace dcc
{

namespace widgets {

class TitleValueItem;
class NextPageWidget;

}

namespace systeminfo
{

class SystemInfoWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit SystemInfoWidget(SystemInfoModel* model);

signals:
    void copyright();
    void boot();

public slots:
    void setEditor(const QString& edition);
    void setType(const QString& type);
    void setProcessor(const QString& processor);
    void setMemory(const QString& memory);
    void setDisk(const QString& disk);

private:
    widgets::TitleValueItem* m_version;
    widgets::TitleValueItem* m_type;
    widgets::TitleValueItem* m_processor;
    widgets::TitleValueItem* m_memory;
    widgets::TitleValueItem* m_disk;

    widgets::NextPageWidget* m_copyright;
    widgets::NextPageWidget* m_boot;
    SystemInfoModel* m_model;
};

}
}
#endif // SYSTEMINFOWIDGET_H
