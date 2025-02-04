#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "appsettings.h"
#include <QWidget>
#include <QTabWidget>
#include <QLabel>

class SettingsWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit SettingsWidget(AppSettings* appsettings, QWidget *parent = nullptr);

signals:
    void apply_main_font();
    void apply_panel_font();

private:
    float w;
    float h;
    QWidget *page_fonts;
    //QWidget *page_ref;
    QLabel *lab_main_font_now;
    QLabel *lab_panel_font_now;
    QLabel *lab_dialog_font_now;
    QLabel *lab_lister_font_now;
    //шрифты
    QFont *main_font;
    QFont *panel_font;
    QFont *dialog_font;
    QFont *lister_font;

private slots:
    void keyPressEvent(QKeyEvent *event);

    void change_main_font();
    void change_panel_font();
    void change_dialog_font();
    void change_lister_font();

};

#endif // SETTINGSWIDGET_H
