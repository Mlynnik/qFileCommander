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
#include <QMimeDatabase>


enum ListerMode {NoMode = 0, Text, Image, Player, Dir};
enum FCodes { UTF8 = 0, Local };

class Lister : public QWidget
{
    Q_OBJECT

public:
    Lister(const QString &_fpath, const AppSettings *_appSettings, QWidget *parent = nullptr);
    ~Lister();
    void reFill(const QString &_fpath);
    bool is_Fast_View = false;

signals:
    void closed();
    void stop();

    void change_xml(bool v);
    void change_cod(FCodes new_cod);


private slots:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

    void Fill();

    void setMode(ListerMode m);

    void f3_cod_only_text();
    void f3_cod_html();
    void f3_cod_img();
    void f3_cod_audio();
    void f3_cod_wrap();
    void f3_cod_img_ratio();
    void f3_cod_utf8();
    void f3_cod_local();

    void th_finished() { is_th = false;  if (is_close) this->close(); }
    void set_close() { is_close = true; }

private:
    QString fpath;
    const AppSettings *appSettings;
    ListerMode mode = ListerMode::NoMode;

    QMimeDatabase db;
    QMenuBar *menubar;
    QMenu *menu_view;
    QMenu *menu_cod;
    QAction *cod_only_text;
    QAction *cod_html;
    QAction *cod_img;
    QAction *cod_audio;
    QAction *cod_utf8;
    QAction *cod_local;

    QAction *cod_wrap;
    QAction *cod_img_ratio;

    QVBoxLayout *horizontalLayout = nullptr;
    QWidget *widget_now = nullptr;

    bool is_th = false;
    bool is_close = false;

    FCodes cod = FCodes::UTF8;
    bool is_xml = false;
};



class DirWidget : public QPlainTextEdit {
    Q_OBJECT

public:
    DirWidget(const QString& _fpath, const AppSettings* appSettings, QWidget *parent = nullptr);

signals:
    void stop();
    void th_finished();
    void parent_close();

public slots:
    void setDirInfo(long long int all_size, long long int dcnt, long long int fcnt);
    void reFill(const QString& _fpath);

private:
    QString fpath;
    QThread *th = nullptr;
    bool is_break = false;
    bool is_reFill = false;

private slots:
    void closeEvent(QCloseEvent *event);

    void Fill();
};

class DirPropProcess : public QObject
{
    Q_OBJECT

public:
    DirPropProcess(const QString& _fpath);
signals:
    void setInfo(long long int all_size, long long int dcnt, long long int fcnt);

public slots:
    void Work();
    void stop() { is_stop = true; }

private:
    const QString& fpath;
    bool is_stop = false;
    long long int all_size = 0;
    long long int dcnt = 0;
    long long int fcnt = 0;
};


class TextWidget : public QWidget {
    Q_OBJECT

public:
    TextWidget(const QString& _fpath, const AppSettings *appSettings, QWidget* parent = nullptr, bool _is_xml = false, FCodes _cod = FCodes::UTF8);
    ~TextWidget();

    //переносить слова
    inline static bool wrap_mode;

    inline void change_wrap_mode() {
        if (wrap_mode)
            plainTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
        else
            plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
    }

signals:
    void stop();

public slots:
    void reFill(const QString &_fpath, bool _is_xml = false, FCodes _cod = FCodes::UTF8);

    void change_xml(bool v) { is_xml = v; updateText(); }
    void change_cod(FCodes new_cod) { cod = new_cod; updateText(); }

    void updateText();

    void on_pushButton_up_clicked();
    void on_pushButton_down_clicked();
    void verticalSlider_valueChanged(int value);
    void on_lineEdit_size_now_returnPressed();

private:
    bool is_xml;
    FCodes cod;
    const AppSettings *appSettings;

    QString fpath;
    QGridLayout *gridLayout = nullptr;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_up;
    QLineEdit *lineEdit_size_now;
    QLabel *label_all_size;
    QPlainTextEdit *plainTextEdit;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_down;
    QSlider *verticalSlider;

    QFile* file;
    QByteArray buf = 0;
    qint64 progress = 0;
    qint64 f_size = 0;

private slots:
    void Fill();
    void v_error(QString str_error);
};


class ImageWidget : public QScrollArea {
    Q_OBJECT

public:
    ImageWidget(const QString& _fpath, QWidget* parent = nullptr);
    //масштабировать картинку
    inline static bool img_ratio;

    void change_ratio_mode();

public slots:
    void reFill(const QString& _fpath);

private:
    QString fpath;
    QLabel *label_image;

private slots:
    void Fill();

};

#endif // Lister_H
