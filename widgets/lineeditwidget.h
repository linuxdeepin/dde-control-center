#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include "settingsitem.h"

#include <QLabel>
#include <QLineEdit>

namespace dcc {
namespace widgets {

class LineEditWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit LineEditWidget(QFrame *parent = 0);

    inline QLineEdit *textEdit() const { return m_edit; }
    inline QString text() const { return m_edit->text(); }

public slots:
    void setTitle(const QString &title);
    void setText(const QString &text);

private:
    QLabel *m_title;
    QLineEdit *m_edit;
};

}
}

#endif // LINEEDITWIDGET_H
