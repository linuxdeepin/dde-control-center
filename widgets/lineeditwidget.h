#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include "settingsitem.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

namespace dcc {
namespace widgets {

class LineEditWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit LineEditWidget(QFrame *parent = 0);

    inline QLineEdit *textEdit() const { return m_edit; }
    inline QString text() const { return m_edit->text(); }
    void setTitleVisible(const bool visible) { m_title->setVisible(visible); }
    void addRightWidget(QWidget *widget);
    void setReadOnly(const bool state);

public slots:
    void setTitle(const QString &title);
    void setText(const QString &text);
    void setPlaceholderText(const QString &text);

protected:
    void mousePressEvent(QMouseEvent *e);

private:
    QLabel *m_title;
    QLineEdit *m_edit;
    QHBoxLayout *m_mainLayout;
};

}
}

#endif // LINEEDITWIDGET_H
