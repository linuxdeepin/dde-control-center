#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include "contentwidget.h"
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
}
namespace personalization
{
class PersonalizationModel;
class Theme;
class ThemeWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = 0);

signals:
    void requestSetDefault(const QJsonObject &value);

public slots:
    void setModel(PersonalizationModel *const model);

private:
    QVBoxLayout *m_mainlayout;
    widgets::TranslucentFrame *m_widget;
    Theme *m_windowTheme;
    Theme *m_iconTheme;
    Theme *m_mouseTheme;
};

}
}

#endif // THEMEWIDGET_H
