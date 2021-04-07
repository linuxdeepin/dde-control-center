#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

private Q_SLOTS:
    void onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType);

private:
    DGuiApplicationHelper::ColorType m_type;
};

#endif // CUSTOMWIDGET_H
