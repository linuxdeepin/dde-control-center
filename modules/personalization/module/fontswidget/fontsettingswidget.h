#ifndef FONTSETTINGSWIDGET_H
#define FONTSETTINGSWIDGET_H


#include <QObject>
#include <QVBoxLayout>
#include <translucentframe.h>
#include <QDebug>

namespace dcc
{
namespace widgets
{
class SettingsGroup;
class NextPageWidget;
}
namespace personalization
{
class PersonalizationModel;
class FontSettingsWidget : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit FontSettingsWidget(QWidget *parent = 0);
    void setModel(PersonalizationModel *const model);

signals:
    void showStandardFont();
    void showMonoFont();

private:
    QVBoxLayout    *m_mainlayout;
    widgets::SettingsGroup  *m_fontWidget;
    widgets::NextPageWidget *m_standard;
    widgets::NextPageWidget *m_mono;
};
}
}


#endif // FONTSETTINGSWIDGET_H
