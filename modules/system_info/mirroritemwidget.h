#ifndef MIRRORITEMWIDGET_H
#define MIRRORITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QDebug>

#include <libdui/dimagebutton.h>

DUI_USE_NAMESPACE

class MirrorItemWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ selected NOTIFY selectStateChanged)

signals:
    void selectStateChanged(bool state) const;

public:
    explicit MirrorItemWidget(QWidget *parent = 0);

    inline bool selected() const {return m_selected;}
    void setSelected(bool state);

private:
    DImageButton *m_selectedBtn;
    QLabel *m_mirrorName;
    QLabel *m_mirrorUrl;

    bool m_selected = false;
};

#endif // MIRRORITEMWIDGET_H
