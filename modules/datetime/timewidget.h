#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QTime>
#include <QLabel>

#include <libdui/dlinkbutton.h>
#include <libdui/dimagebutton.h>

DUI_USE_NAMESPACE

class TimeSpinBox : public QFrame
{
    Q_OBJECT

public:
    TimeSpinBox(QWidget *parent = 0);

    inline const QString text() const {return m_spinLabel->text();}
    inline int getMaxNumber() const {return m_maxNumber;}
    inline int getMinNumber() const {return m_minNumber;}

public slots:
    inline void setMaxNumber(int max) {m_maxNumber = max;}
    inline void setMinNumber(int min) {m_minNumber = min;}
    void setNumber(int num);

private:
    //bool eventFilter(QObject *o, QEvent *e);

private slots:
    void addNumber();
    void subNumber();

private:
    int m_maxNumber = 0;
    int m_minNumber = 0;

    DImageButton *m_addBtn;
    DImageButton *m_subBtn;
    QLabel *m_spinLabel;
};

class NormalWidget : public QWidget
{
    Q_OBJECT

public:
    NormalWidget(QWidget *parent = 0);

signals:
    void onDoubleClick();

public slots:
    void updateDateTime();
    inline void setIs24HourFormat(const bool is24HourFormat) {m_amOrPm->setVisible(!is24HourFormat); updateDateTime();}

private:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

private:
    QLabel *m_timeLabel;
    QLabel *m_amOrPm;
    QLabel *m_tipsLabel;
};

class EditWidget : public QWidget
{
    Q_OBJECT

public:
    EditWidget(QWidget *parent = 0);

signals:
    void accept(QDateTime time);
    void cancel();

public slots:
    void updateTime();

private:
    TimeSpinBox *m_spinHour;
    TimeSpinBox *m_spinMinute;
    DLinkButton *m_setTimeButton;
    DLinkButton *m_cancelTimeButton;
};

class TimeWidget : public QFrame
{
    Q_OBJECT

public:
    TimeWidget(QWidget *parent = 0);

signals:
    void applyTime(QDateTime time);

public slots:
    void switchToEditMode();
    void switchToNormalMode();
    //inline void setTime(const QTime & time) {m_normalWidget->setTime(time); m_editWidget->setTime(time);}
    inline void setIs24HourFormat(const bool is24HourFormat) {m_normalWidget->setIs24HourFormat(is24HourFormat);}

private:
    NormalWidget *m_normalWidget;
    EditWidget *m_editWidget;
};

#endif // TIMEWIDGET_H
