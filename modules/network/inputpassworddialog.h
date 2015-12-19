#ifndef INPUTPASSWORDDIALOG_H
#define INPUTPASSWORDDIALOG_H

#include <QWidget>
#include <QPointer>

#include <libdui/dboxwidget.h>
#include <libdui/dpasswordedit.h>
#include <libdui/dcheckbox.h>
#include <libdui/dseparatorhorizontal.h>

DUI_USE_NAMESPACE

class QEventLoop;
class InputPasswordDialog : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool autoConnect READ autoConnect)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit InputPasswordDialog(QWidget *parent = 0);
    ~InputPasswordDialog();

    bool autoConnect() const;
    QString text() const;
    void setInputAlert(bool alert);

public slots:
    int exec();
    void done(int code);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

signals:
    void setText(const QString &text);

    void cancel();
    void confirm();
    void textChanged(QString text);

private slots:
    void updateMainWidgetPos();

private:
    DPasswordEdit *m_edit;
    DCheckBox *m_checkBox;
    DVBoxWidget *m_mainWidget;
    QPointer<QEventLoop> m_eventLoop;
};

#endif // INPUTPASSWORDDIALOG_H
