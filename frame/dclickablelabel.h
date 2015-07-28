#ifndef DCLICKABLELABEL_H
#define DCLICKABLELABEL_H

#include <QLabel>
#include <QPixmap>

class DClickablePictureLabel : public QLabel
{
    Q_OBJECT

public:
    DClickablePictureLabel(QWidget *parent, const QString & normalPic,
                           const QString & hoverPic, const QString & pressPic);
    ~DClickablePictureLabel();

    void enterEvent(QEvent * event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

signals:
    void clicked();
    void stateChanged();

private:
    void changeState();

private:
    enum State {Normal, Hover, Press};

    State m_state = Normal;

    QPixmap m_normalPic;
    QPixmap m_hoverPic;
    QPixmap m_pressPic;
};

#endif // DCLICKABLELABEL_H
