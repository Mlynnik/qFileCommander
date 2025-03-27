#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "appsettings.h"
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>

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
    void ref_clicked(int ind);
};


class SettingsFavWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsFavWidget(AppSettings* _appSettings, QToolButton *_menu_l, QToolButton *_menu_r, QWidget *parent = nullptr);

signals:
    void update_menu(QStringList fnames, QStringList fpathes);

public slots:
    void ok_clicked();
    void cancel_clicked();
    void rename_clicked();
    void del_clicked();
    void add_clicked();

private:
    AppSettings *appSettings;
    QToolButton *menu_l;
    QToolButton *menu_r;
    QGridLayout *gridLayout;
    QListWidget *listWidget;
    QPushButton *btn_add;
    QPushButton *btn_del;
    QPushButton *btn_rename;
    QSpacerItem *verticalSpacer;
    QPushButton *btn_ok;
    QPushButton *btn_cancel;
    QLineEdit *path;

private slots:
    void keyPressEvent(QKeyEvent *event);
};

#endif // SETTINGSWIDGET_H
