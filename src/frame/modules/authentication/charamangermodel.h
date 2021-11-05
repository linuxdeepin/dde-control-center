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
    enum EnrollFaceStatusType {
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
     * @brief The EnrollIrisStatusType enum
     */
    enum EnrollIrisStatusType {
        STATUS_IRIS_SUCCESS = 0,   // 成功
        STATUS_IRIS_TOO_BIG,  // 太大
        STATUS_IRIS_TOO_SMALL,
        STATUS_IRIS_NO_FACE,
        STATUS_IRIS_NOT_CLEARITY,  // 不清晰
        STATUS_IRIS_BRIGHTNESS,  // 亮度
        STATUS_IRIS_EYES_CLOSE,  // 闭目
        STATUS_IRIS_CANCELED, // 取消
        STATUS_IRIS_Error, // 崩溃
        STATUS_IRIS_OVERTIME // 超时
    };

    /**
     * @brief The AddInfoState enum 录入的四种状态
     */
    enum AddInfoState {
        StartState,
        Success,
        Fail,
        Processing,
    };

public:
    explicit CharaMangerModel(QObject *parent = nullptr);

    inline int faceCharaType() const { return  FACE_CHARA; }
    inline int irisCharaType() const { return  IRIS_CHARA; }

    inline bool faceDriverVaild() const {  return m_isFaceDriverVaild; }
    void setFaceDriverVaild(bool isVaild);

    inline QString faceDriverName() const { return  m_faceDriverName; }
    void setFaceDriverName(const QString &driverName);

    inline QStringList facesList() const { return  m_facesList; }
    void setFacesList(const QStringList &faces);

    inline bool irisDriverVaild() const {  return m_isIrisDriverVaild; }
    void setIrisDriverVaild(bool isVaild);

    inline QString irisDriverName() const { return  m_irisDriverName; }
    void setIrisDriverName(const QString &driverName);

    inline QStringList irisList() const { return  m_irisList; }
    void setIrisList(const QStringList &iris);

    /**
     * @brief onEnrollStatusChanged 录入状态信号，在录入过程中通过此信号提示当前录入状态
     * @param code
     * @param msg
     */
    void onEnrollStatusChanged(int code, const QString& msg);
    void onEnrollIrisStatusChanged(int code, const QString& msg);

Q_SIGNALS:
    void vaildFaceDriverChanged(const bool isVaild);
    void vaildIrisDriverChanged(const bool isVaild);

    void facesListChanged(const QStringList &faces);
    void irisListChanged(const QStringList &iris);

    /**
     * @brief enrollInfoState 注册录入状态  用于区分页面显示状态  注：仅处理成功录入失败状态
     * @param state 录入的状态
     * @param tips 对应的提示语
     */
    void enrollInfoState(AddInfoState state, const QString &tips);
    void enrollStatusTips(QString title);

    /**
     * @brief enrollIrisInfoState 注册虹膜录入状态
     * @param state 录入状态
     * @param tips 提示信息
     */
    void enrollIrisInfoState(AddInfoState state, const QString &tips);
    void enrollIrisStatusTips(QString title);

private:
    // 人脸
    QString m_faceDriverName;
    bool m_isFaceDriverVaild;
    QStringList m_facesList;

    // 虹膜
    QString m_irisDriverName;
    bool m_isIrisDriverVaild;
    QStringList m_irisList;
};

}
}
#endif // CHARAMANGERMODEL_H
