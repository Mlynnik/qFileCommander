#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "treefileswidget.h"
#include <QMainWindow>
#include <QGuiApplication>
#include <QScreen>
#include <QComboBox>
#include <QLabel>
#include <QDir>
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QScreen *screen = QGuiApplication::primaryScreen();
    float w = float(screen->geometry().width()) / float(1536);
    float h = float(screen->geometry().height()) / float(864);
    //ширина экрана
    int w_max;
    //высота экрана
    int h_max;

    //левое дерево
    TreeFilesWidget *treeWidget_l;
    //правое дерево
    TreeFilesWidget *treeWidget_r;

    //текущий шрифт
    QFont main_font;

    //список дисков
    QFileInfoList md_old;

    //последний левый путь
    QString last_path_l = "";
    //последний правый путь
    QString last_path_r = "";

    //отображение скрытых файлов
    bool hidden_f = false;

    //левый combobox дисков
    QComboBox *combobox_disk_l = new QComboBox();
    //имя выбранного левого диска
    QLabel *disk_name_l = new QLabel();
    //прогресс бар занятого места левого диска
    QProgressBar *disk_progress_l = new QProgressBar();
    //label занятого места левого диска
    QLabel *disk_free_size_l = new QLabel();

    //правый combobox дисков
    QComboBox *combobox_disk_r = new QComboBox();
    //имя выбранного правого диска
    QLabel *disk_name_r = new QLabel();
    //прогресс бар занятого места правого диска
    QProgressBar *disk_progress_r = new QProgressBar();
    //label занятого места правого диска
    QLabel *disk_free_size_r = new QLabel();

private slots:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);


    //вызвает окно ошибки с переданным текстом
    void v_error(QString str_error);
    //получает список дисков, при его изменении обновляет текущий
    void find_disk();
    //нажатие на кнопку диска слева
    void cl_on_disk_l();
    //нажатие на кнопку диска справа
    void cl_on_disk_r();
    //изменение выбора combobox диска слева
    void combobox_disk_l_changed(int ind);
    //изменение выбора combobox диска справа
    void combobox_disk_r_changed(int ind);
    //изменяет информацию по выбранному левому диску
    void size_d_l(QString disk);
    //изменяет информацию по выбранному правому диску
    void size_d_r(QString disk);

    //изменение левого пути
    void on_path_l_returnPressed();
    //изменение правого пути
    void on_path_r_returnPressed();
};
#endif // MAINWINDOW_H
