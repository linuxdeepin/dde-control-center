#ifndef DYNAMICLABEL_H
#define DYNAMICLABEL_H

#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

#include <libdui/libdui_global.h>

class DynamicLabel : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int showDuration READ showDuration WRITE setShowDuration)
    Q_PROPERTY(QEasingCurve::Type showEasingType READ showEasingType WRITE setShowEasingType)
    Q_PROPERTY(int hideDuration READ hideDuration WRITE setHideDuration)
    Q_PROPERTY(QEasingCurve::Type hideEasingType READ hideEasingType WRITE setHideEasingTyp)

public:
    explicit DynamicLabel(QWidget *parent = 0);

    QString text() const;
    QLabel *label() const;
    QColor color() const;
    int duration() const DUI_DECL_DEPRECATED;
    QEasingCurve::Type easingType() const DUI_DECL_DEPRECATED;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    int hideDuration() const;
    QEasingCurve::Type hideEasingType() const;
    int showDuration() const;
    QEasingCurve::Type showEasingType() const;

public slots:
    void setText(const QString &text);
    void showLabel();
    void hideLabel();
    void delayShowLabel(int duration);
    void delayHideLabel(int duration);
    void setColor(QColor color);
    void setDuration(int duration) DUI_DECL_DEPRECATED;
    void setEasingType(QEasingCurve::Type easingType) DUI_DECL_DEPRECATED;
    void setDisappearDuration(int duration);
    void setHideDuration(int hideDuration);
    void setHideEasingTyp(QEasingCurve::Type hideEasingType);
    void setShowDuration(int showDuration);
    void setShowEasingType(QEasingCurve::Type showEasingType);

signals:
    void hideFinished();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    QLabel *m_label;
    QPropertyAnimation *m_showAnimation;
    QPropertyAnimation *m_hideAnimation;
    int m_timeout;
    QTimer m_delayTimer;
};

#endif // DYNAMICLABEL_H
