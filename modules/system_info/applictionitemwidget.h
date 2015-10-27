#ifndef APPLICTIONITEMWIDGET_H
#define APPLICTIONITEMWIDGET_H

#include <QWidget>
#include <QLabel>

#include <libdui/dimagebutton.h>

DUI_USE_NAMESPACE

class ApplictionItemWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ selected NOTIFY selectStateChanged)

signals:
    void selectStateChanged() const;

public:
    explicit ApplictionItemWidget(QWidget *parent = 0);

    inline bool selected() const {return m_selected;}

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QLabel *m_appIcon;
    QLabel *m_appName;
    QLabel *m_appVersion;
    DImageButton *m_updateBtn;

    bool m_selected = false;
};

#endif // APPLICTIONITEMWIDGET_H
