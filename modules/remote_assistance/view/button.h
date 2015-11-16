#ifndef WIDGET_H_NZTWOD8T
#define WIDGET_H_NZTWOD8T

#include <QFrame>
#include "libdui_global.h"

DUI_BEGIN_NAMESPACE
class DImageButton;
DUI_END_NAMESPACE

class QHBoxLayout;
class QVBoxLayout;
class QLabel;

class Button : public QFrame
{
    Q_OBJECT
public:
    Button(const QString& title, const QString& tip, QWidget* p=nullptr);

    void setNormalIcon(const QString&);
    void setHoverIcon(const QString&);
    void setPressedIcon(const QString&);
    void setTitle(const QString&);
    void setTip(const QString&);

signals:
    void clicked();

public:
    void enterEvent(QEvent*) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent*) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
    QHBoxLayout* m_hlayout;
    QVBoxLayout* m_vlayout;
    DUI_NAMESPACE::DImageButton* m_icon;
    QLabel* m_title;
    QLabel* m_tip;
    DUI_NAMESPACE::DImageButton* m_arrow;

private slots:
    void emitClicked() { emit clicked(); }
    void onMousePressed(QMouseEvent*e);
    void onMouseReleased(QMouseEvent*e);
};

#endif /* end of include guard: WIDGET_H_NZTWOD8T */
