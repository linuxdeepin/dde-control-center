#ifndef SYSTEMINFOWIDGET_H
#define SYSTEMINFOWIDGET_H

#include "modulewidget.h"
#include "nextpagewidget.h"
#include "titlevalueitem.h"
#include "systeminfomodel.h"

using namespace dcc;

namespace dcc{
namespace systeminfo{

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
    TitleValueItem* m_version;
    TitleValueItem* m_type;
    TitleValueItem* m_processor;
    TitleValueItem* m_memory;
    TitleValueItem* m_disk;

    NextPageWidget* m_copyright;
    NextPageWidget* m_boot;
    SystemInfoModel* m_model;
};

}
}
#endif // SYSTEMINFOWIDGET_H
