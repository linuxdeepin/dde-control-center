#ifndef GENERICLISTITEM_H
#define GENERICLISTITEM_H

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>

#include "searchlist.h"

class ImageNameButton;
class GenericListItem : public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString imageNormal READ imageNormal WRITE setImageNormal)
    Q_PROPERTY(QString imageHover READ imageHover WRITE setImageHover)
    Q_PROPERTY(QString imagePress READ imagePress WRITE setImagePress)
    Q_PROPERTY(QString imageChecked READ imageChecked WRITE setImageChecked)
    Q_PROPERTY(QStringList keyWords READ keyWords WRITE setKeyWords)
    Q_PROPERTY(QString checkedIcon READ checkedIcon WRITE setCheckedIcon NOTIFY checkedIconChanged)
    Q_PROPERTY(QString loadingIcon READ loadingIcon WRITE setLoadingIcon NOTIFY loadingIconChanged)
    Q_PROPERTY(bool showBgColor READ showBgColor WRITE setShowBgColor NOTIFY showBgColorChanged)
    Q_PROPERTY(bool showCheckedIcon READ showCheckedIcon WRITE setShowCheckedIcon NOTIFY showCheckedIconChanged)
    Q_PROPERTY(bool showClearButton READ showClearButton WRITE setShowClearButton NOTIFY showClearButtonChanged)
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit GenericListItem(bool showRmButton = false, QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;
    void setListWidget(SearchList *list) Q_DECL_OVERRIDE;

    void addWidgetToRight(QWidget *widget);
    QBoxLayout *rightLayout() const;

    bool checked() const;
    QString title() const;
    QString imagePress() const;
    QString imageHover() const;
    QString imageNormal() const;
    QString imageChecked() const;
    QString checkedIcon() const;
    QString loadingIcon() const;
    bool showBgColor() const;
    bool loading() const;
    bool showClearButton() const;
    bool showCheckedIcon() const;

public slots:
    void setChecked(bool checked);
    void setTitle(QString title);
    void setImagePress(QString imagePress);
    void setImageHover(QString imageHover);
    void setImageNormal(QString imageNormal);
    void setImageChecked(QString imageChecked);
    void setKeyWords(QStringList keyWords);
    void setCheckedIcon(QString checkedIcon);
    void setLoadingIcon(QString loadingIcon);
    void setShowBgColor(bool showBgColor);
    void setLoading(bool loading);
    void setShowClearButton(bool showClearButton);
    void setShowCheckedIcon(bool showCheckedIcon);


signals:
    void clicked();
    void mouseLeave();
    void mouseEnter();
    void checkedChanged(bool checked);
    void removeButtonClicked();
    void showRemoveButton();
    void hideRemoveButton();
    void checkedIconChanged(QString checkedIcon);
    void loadingIconChanged(QString loadingIcon);
    void showBgColorChanged(bool showBgColor);
    void loadingChanged(bool loading);
    void showClearButtonChanged(bool showClearButton);
    void showCheckedIconChanged(bool showCheckedIcon);
    void clearButtonClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

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
    QString m_leftIconSource;
    ImageNameButton *m_deleteButton;
    bool m_showBgColor;
    bool m_loading;
    bool m_showClearButton;
    bool m_showCheckedIcon;
    QString m_loadingIcon;
};

#endif // GENERICLISTITEM_H
