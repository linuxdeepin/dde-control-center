import QtQuick 2.1

Item {
    property alias timezoneList: timezone_items
    property var currentTimezoneItem
    property var timezoneIndexInfo: [
            [-11, "Pacific/Pago_Pago"],
            [-10, "Pacific/Tahiti"],
            [-9, "Pacific/Gambier"],
            [-8, "America/Los_Angeles"],
            [-7, "America/Edmonton"],
            [-6, "America/Rainy_River"],
            [-5, "America/Montreal"],
            [-4, "America/Goose_Bay"],
            [-3, "America/Godthab"],
            [-2, "America/Noronha"],
            [-1, "America/Scoresbysund"],
            [0, "Africa/Bamako"],
            [1, "Africa/Ndjamena"],
            [2, "Africa/Lusaka"],
            [3, "Africa/Addis_Ababa"],
            [4, "Asia/Dubai"],
            [5, "Indian/Maldives"],
            [6, "Asia/Almaty"],
            [7, "Asia/Phnom_Penh"],
            [8, "Asia/Shanghai"],
            [9, "Asia/Tokyo"],
            [10, "Asia/Yakutsk"],
            [11, "Pacific/Guadalcanal"],
            [12, "Asia/Kamchatka"]]

    function getTimezoneByOffset(gmtoff){
        for(var i=0;i<timezoneIndexInfo.length;i++){
            if(gmtoff == timezoneIndexInfo[i][0]){
                return timezoneIndexInfo[i][1]
            }
        }
        return "Asia/Shanghai"
    }

    ListModel { id: timezone_items }

    Component.onCompleted: {
        timezone_items.append({"textD": dsTr("(UTC-11:00)Samoa"), "value": -11})
        timezone_items.append({"textD": dsTr("(UTC-10:00)Hawaii"), "value": -10})
        timezone_items.append({"textD": dsTr("(UTC-09:00)Alaska"), "value": -9})
        timezone_items.append({"textD": dsTr("(UTC-08:00)Lower California"), "value": -8})
        timezone_items.append({"textD": dsTr("(UTC-07:00)Arizona, Llamas, Mazatlan, Chihuahua"), "value": -7})
        timezone_items.append({"textD": dsTr("(UTC-06:00)Saskatchewan, Mexico City, Monterrey"), "value": -6})
        timezone_items.append({"textD": dsTr("(UTC-05:00)Indiana, Bogota, Lima, Quito"), "value": -5})
        timezone_items.append({"textD": dsTr("(UTC-04:00)San Diego, Georgetown, San Juan"), "value": -4})
        timezone_items.append({"textD": dsTr("(UTC-03:00)Greenland, Brasilia, Fortaleza"), "value": -3})
        timezone_items.append({"textD": dsTr("(UTC-02:00)Mid-Atlantic"), "value": -2})
        timezone_items.append({"textD": dsTr("(UTC-01:00)Cape Verde Islands, Azores"), "value": -1})
        timezone_items.append({"textD": dsTr("(UTC)London, Dublin, Edinburgh, Lisbon, Casablanca"), "value": 0})
        timezone_items.append({"textD": dsTr("(UTC+01:00)Paris, Amsterdam, Berlin, Rome, Vienna"), "value": 1})
        timezone_items.append({"textD": dsTr("(UTC+02:00)Cairo, Athens, Istanbul, Jerusalem"), "value": 2})
        timezone_items.append({"textD": dsTr("(UTC+03:00)Moscow, St. Petersburg, Baghdad"), "value": 3})
        timezone_items.append({"textD": dsTr("(UTC+04:00)Port Louis, Abu Dhabi, Muscat, Yerevan"), "value": 4})
        timezone_items.append({"textD": dsTr("(UTC+05:00)Islamabad, Karachi, Tashkent"), "value": 5})
        timezone_items.append({"textD": dsTr("(UTC+06:00)Dhaka, Novosibirsk"), "value": 6})
        timezone_items.append({"textD": dsTr("(UTC+07:00)Bangkok, Hanoi, Jakarta"), "value": 7})
        timezone_items.append({"textD": dsTr("(UTC+08:00)Beijing, Chongqing, HongKong, Taipei, Urumqi"), "value": 8})
        timezone_items.append({"textD": dsTr("(UTC+09:00)Osaka, Sapporo, Tokyo, Seoul"), "value": 9})
        timezone_items.append({"textD": dsTr("(UTC+10:00)Guam, Canberra, Melbourne, Sydney"), "value": 10})
        timezone_items.append({"textD": dsTr("(UTC+11:00)Magadan, Solomon Islands"), "value": 11})
        timezone_items.append({"textD": dsTr("(UTC+12:00)New Zealand, Kiribati"), "value": 12})

        var curOffset = -1 * date.getTimezoneOffset()/60

        for(var i=0;i<timezone_items.count;i++){
            if (timezone_items.get(i).value == curOffset){
                currentTimezoneItem = timezone_items.get(i)
                timezoneExpand.currentTimezoneLabel = currentTimezoneItem.textD
                //timezoneExpand.timezoneListview.positionViewAtIndex(i, ListView.Center)
            }
        }
    }
}
