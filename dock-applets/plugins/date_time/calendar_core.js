var CONST = {
    days: ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", 
        "Saturday", "Sunday"],
    daysShort: [dsTr("Sun"), dsTr("Mon"), dsTr("Tue"), dsTr("Wed"), dsTr("Thu"), dsTr("Fri"), dsTr("Sat")],
    daysMin: ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"],
    //months: [dsTr("January"), dsTr("February"), dsTr("March"), dsTr("April"), dsTr("May"), dsTr("June"), dsTr("July"), 
        //dsTr("August"), dsTr("September"), dsTr("October"), dsTr("November"), dsTr("December")],
    monthsShort: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", 
        "Sep", "Oct", "Nov", "Dec"],
    monthsDates: [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31],
}

function isSameMonth(d_obj_1, d_obj_2){
    if(d_obj_1.getFullYear() != d_obj_2.getFullYear()){
        return false
    }
    else{
        if(d_obj_1.getMonth() != d_obj_2.getMonth()){
            return false
        }
        else{
            return true
        }
    }
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
function setDateOne(d){
    var year = d.getFullYear();
    var month = d.getMonth() + 1;
    if(month<10){
        month = "0" + month;
    }
    return new Date(year+"-"+month+"-0"+1)
}

function getDateWidthMonthStep(d, step){
    var new_d = new Date(dateToString(d));
    var new_month = d.getMonth() + step;
    if (new_month > 11) {
        new_month = new_month - 12
    }
    else if (new_month < 0){
        new_month = 12 + new_month
    }
    new_d.setMonth(d.getMonth() + step)
    return new_d
}

function getDates(dateStr){
    var results = new Array();
    var d = new Date(dateStr);
    var cur_year = d.getFullYear();
    var cur_month = d.getMonth() + 1;
    var feb_dates = isLeapYear(d.getFullYear())?29:28;

    var cur_d = new Date();

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
