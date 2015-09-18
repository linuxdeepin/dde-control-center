#ifndef DSHINELABEL
#define DSHINELABEL
#include <QtWidgets/QLabel>
#include <QtGui/QPalette>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>

//The label can just control the fade in or out of text

class DBreathingLabel: public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)
public:
    DBreathingLabel(QWidget *parent=0, Qt::WindowFlags f = 0);
    DBreathingLabel(const QString & text, QWidget * parent = 0,
                    Qt::WindowFlags f = 0);
    ~DBreathingLabel();
    void setColor(QColor color);

public slots:
    void showLabel();
    void hideLabel();
    void setDuration(int duration);
private:
    int alpha_;
    int alpha() const;
    void setAlpha(int alpha);
    QPropertyAnimation* m_showAnimation;
    QPropertyAnimation* m_hideAnimation;
    QPalette m_palette;
};
#endif // DSHINELABEL

