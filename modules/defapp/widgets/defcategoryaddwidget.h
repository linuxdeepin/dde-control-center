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

signals:
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

public slots:
    void clicked();

private:
    FuncButton *m_add;
    QString m_category;
};
}
}



#endif // DEFCATEGORYADDWIDGET_H
