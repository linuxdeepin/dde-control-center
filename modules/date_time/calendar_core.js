var CONST = {
    monthsDates: [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31],
}

function isLeapYear(year){
    if (year % 100 == 0){
        if (year % 400 == 0){
            return true
        }
        else{
            return false
        }
    }
    else{
        if (year % 4 == 0){
            return true
        }
        else{
            return false
        }
    }
}

function dateToString(d){
    var month_str = String(d.getMonth()+1);
    if(d.getMonth()+1<10){
        month_str = "0" + month_str;
    }
    var day_str = String(d.getDate());
    if(d.getDate()<10){
        day_str = "0" + day_str;
    }
    return d.getFullYear() + "-" + month_str + "-" + day_str;
}

function getNextMonthDateValue(dateValue){
    var dateValueArray = dateValue.split("-");
    dateValueArray[2] = "01";
    if(dateValueArray[1] == "12"){
        dateValueArray[1] = "01";
        var tmpYear = Math.round(dateValueArray[0]);
        tmpYear += 1;
        dateValueArray[0] = String(tmpYear);
    }
    else{
        var tmpMonth = Math.round(dateValueArray[1]);
        tmpMonth += 1;
        if(tmpMonth < 10){
            dateValueArray[1] = "0" + String(tmpMonth);
        }
        else{
            dateValueArray[1] = String(tmpMonth);
        }
    }
    return dateValueArray.join("-");
}
function getPreviousMonthDateValue(dateValue){
    var dateValueArray = dateValue.split("-");
    dateValueArray[2] = "01";
    if(dateValueArray[1] == "01"){
        dateValueArray[1] = "12";
        var tmpYear = Math.round(dateValueArray[0]);
        tmpYear -= 1;
        dateValueArray[0] = String(tmpYear);
    }
    else{
        var tmpMonth = Math.round(dateValueArray[1]);
        tmpMonth -= 1;
        if(tmpMonth < 10){
            dateValueArray[1] = "0" + String(tmpMonth);
        }
        else{
            dateValueArray[1] = String(tmpMonth);
        }
    }
    return dateValueArray.join("-");
}

function getYearMonth(dateValue){
    var a = dateValue.split("-");
    return a[0] + "-" + a[1];
}

function compareYearMonth(yearMonthA, yearMonthB){
    if (yearMonthA == yearMonthB){
        return 0;
    }

    var a = yearMonthA.split("-");
    var b = yearMonthB.split("-");
    if(Math.round(a[0]) > Math.round(b[0])){
        return 1;
    }
    else if(Math.round(a[0]) < Math.round(b[0])){
        return -1;
    }
    else{
        if(Math.round(a[1]) > Math.round(b[1])){
            return 1;
        }
        else if(Math.round(a[1]) < Math.round(b[1])){
            return -1;
        }
        else{
            return 0;
        }
    }
}

function getDates(dateStr){
    var results = new Array();
    var d = new Date(dateStr.split("-").join(" "));
    var cur_year = d.getFullYear();
    var cur_month = d.getMonth() + 1;
    var feb_dates = isLeapYear(d.getFullYear())?29:28;

    if (cur_month == 1){
        var dates_arr = [31, 31, feb_dates];
    }
    else if (cur_month == 2) {
        var dates_arr = [31, feb_dates, 31];
    }
    else if (cur_month == 3) {
        var dates_arr = [feb_dates, 31, 30];
    }
    else if (cur_month == 12){
        var dates_arr = [30, 31, 31];
    }
    else {
        var dates_arr = CONST.monthsDates.slice(cur_month-2, cur_month+1);
    }


    var cur_month_str = String(cur_month);
    if(cur_month<10){
        cur_month_str = "0" + cur_month_str;
    }
    for(var i=1; i <= dates_arr[1]; i++){
        var i_str = String(i);
        if(i<10){
            i_str = "0" + i_str;
        }
        var dateValue = cur_year + "-" + cur_month_str + "-" + i_str;
        results.push({
            "dayText": String(i), "isGrey": false,
            "dateValue": dateValue
        });
    }

    var pre_year = cur_month == 1 ? cur_year-1 : cur_year;
    var pre_month = cur_month == 1 ? 12 : d.getMonth();
    var pre_month_str = String(pre_month);
    if(pre_month<10){
        pre_month_str = "0" + pre_month_str;
    }
    if (d.getDate() > 7){
        var tmpInterval = (d.getDate() - d.getDay()) % 7
        if (tmpInterval == 0){
            var day = dates_arr[0];
            var i_str = String(day);
            if (day<10){
                i_str = "0" + i_str;
            }
            var dateValue = pre_year + "-" + pre_month_str + "-" + i_str;
            results.splice(0, 0, {
                "dayText": String(day), "isGrey": true,
                "dateValue": dateValue
            });
        }
        else if (tmpInterval > 1){
            var firstDay = dates_arr[0] + tmpInterval - 7;
            for (var i=0; i <= 7 - tmpInterval; i++) {
                var day = dates_arr[0] - i;
                var i_str = String(day);
                if (day<10){
                    i_str = "0" + i_str;
                }
                var dateValue = pre_year + "-" + pre_month_str + "-" + i_str;
                results.splice(0, 0, {
                    "dayText": String(day), "isGrey": true,
                    "dateValue": dateValue
                });
            }
        }
    }
    else{
        var tmpInterval = d.getDate() - d.getDay();
        if (tmpInterval > 1){
            for (var i=0; i <= 7 - tmpInterval; i++) {
                var day = dates_arr[0] - i;
                var i_str = String(day);
                if (day<10){
                    i_str = "0" + i_str;
                }
                var dateValue = pre_year + "-" + pre_month_str + "-" + i_str;
                results.splice(0, 0, {
                    "dayText": String(day), "isGrey": true,
                    "dateValue": dateValue
                });
            }
        }
        else if (tmpInterval < 1){
            for (var i=0; i <= (-1 * tmpInterval); i++) {
                var day = dates_arr[0] - i;
                var i_str = String(day);
                if (day<10){
                    i_str = "0" + i_str;
                }
                var dateValue = pre_year + "-" + pre_month_str + "-" + i_str;
                results.splice(0, 0, {
                    "dayText": String(day), "isGrey": true,
                    "dateValue": dateValue
                });
            }
        }
    }

    //var tmpInterval = results.length % 7 == 0 ? 0 : 7 - (results.length % 7);
    var tmpInterval = 42 - results.length
    var next_year = cur_month == 12 ? cur_year + 1: cur_year;
    var next_month = cur_month == 12 ? 1 : cur_month + 1;
    var next_month_str = String(next_month);
    if (next_month<10){
        next_month_str = "0" + next_month_str;
    }
    for (var i=1;i<= tmpInterval; i++){
        var i_str = String(i);
        if (i<10){
            i_str = "0" + i_str;
        }
        var dateValue = next_year + "-" + next_month_str + "-" + i_str;
        results.push({
            "dayText": String(i), "isGrey": true,
            "dateValue": dateValue
        });
    }

    return results;
}
