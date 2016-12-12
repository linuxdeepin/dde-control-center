#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "personalization/personalizationmodel.h"
#include "theme.h"
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
using namespace dcc;
using namespace dcc::widgets;
class ThemeWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = 0);

signals:
    void requestSetDefault(const QJsonObject &value);

public slots:
    void setModel(PersonalizationModel * const model);

private:
    QVBoxLayout *m_mainlayout;
    TranslucentFrame *m_widget;
    Theme *m_windowTheme;
    Theme *m_iconTheme;
    Theme *m_mouseTheme;
};

#endif // THEMEWIDGET_H
