/***
 ***/

var CONST = {
    days: ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"],
    daysShort: ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"],
    daysMin: ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"],
    months: ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"],
    monthsShort: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"],
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

function getDates(dateStr){
    var results = new Array()
    var d = new Date(dateStr);
    var month = d.getMonth();
    var feb_dates = isLeapYear(d.getFullYear())?29:28;

    if (month == 1){
        var dates_arr = [31, 31, feb_dates];
    }
    else if (month == 2) { 
        var dates_arr = [31, feb_dates, 31];
    }
    else if (month == 3) {
        var dates_arr = [feb_dates, 31, 30];
    }
    else if (month == 12){
        var dates_arr = [30, 31, 31];
    }
    else {
        var dates_arr = CONST.monthsDates.slice(month-1, month+2);
    }

    var cur_date_pre = d.getFullYear() + "-";

    var i = 1;
    while (i <= dates_arr[1] ){
        results.push({"dayText": String(i), "isGrey": false, "curDate": cur_date_pre + d.getMonth() + "-" + i});
        i += 1;
    }

    var tmpInterval = (d.getDate() - d.getDay()) % 7
    if (tmpInterval == 0){
        var firstDay = dates_arr[0];
        results.splice(0, 0, {"dayText": String(firstDay), "isGrey": true, "curDate": cur_date_pre + 1 + "-" + firstDay});
    }
    else if (tmpInterval == 1){
        var firstDay = 1;
    }
    else if (tmpInterval > 1){
        var firstDay = dates_arr[0] + tmpInterval - 7;
        for (var i=0; i <= 7 - tmpInterval; i++) {
            var text = dates_arr[0] - i
            results.splice(0, 0, {"dayText": String(text), "isGrey": true, "curDate": cur_date_pre + 1 + "-" + text});
        }
    }

    return results;
}
