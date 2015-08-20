#ifndef KEYBOARDLAYOUTITEM_H
#define KEYBOARDLAYOUTITEM_H

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>

#include "searchlist.h"

class ImageNameButton;
class KeyboardLayoutItem : public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString imageNormal READ imageNormal WRITE setImageNormal)
    Q_PROPERTY(QString imageHover READ imageHover WRITE setImageHover)
    Q_PROPERTY(QString imagePress READ imagePress WRITE setImagePress)
    Q_PROPERTY(QString imageChecked READ imageChecked WRITE setImageChecked)
    Q_PROPERTY(QStringList keyWords READ keyWords WRITE setKeyWords)

public:
    explicit KeyboardLayoutItem(bool showRmButton = false, QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;
    bool checked() const;
    QString title() const;
    QString imagePress() const;
    QString imageHover() const;
    QString imageNormal() const;
    QString imageChecked() const;

public slots:
    void setChecked(bool checked);
    void setTitle(QString title);
    void setImagePress(QString imagePress);
    void setImageHover(QString imageHover);
    void setImageNormal(QString imageNormal);
    void setImageChecked(QString imageChecked);
    void setKeyWords(QStringList keyWords);

signals:
    void checkedChanged(bool checked);
    void removeButtonClicked();
    void showRemoveButton();
    void hideRemoveButton();

protected:
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    QWidget *m_widget;
    QLabel *m_label;
    bool m_checked;
    QHBoxLayout *m_layout;
    QString m_imagePress;
    QString m_imageHover;
    QString m_imageNormal;
    QStringList m_keyWords;
    QString m_imageChecked;
    ImageNameButton *m_deleteButton;
};

#endif // KEYBOARDLAYOUTITEM_H
