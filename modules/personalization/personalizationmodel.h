/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    explicit PersonalizationModel(QObject *parent = 0);
    ~PersonalizationModel();
    inline ThemeModel    *getWindowModel()    const {return m_windowModel;}
    inline ThemeModel    *getIconModel()      const {return m_iconModel;}
    inline ThemeModel    *getMouseModel()     const {return m_mouseModel;}
    inline FontModel     *getStandFontModel() const {return m_standFontModel;}
    inline FontModel     *getMonoFontModel()  const {return m_monoFontModel;}
    inline FontSizeModel *getFontSizeModel()  const {return m_fontSizeModel;}
    void setIs3DWm(const bool is3d);
    bool is3DWm() const;

    inline std::pair<int, double> opacity() const { return m_opacity; }
    void setOpacity(std::pair<int, double> opacity);

Q_SIGNALS:
    void wmChanged(const bool is3d);
    void onOpacityChanged(std::pair<int, double> opacity);

private:
    ThemeModel    *m_windowModel;
    ThemeModel    *m_iconModel;
    ThemeModel    *m_mouseModel;
    FontModel     *m_standFontModel;
    FontModel     *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;
    bool m_is3DWm;
    std::pair<int, double> m_opacity;
};
}
}
#endif // PERSONALIZATIONMODEL_H
