#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QString>

///форматированный вывод размера
static QString reformat_size(long long int in_n)
{
    QString in_str = QString::number(in_n);
    int x = in_str.size(); int i = 0;
    QString res;
    res.reserve(x + x / 3);
    const QChar *str = in_str.constData();
    const QChar *end = str + x;

    if (x % 3 == 2) {
        res.append(*str++);
        res.append(*str++);
        res.append(' ');
    } else if (x % 3 == 1) {
        res.append(*str++);
        res.append(' ');
    }
    while (str != end) {
        res.append(*str++);
        res.append(*str++);
        res.append(*str++);
        res.append(' ');
    }
    return res;
}

///форматированный вывод размера с единицами измерения
static QString reformat_size_2(float num_0)
{
    QString str1;
    if (num_0 >= 1000) {
        num_0 = round(num_0 / 10.24) / 100;
        str1 = QString::number(num_0) % " КБ";
        if (num_0 >= 1000) {
            num_0 = round(num_0 / 10.24) / 100;
            str1 = QString::number(num_0) % " MБ";
            if (num_0 >= 1000) {
                num_0 = round(num_0 / 10.24) / 100;
                str1 = QString::number(num_0) % " ГБ";
                if (num_0 >= 1000) {
                    num_0 = round(num_0 / 10.24) / 100;
                    str1 = QString::number(num_0) % " ТБ";
                }
            }
        }
    } else
        str1 = QString::number(num_0) % " Б";
    return str1;
}

#endif // HELPERFUNCTIONS_H
