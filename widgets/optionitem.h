#ifndef OPTIONITEM_H
#define OPTIONITEM_H

#include "settingsitem.h"
#include "labels/normallabel.h"

#include <QHBoxLayout>
#include <QPointer>

namespace dcc {
namespace widgets {

class OptionItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit OptionItem(QWidget *parent = 0);
    explicit OptionItem(QString title, bool selected, QWidget *parent = 0);

    void setTitle(const QString &title);
    void setTitleFixedHeight(int height);
    void setTitleWidget(QWidget *titleWidget);
    void setContentWidget(QWidget *contentWidget);

    inline bool selected() const { return m_selected; }
    void setSelected(bool selected);

signals:
    void selectedChanged(const bool selected) const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

protected:
    QString m_title;
    bool m_selected;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_titleLayout;
    QWidget *m_titleFrame;
    QPointer<QWidget> m_titleWidget;
    QPointer<QWidget> m_contentWidget;
    QLabel *m_selectedIcon;
};

} // namespace widgets;
} // namespace dcc;

#endif // OPTIONITEM_H
