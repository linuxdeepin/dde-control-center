//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSONALIZATIONMODEL_H
#define PERSONALIZATIONMODEL_H

#include <QObject>
#include <QDebug>

class ThemeModel;
class FontModel;
class FontSizeModel;
class PersonalizationModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;

public:
    explicit PersonalizationModel(QObject *parent = nullptr);
    ~PersonalizationModel();
    inline ThemeModel *getWindowModel() const { return m_windowModel; }
    inline ThemeModel *getIconModel() const { return m_iconModel; }
    inline ThemeModel *getMouseModel() const { return m_mouseModel; }
    inline ThemeModel *getGlobalThemeModel() const { return m_globalThemeModel; }
    inline FontModel *getStandFontModel() const { return m_standFontModel; }
    inline FontModel *getMonoFontModel() const { return m_monoFontModel; }
    inline FontSizeModel *getFontSizeModel() const { return m_fontSizeModel; }
    void setIs3DWm(const bool is3d);
    bool is3DWm() const;

    void setWindowRadius(int radius);
    int windowRadius();

    inline std::pair<int, double> opacity() const { return m_opacity; }
    void setOpacity(std::pair<int, double> opacity);

    inline int miniEffect() const { return m_miniEffect; }
    void setMiniEffect(const int &effect);

    inline QString getActiveColor() { return m_activeColor; }
    void setActiveColor(const QString &color);

    inline bool getAllowSwitch() { return m_allowSwitch; }
    void setCompositingAllowSwitch(bool value);

    inline bool getCompactDisplay() { return m_compactDisplay; }
    void setCompactDisplay(bool value);

Q_SIGNALS:
    void wmChanged(const bool is3d);
    void onOpacityChanged(std::pair<int, double> opacity);
    void onMiniEffectChanged(int effect);
    void onActiveColorChanged(const QString &color);
    void onCompositingAllowSwitch(bool value);
    void onWindowRadiusChanged(int radius);
    void onSaveWindowRadiusChanged(int radius);
    void compactDisplayChanged(bool value);

private:
    ThemeModel *m_windowModel;
    ThemeModel *m_iconModel;
    ThemeModel *m_mouseModel;
    ThemeModel *m_globalThemeModel;
    FontModel *m_standFontModel;
    FontModel *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;
    bool m_is3DWm;
    std::pair<int, double> m_opacity;
    int m_miniEffect;
    QString m_activeColor;
    bool m_allowSwitch;
    int m_windowRadius;
    bool m_compactDisplay;
};
#endif // PERSONALIZATIONMODEL_H
