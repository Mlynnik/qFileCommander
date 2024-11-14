#include "helperfunctions.h"

HelperFunctions::HelperFunctions() {}


QString HelperFunctions::reformat_size(QString str)
{
    int x = str.length() - 3;
    while(x > 0) {str.insert(x, QString(" ")); x -= 3;}
    return str;
}


QString HelperFunctions::reformat_size_2(float num_0)
{
    QString str1;
    if (num_0 >= 1000) {
        num_0 = round(num_0 / 10.24) / 100;
        str1 = QString::number(num_0) + " КБ";
        if (num_0 >= 1000) {
            num_0 = round(num_0 / 10.24) / 100;
            str1 = QString::number(num_0) + " MБ";
            if (num_0 >= 1000) {
                num_0 = round(num_0 / 10.24) / 100;
                str1 = QString::number(num_0) + " ГБ";
                if (num_0 >= 1000) {
                    num_0 = round(num_0 / 10.24) / 100;
                    str1 = QString::number(num_0) + " ТБ";
                }
            }
        }
    } else
        str1 = QString::number(num_0) + " Б";
    return str1;
}

QString HelperFunctions::reform_date(QString date0)
{
    QString date1;
    if (date0.startsWith("0"))
        date0.remove(0, 1);
    if (date0.contains(".01."))
        date1 = date0.replace(".01.", " декабря ");
    else if (date0.contains(".02."))
        date1 = date0.replace(".02.", " февраля ");
    else if (date0.contains(".03."))
        date1 = date0.replace(".03.", " марта ");
    else if (date0.contains(".04."))
        date1 = date0.replace(".04.", " апреля ");
    else if (date0.contains(".05."))
        date1 = date0.replace(".05.", " мая ");
    else if (date0.contains(".06."))
        date1 = date0.replace(".06.", " июня ");
    else if (date0.contains(".07."))
        date1 = date0.replace(".07.", " июля ");
    else if (date0.contains(".08."))
        date1 = date0.replace(".08.", " августа ");
    else if (date0.contains(".09."))
        date1 = date0.replace(".09.", " сентября ");
    else if (date0.contains(".10."))
        date1 = date0.replace(".10.", " октября ");
    else if (date0.contains(".11."))
        date1 = date0.replace(".11.", " ноября ");
    else if (date0.contains(".12."))
        date1 = date0.replace(".12.", " декабря ");
    return date1;
}
