#ifndef SHORTCUTKEY_H
#define SHORTCUTKEY_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class KeyLabel;

class ShortcutKey : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutKey(QWidget *parent = nullptr);
    void setTextList(const QStringList &list);

protected:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QHBoxLayout *m_mainLayout;
    QList<KeyLabel*> m_list;
    QLineEdit *m_lineEdit;
};

#endif // SHORTCUTKEY_H
