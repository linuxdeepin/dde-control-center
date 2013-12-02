import QtQuick 2.1

Item {
    property alias timezoneList: timezone_items
    property var currentTimezoneItem

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
            }
        }
        currentTimezone.text = currentTimezoneItem.textD
    }
}
