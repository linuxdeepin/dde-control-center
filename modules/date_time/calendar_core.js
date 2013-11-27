var CONST = {
    days: ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", 
        "Saturday", "Sunday"],
    daysShort: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"],
    daysMin: ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"],
    months: ["January", "February", "March", "April", "May", "June", "July", 
        "August", "September", "October", "November", "December"],
    monthsShort: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", 
        "Sep", "Oct", "Nov", "Dec"],
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
    return d.getFullYear() + "-" + (d.getMonth()+1) + "-" + d.getDate();
}

function getDates(dateStr){
    var results = new Array();
    var d = new Date(dateStr);
    var cur_year = d.getFullYear();
    var cur_month = d.getMonth() + 1;
    var feb_dates = isLeapYear(d.getFullYear())?29:28;

    var cur_d = new Date();
    var cur_date_str = cur_d.getFullYear() + "-" + (cur_d.getMonth() + 1) + "-" 
        + cur_d.getDate();

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


    var i = 1;
    while (i <= dates_arr[1] ){
        var dateValue = cur_year + "-" + cur_month + "-" + i;
        results.push({"dayText": String(i), "isGrey": false, 
            "dateValue": dateValue,
            "isCurrentDate": cur_date_str == dateValue});
        i += 1;
    }

    var pre_year = cur_month == 1 ? cur_year-1 : cur_year;
    var pre_month = cur_month == 1 ? 12 : d.getMonth();
    if (d.getDate() > 7){
        var tmpInterval = (d.getDate() - d.getDay()) % 7
        if (tmpInterval == 0){
            var dateValue = pre_year + "-" + pre_month + "-" + dates_arr[0];
            results.splice(0, 0, {"dayText": String(firstDay), "isGrey": true, 
                "dateValue": dateValue,
                "isCurrentDate": cur_date_str == dateValue});
        }
        else if (tmpInterval > 1){
            var firstDay = dates_arr[0] + tmpInterval - 7;
            for (var i=0; i <= 7 - tmpInterval; i++) {
                var text = dates_arr[0] - i;
                var dateValue = pre_year + "-" + pre_month + "-" + text;
                results.splice(0, 0, {"dayText": String(text), "isGrey": true, 
                    "dateValue": dateValue,
                    "isCurrentDate": cur_date_str == dateValue});
            }
        }
    }
    else{
        var tmpInterval = d.getDate() - d.getDay();
        if (tmpInterval > 1){
            for (var i=0; i <= 7 - tmpInterval; i++) {
                var text = dates_arr[0] - i;
                var dateValue = pre_year + "-" + pre_month + "-" + text;
                results.splice(0, 0, {"dayText": String(text), "isGrey": true, 
                    "dateValue": dateValue,
                    "isCurrentDate": cur_date_str == dateValue});
            }
        }
        else if (tmpInterval < 1){
            for (var i=0; i <= (-1 * tmpInterval); i++) {
                var text = dates_arr[0] - i;
                var dateValue = pre_year + "-" + pre_month + "-" + text;
                results.splice(0, 0, {"dayText": String(text), "isGrey": true, 
                    "dateValue": dateValue,
                    "isCurrentDate": cur_date_str == dateValue});
            }
        }
    }

    var tmpInterval = results.length % 7 == 0 ? 0 : 7 - (results.length % 7);
    var next_year = cur_month == 12 ? cur_year + 1: cur_year;
    var next_month = cur_month == 12 ? 1 : cur_month + 1;
    for (var i=1;i<= tmpInterval; i++){
        var dateValue = next_year + "-" + next_month + "-" + i;
        results.push({"dayText": String(i), "isGrey": true,
            "dateValue": dateValue,
            "isCurrentDate": cur_date_str == dateValue});
    }

    return results;
}
