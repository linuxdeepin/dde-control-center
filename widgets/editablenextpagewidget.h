#ifndef EDITABLENEXTPAGEWIDGET_H
#define EDITABLENEXTPAGEWIDGET_H

#include "settingsitem.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

class EditableNextPageWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit EditableNextPageWidget(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);

signals:
    void acceptNextPage() const;
    void selected() const;
    void textChanged(const QString &oldText, const QString &newText) const;

private:
    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void toEditMode();
    void toEchoMode();

private:
    QLabel *m_name;
    QLabel *m_selectIcon;
    QLineEdit *m_edit;
    DImageButton *m_editButton;
    DImageButton *m_nextButton;
};

}

}

#endif // EDITABLENEXTPAGEWIDGET_H
