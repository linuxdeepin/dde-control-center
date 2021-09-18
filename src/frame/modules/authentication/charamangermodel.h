/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef CHARAMANGERMODEL_H
#define CHARAMANGERMODEL_H
#include <QObject>

namespace dcc {
namespace authentication {

#define FACE_CHARA 4
#define IRIS_CHARA 64

class CharaMangerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool faceDriverVaild READ faceDriverVaild WRITE setFaceDriverVaild NOTIFY vaildFaceDriverChanged)
    Q_PROPERTY(QStringList facesList READ facesList WRITE setFacesList NOTIFY facesListChanged)

public:
    /**
     * @brief The EnrollStatusType enum 人脸录入状态
     */
    enum EnrollStatusType {
        STATUS_SUCCESS = 0,   // 成功
        STATUS_NOT_REAL_HUMAN,  // 非人类
        STATUS_FACE_NOT_CENTER,  // 非中心
        STATUS_FACE_TOO_BIG,  // 脸太大
        STATUS_FACE_TOO_SMALL,
        STATUS_NO_FACE,
        STATUS_FACE_TOO_MANY,  // 多人
        STATUS_FACE_NOT_CLEARITY,  // 不清晰
        STATUS_FACE_BRIGHTNESS,  // 亮度
        STATUS_FACE_COVERD,  // 遮挡
        STATUS_CANCELED, // 取消
        STATUS_OVERTIME, // 超时
        STATUS_COLLAPSE // 崩溃
    };

    /**
     * @brief The AddInfoState enum 录入人脸的三种状态
     */
    enum AddInfoState {
        StartState,
        Success,
        Fail,
    };

public:
    explicit CharaMangerModel(QObject *parent = nullptr);
    inline QList<QString> getPredefineFaceName() const { return m_predefineFaceNames; }

    inline bool faceDriverVaild() const {  return m_isFaceDriverVaild; }
    void setFaceDriverVaild(bool isVaild);

    inline QString faceDriverName() const { return  m_faceDriverName; }
    void setFaceDriverName(const QString &driverName);

    inline int faceCharaType() const { return  FACE_CHARA; }
    inline int irisCharaType() const { return  IRIS_CHARA; }

    inline QStringList facesList() const { return  m_facesList; }
    void setFacesList(const QStringList &faces);


    /**
     * @brief onEnrollStatusChanged 录入状态信号，在录入过程中通过此信号提示当前录入状态
     * @param code
     * @param msg
     */
    void onEnrollStatusChanged(int code, const QString& msg);

Q_SIGNALS:
    void vaildFaceDriverChanged(const bool isVaild);
    void facesListChanged(const QStringList &faces);

    /**
     * @brief enrollInfoState 注册录入状态  用于区分页面显示状态  注：仅处理成功录入失败状态
     * @param state 录入的状态
     * @param tips 对应的提示语
     */
    void enrollInfoState(AddInfoState state, const QString &tips);
    void enrollStatusTips(QString title);

private:
    QString m_faceDriverName;
    bool m_isFaceDriverVaild;
    QStringList m_facesList;
    QList<QString> m_predefineFaceNames;
};

}
}
#endif // CHARAMANGERMODEL_H
