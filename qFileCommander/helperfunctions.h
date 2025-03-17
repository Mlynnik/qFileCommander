#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QString>

class HelperFunctions
{
public:
    HelperFunctions();
    ///форматированный вывод размера
    static QString reformat_size(long long int in_n);
    ///форматированный вывод размера с единицами измерения
    static QString reformat_size_2(float num_0);
};

#endif // HELPERFUNCTIONS_H
