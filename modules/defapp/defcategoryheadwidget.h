#ifndef DEFCATEGORYHEADWIDGET_H
#define DEFCATEGORYHEADWIDGET_H

#include "settingsitem.h"
#include "funcbutton.h"

using namespace dcc;
class DefCategoryHeadWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit DefCategoryHeadWidget(const QString &name);

signals:
    void editChanged(const bool edit);

public slots:
    void setedit(const bool edit);
    void clicked();

private:
    QLabel     *m_title;
    FuncButton *m_edit;
    bool        m_editState = true;
};

#endif // DEFCATEGORYHEADWIDGET_H
