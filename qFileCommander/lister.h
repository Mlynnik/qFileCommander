#ifndef Lister_H
#define Lister_H

#include "appsettings.h"
#include <QThread>
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


enum ListerMode {NoMode = 0, Text, Image, Player, Dir};

class Lister : public QMainWindow
{
    Q_OBJECT

public:
    Lister(const QString &_fpath, const AppSettings *_appSettings, QWidget *parent = nullptr);
    ~Lister();

signals:
    void closed();
    void stop();
    void signal_loop();

public slots:
    void setDirInfo(long long int all_size, long long int dcnt, long long int fcnt);
    void break_proc() { is_break = true; }

private slots:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

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

    QString fpath;
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
    QByteArray buf = 0;
    qint64 progress = 0;
    qint64 f_size = 0;

    QThread *th = nullptr;
    bool is_break = false;
};



class DirPropProcess : public QObject
{
    Q_OBJECT

public:
    DirPropProcess(const QString& _fpath);
signals:
    void setInfo(long long int all_size, long long int dcnt, long long int fcnt);
    void break_proc();

public slots:
    void Work();
    void stop() { is_stop = true; }

private:
    bool is_stop = false;
    const QString& fpath;
    long long int all_size = 0;
    long long int dcnt = 0;
    long long int fcnt = 0;
};

#endif // Lister_H
