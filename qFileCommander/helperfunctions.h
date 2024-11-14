#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QString>

class HelperFunctions
{
public:
    HelperFunctions();
    static QString reformat_size(QString str);
    static QString reformat_size_2(float num_0);
    static QString reform_date(QString date0);
};

#endif // HELPERFUNCTIONS_H
