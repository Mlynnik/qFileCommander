#ifndef Lister_H
#define Lister_H

#include "appsettings.h"
#include <QMainWindow>
#include <QFile>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSlider>
#include <QMenuBar>
#include <QScrollArea>

enum ListerMode {NoMode = 0, Text, Image, Player, };

class Lister : public QMainWindow
{
    Q_OBJECT

public:
    Lister(const QString &_f_name, const AppSettings *_appSettings, QWidget *parent = nullptr);
    ~Lister();

signals:
    void closed();

private slots:
    void keyPressEvent(QKeyEvent *event);

    void Fill();

    void setMode(ListerMode m);

    void on_pushButton_up_clicked();

    void on_pushButton_down_clicked();

    void verticalSlider_valueChanged(int value);

    void on_lineEdit_size_now_returnPressed();

    void f3_cod_only_text();
    void f3_cod_html();
    void f3_cod_img();
    void f3_cod_audio();
    void f3_cod_utf8();
    void f3_cod_local();

private:
    ListerMode mode = ListerMode::NoMode;

    QString f_name;
    const AppSettings* appSettings;
    const QFont *main_font;
    const QFont *dialog_font;
    const QFont *lister_font;
    ///t - text, x = xml, i - image, a - audio
    char f_type = 't';
    ///t - text, x = xml, i - image, a - audio
    char f_type_now = 't';
    ///8 - utf8, l - local
    char f_cod = '8';

    QAction *cod_only_text;
    QAction *cod_html;
    QAction *cod_img;
    QAction *cod_audio;
    QAction *cod_utf8;
    QAction *cod_local;

    QWidget *centralwidget = nullptr;
    QHBoxLayout *horizontalLayout = nullptr;
    QGridLayout *gridLayout = nullptr;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_up;
    QLineEdit *lineEdit_size_now;
    QScrollArea *image_area;
    QLabel *label_image;
    QLabel *label_all_size;
    QPlainTextEdit *plainTextEdit;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_down;
    QSlider *verticalSlider;
    QMenuBar *menubar;
    QMenu *menu_view;
    QMenu *menu_cod;

    QFile* file;
    QByteArray prev_buf = 0;
    QByteArray buf = 0;
    QByteArray next_buf = 0;
    qint64 progres = 0;
    qint64 f_size = 0;
};
#endif // Lister_H
