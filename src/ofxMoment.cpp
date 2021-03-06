//
//  ofxMoment.cpp
//  ofxMoment
//
//  Created by  Tyler Parker on 4/19/15.
//
//

#include "ofxMoment.h"

ofxMoment::ofxMoment(){
    valid = true;
    Poco::DateTime cdt;
    currDT = Poco::LocalDateTime(currDT.tzd(), cdt);
}

ofxMoment::ofxMoment(string timeStr){
    int tzd;
    Poco::DateTime cdt;
    valid = Poco::DateTimeParser::tryParse(timeStr, cdt, tzd);
    currDT = Poco::LocalDateTime(tzd, cdt);
}


ofxMoment::~ofxMoment(){};

string ofxMoment::format(){
    string timeFormat = "%Y-%m-%d-%H-%M-%S-%i";
    return format(timeFormat);
}

string ofxMoment::format(string timeFormat){
    return Poco::DateTimeFormatter::format(currDT.timestamp(), timeFormat);
}

bool ofxMoment::isValid(){
    return valid;
}

int ofxMoment::seconds(){
    return currDT.second();
}

void ofxMoment::seconds(int sec){
    DT dt = getDT(currDT);
    dt.sec = sec;
    if(!validDT(dt)){
        // Bubble up to minutes
        add(floor(sec/60), "minutes");
        seconds(sec%60);
    }
    else{
        assignDT(currDT, dt);
    }
}

int ofxMoment::minutes(){
    return currDT.minute();
}

void ofxMoment::minutes(int min){
    DT dt = getDT(currDT);
    dt.min = min;
    if(!validDT(dt)){
        // Bubble up to hours
        add(floor(min/60),"hours");
        minutes(min%60);
    }
    else{
        assignDT(currDT, dt);
    }
}

int ofxMoment::hours(){
    return currDT.hour();
}

void ofxMoment::hours(int hour){
    DT dt = getDT(currDT);
    dt.hour = hour;
    if(!validDT(dt)){
        // Bubble up to days
        add(floor(hour/24),"days");
        hours(hour%24);
    }
    else{
        assignDT(currDT, dt);
    }
}


int ofxMoment::day(){
    return currDT.day();
}

void ofxMoment::day(int dayNum){
    DT dt = getDT(currDT);
    dt.day = dayNum;
    if(!validDT(dt)){
        dt.day = (dt.day - Poco::DateTime::daysOfMonth(dt.year, dt.month));
        add(1, "months");
        day(dt.day);
    }
    else{
        assignDT(currDT, dt);
    }
}

int ofxMoment::month(){
    return currDT.month();
}

void ofxMoment::month(int month){
    DT dt = getDT(currDT);
    dt.month = month;
    if(dt.month > 12){
        // Bubble up to years
        dt.year += floor(month/12);
        dt.month = dt.month%12;
    }
    if(dt.day > Poco::DateTime::daysOfMonth(dt.year, dt.month)){
        dt.day = Poco::DateTime::daysOfMonth(dt.year, dt.month);
    }
    assignDT(currDT, dt);
}


int ofxMoment::years(){
    return currDT.year();
}

void ofxMoment::years(int year){
    DT dt = getDT(currDT);
    dt.year = year;
    assignDT(currDT, dt);
}


void ofxMoment::add(int num, string timeUnit){
    add(currDT, num, timeUnit);
}

void ofxMoment::add(Poco::LocalDateTime& datettime, int num, string timeUnit){
    Poco::Timespan ts;
    if(timeUnit == "years" || timeUnit == "y"){
        DT dt = getDT(datettime);
        dt.year += num;
        assignDT(datettime, dt);
        return;
    }
    else if(timeUnit == "months" || timeUnit == "M"){
        int m = month()+num;
        if(m <= 0){
            int yDiff = 0;
            DT dt = getDT(datettime);
            dt.year += floor(m/12)-1;
            dt.month = ((11-m)%12)+1;
            assignDT(datettime, dt);
        }
        else{
            month(m);
        }
        
        return;
    }
    else if(timeUnit == "weeks" || timeUnit == "w"){
        ts = Poco::Timespan(num*7*Poco::Timespan::DAYS);
    }
    else if(timeUnit == "days" || timeUnit == "d"){
        ts = Poco::Timespan(num*Poco::Timespan::DAYS);
    }
    else if(timeUnit == "hours" || timeUnit == "h"){
        ts = Poco::Timespan(num*Poco::Timespan::HOURS);
    }
    else if(timeUnit == "minutes" || timeUnit == "m"){
        ts = Poco::Timespan(num*Poco::Timespan::MINUTES);
    }
    else if(timeUnit == "seconds" || timeUnit == "s"){
        ts = Poco::Timespan(num*Poco::Timespan::SECONDS);
    }
    else if(timeUnit == "milliseconds" || timeUnit == "ms"){
        ts = Poco::Timespan(num*Poco::Timespan::MILLISECONDS);
    }
    else{
        ofLogError("ofxMoment","Invalid time unit '"+timeUnit+"'");
        return;
    }
    
    datettime += ts;
}

Poco::Timespan ofxMoment::diff(Poco::LocalDateTime &datetime){
    return currDT-datetime;
}

int ofxMoment::diff(ofxMoment& moment, string timeUnit){
    return diff(*this, moment, timeUnit);
}

int ofxMoment::diff(ofxMoment& a, ofxMoment& b, string timeUnit){
    Poco::Timespan ts = a.currDT.timestamp()-b.currDT.timestamp();
    
    if(timeUnit == "years" || timeUnit == "y"){
        return floor(diff(a,b,"months")/12);
    }
    if(timeUnit == "months" || timeUnit == "M"){
        // TODO : actual months
        int months = ((a.years() - b.years()) * 12) + (a.month() - b.month());
        return months;
    }
    else if(timeUnit == "weeks" || timeUnit == "w"){
        return floor(diff(a,b,"days")/7);
    }
    else if(timeUnit == "days" || timeUnit == "d"){
        return floor(ts.totalHours()/24);
    }
    else if(timeUnit == "hours" || timeUnit == "h"){
        return ts.totalHours();
    }
    else if(timeUnit == "minutes" || timeUnit == "m"){
        return ts.totalMinutes();
    }
    else if(timeUnit == "seconds" || timeUnit == "s"){
        return ts.totalSeconds();
    }
    else if(timeUnit == "milliseconds" || timeUnit == "ms"){
        return ts.totalMilliseconds();
    }
    else{
        ofLogError("ofxMoment","Invalid time unit '"+timeUnit+"'");
        return 0;
    }
}

void ofxMoment::subtract(int num, string timeUnit){
    add(-1*num,timeUnit);
}

DT ofxMoment::getDT(){
    getDT(currDT);
}

DT ofxMoment::getDT(Poco::LocalDateTime datetime){
    DT dt;
    dt.year = datetime.year();
    dt.month = datetime.month();
    dt.day = datetime.day();
    dt.hour = datetime.hour();
    dt.min = datetime.minute();
    dt.sec = datetime.second();
    dt.ms = datetime.millisecond();
    dt.us = datetime.microsecond();
    dt.jd = datetime.julianDay();
    dt.tzd = datetime.tzd();
    return dt;
}

bool ofxMoment::validDT(DT dt){
    return Poco::DateTime::isValid(dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.ms, dt.us);
}

bool ofxMoment::assignDT(Poco::LocalDateTime& datetime, DT dt){
    bool vdt = validDT(dt);
    if(vdt){
        datetime.assign(dt.tzd, dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.ms, dt.us);
    }
    return vdt;
}

string ofxMoment::weekday(){
    return format("%W");
}

void ofxMoment::startOf(string timeUnit){
    // the following switch intentionally omits break keywords
    // to utilize falling through the cases.
//    switch (timeUnit) {
//        case 'year':
//            this.month(0);
//            /* falls through */
//        case 'quarter':
//        case 'month':
//            this.date(1);
//            /* falls through */
//        case 'week':
//        case 'isoWeek':
//        case 'day':
//            this.hours(0);
//            /* falls through */
//        case 'hour':
//            this.minutes(0);
//            /* falls through */
//        case 'minute':
//            this.seconds(0);
//            /* falls through */
//        case 'second':
//            this.milliseconds(0);
//    }
//    
//    // weeks are a special case
//    if (units === 'week') {
//        this.weekday(0);
//    }
//    if (units === 'isoWeek') {
//        this.isoWeekday(1);
//    }
//    
//    // quarters are also special
//    if (units === 'quarter') {
//        this.month(Math.floor(this.month() / 3) * 3);
//    }
    if(timeUnit == "year"){
        month(0);
    }
}