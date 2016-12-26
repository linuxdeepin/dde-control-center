#ifndef DEFCATEGORYADDWIDGET_H
#define DEFCATEGORYADDWIDGET_H

#include "settingsitem.h"
#include "funcbutton.h"
#include <QObject>
#include <QFileInfo>
namespace dcc
{
namespace widgets {
class SettingsItem;
}
namespace defapp
{
class FuncButton;
class DefCategoryAddWidget : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DefCategoryAddWidget(QWidget *parent = 0);
    inline void setCategory(const QString &category){ m_category = category;}
    bool createDesktopFile(const QFileInfo &info);

signals:
    void addUserItem(const QString &category, const QString &name);
    void requestFrameAutoHide(const bool autoHide) const;

public slots:
    void clicked();

private:
    FuncButton *m_add;
    QString m_category;
};
}
}



#endif // DEFCATEGORYADDWIDGET_H
