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
    Q_PROPERTY(bool checked READ selected WRITE setSelected NOTIFY selectStateChanged)

signals:
    void selectStateChanged(bool state) const;
    void clicked() const;

public:
    explicit MirrorItemWidget(QWidget *parent = 0);

    inline bool selected() const {return m_selected;}
    void setSelected(bool state);

    QSize sizeHint() const Q_DECL_OVERRIDE;

    inline void setMirrorName(const QString &name) {m_mirrorName->setText(name);}

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    DImageButton *m_selectedBtn;
    QLabel *m_mirrorName;

    bool m_selected = false;
};

#endif // MIRRORITEMWIDGET_H
