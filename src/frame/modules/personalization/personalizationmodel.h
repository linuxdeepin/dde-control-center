// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef PERSONALIZATIONMODEL_H
#define PERSONALIZATIONMODEL_H

#include <QObject>
#include <QDebug>

namespace dcc
{
namespace personalization
{
class ThemeModel;
class FontModel;
class FontSizeModel;
class PersonalizationModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    enum ScrollBarPolicy {
        KeepShown = 0,
        ShowOnScrolling = 1
    };

public:
    explicit PersonalizationModel(QObject *parent = nullptr);
    ~PersonalizationModel();
    inline ThemeModel    *getWindowModel()    const {return m_windowModel;}
    inline ThemeModel    *getIconModel()      const {return m_iconModel;}
    inline ThemeModel    *getMouseModel()     const {return m_mouseModel;}
    inline FontModel     *getStandFontModel() const {return m_standFontModel;}
    inline FontModel     *getMonoFontModel()  const {return m_monoFontModel;}
    inline FontSizeModel *getFontSizeModel()  const {return m_fontSizeModel;}
    void setIs3DWm(const bool is3d);
    bool is3DWm() const;

    void setIsMoveWindow(const bool isMoveWindow);
    bool isMoveWindow() const;
    bool getIsMoveWindowDconfig();
    void setIsMoveWindowDconfig(bool value);

    void setWindowRadius(int radius);
    int windowRadius();

    inline std::pair<int, double> opacity() const { return m_opacity; }
    void setOpacity(std::pair<int, double> opacity);

    inline int miniEffect() const { return m_miniEffect; }
    void setMiniEffect(const int &effect);

    inline QString getActiveColor() {return m_activeColor;}
    void setActiveColor(const QString &color);

    inline bool getAllowSwitch() {return m_allowSwitch;}
    void setCompositingAllowSwitch(bool value);

    inline bool getIsEffectSupportScale() {return m_IsEffectSupportedScale;}
    void setIsEffectSupportScale(bool value);

    inline bool getIsEffectSupportMagiclamp() {return m_IsEffectSupportedMagiclamp;}
    void setIsEffectSupportMagiclamp(bool value);

    inline bool getIsEffectSupportMoveWindow() {return m_IsEffectSupportedMoveWindow;}
    void setIsEffectSupportMoveWindow(bool value);

    inline int scrollBarPolicy() const { return m_scrollBarPolicy;}
    void setScrollBarPolicy(int policy);

    inline bool compactDisplay() const { return  m_compactDisplay;}
    void setCompactDisplay(bool enabled);

Q_SIGNALS:
    void wmChanged(const bool is3d);
    void moveWindowChanged(const bool isMoveWindow);
    void onOpacityChanged(std::pair<int, double> opacity);
    void onMiniEffectChanged(int effect);
    void onActiveColorChanged(const QString &color);
    void onCompositingAllowSwitch(bool value);
    void onWindowRadiusChanged(int radius);
    void onSaveWindowRadiusChanged(int radius);
    void onEffectSupportDisableChanged();
    void onScrollBarPolicyChanged(int policy);
    void onCompactDisplayChanged(bool enabled);

private:
    ThemeModel    *m_windowModel;
    ThemeModel    *m_iconModel;
    ThemeModel    *m_mouseModel;
    FontModel     *m_standFontModel;
    FontModel     *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;
    bool m_is3DWm;
    bool m_isMoveWindow;
    std::pair<int, double> m_opacity;
    int m_miniEffect;
    QString m_activeColor;
    bool m_allowSwitch;
    int m_windowRadius;
    bool m_IsEffectSupportedScale;
    bool m_IsEffectSupportedMagiclamp;
    bool m_IsEffectSupportedMoveWindow;
    int m_scrollBarPolicy;
    bool m_compactDisplay;
};
}
}
#endif // PERSONALIZATIONMODEL_H
