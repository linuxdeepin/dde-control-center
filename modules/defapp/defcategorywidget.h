#ifndef DEFCATEGORYWIDGET_H
#define DEFCATEGORYWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "optionwidget.h"

#include <QWidget>

class DefCategoryWidget : public ModuleWidget
{
    Q_OBJECT
public:
    explicit DefCategoryWidget();
    void add(QString name, QString value, QString mime);

signals:
    void setDefaultApp(QString name,QString id);

public slots:
    void onDeleteRunner();
    void onSetRunner(QString mime, QString value);
    void onDefaultAppSet(QString id);

private:
    dcc::SettingsGroup *m_userGroup;
    QWidget            *m_listWidget;
    dcc::OptionWidget  *m_optionWidget;
    QLayoutItem        *m_userGroupLayout;

};
#endif // DEFCATEGORYWIDGET_H
