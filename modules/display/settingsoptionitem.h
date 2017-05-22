#ifndef SETTINGSOPTIONITEM_H
#define SETTINGSOPTIONITEM_H

#include <QWidget>
#include <QLabel>

namespace dcc {

namespace display {

class SettingsOptionItem : public QFrame
{
    Q_OBJECT

public:
    explicit SettingsOptionItem(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setSelected(const bool checked);

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    QLabel *m_title;
    QLabel *m_icon;
};

} // namespace display

} // namespace dcc

#endif // SETTINGSOPTIONITEM_H
