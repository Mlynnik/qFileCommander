#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QFont>

struct AppSettings {
    float w;
    float h;
    QFont* main_font;
    QFont* panel_font;
    QFont* dialog_font;
    QFont* lister_font;
};

#endif // APPSETTINGS_H
