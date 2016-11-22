#ifndef CUSTOMCONTENT_H
#define CUSTOMCONTENT_H


#include "contentwidget.h"
#include "settingsgroup.h"
#include "inputitem.h"
#include <QPushButton>

using namespace dcc;

class CustomContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit CustomContent(QWidget *parent = 0);

private:
    SettingsGroup *m_commandGroup;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    InputItem* m_name;
    InputItem* m_command;
};

#endif // CUSTOMCONTENT_H
