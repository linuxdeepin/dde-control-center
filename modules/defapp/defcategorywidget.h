#ifndef DEFCATEGORYWIDGET_H
#define DEFCATEGORYWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "optionwidget.h"

#include <QWidget>
#include "defappmodel.h"

class DefCategoryWidget : public ModuleWidget
{
    Q_OBJECT
public:
    explicit DefCategoryWidget();
    void setCategory(Category * const category);
    void addItems(const QStringList &list);
    void removeItems(const QStringList &list);

signals:
    void setDefaultApp(const QString &name, const QString &id);

public slots:
    void onSetRunner(const QString &mime, const QString &value);
    void onDefaultAppSet(const QString &id);
    void onEdit();

private:
    void addItem(const QString &name, const QString &value, const QString &mime);
    void removeItem(const QString &id);
    dcc::OptionWidget * findObject(const QString &id);

private:
    dcc::SettingsGroup *m_userGroup;
    QWidget            *m_listWidget;
    dcc::OptionWidget  *m_optionWidget;
    QLayoutItem        *m_userGroupLayout;

};
#endif // DEFCATEGORYWIDGET_H
