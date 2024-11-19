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
#include <QPushButton>
#include <QFileIconProvider>

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

    float w_now = w;
    float h_now = h;

    //левое дерево
    TreeFilesWidget *treeWidget_l;
    //ширина колонок левого дерева
    int w_col_l[4] {435, 65, 120, 125};
    //правое дерево
    TreeFilesWidget *treeWidget_r;
    //ширина колонок правого дерева
    int w_col_r[4] {430, 65, 120, 125};

    QFileIconProvider ic_pr;

    //меню
    QMenu* cust_menu;
    QAction * menu_f4;
    QAction * menu_open;
    QAction * menu_f5;
    QAction * copy_as_path;
    QAction * menu_f6;
    QAction * menu_f8;
    QAction * menu_properties;

    //текущий шрифт
    QFont main_font;

    //список дисков
    QFileInfoList md_old;

    //последний левый путь
    QString last_path_l = "";
    //последний правый путь
    QString last_path_r = "";

    //размер всех файлов слева
    float all_l_v = 0.0;
    //размер всех файлов справа
    float all_r_v = 0.0;
    //кол-во файлов слева
    int all_f_l = 0;
    //кол-во файлов справа
    int all_f_r = 0;

    //отображение скрытых файлов
    bool hidden_f = false;

    //диски

    //левый список указателей на qpushbutton
    QList<QPushButton*> l_l_buttons_disk;

    //левый combobox дисков
    QComboBox *combobox_disk_l = new QComboBox();
    //имя выбранного левого диска
    QLabel *disk_name_l = new QLabel();
    //прогресс бар занятого места левого диска
    QProgressBar *disk_progress_l = new QProgressBar();
    //label занятого места левого диска
    QLabel *disk_free_size_l = new QLabel();

    //правый список указателей на qpushbutton
    QList<QPushButton*> r_l_buttons_disk;
    //правый combobox дисков
    QComboBox *combobox_disk_r = new QComboBox();
    //имя выбранного правого диска
    QLabel *disk_name_r = new QLabel();
    //прогресс бар занятого места правого диска
    QProgressBar *disk_progress_r = new QProgressBar();
    //label занятого места правого диска
    QLabel *disk_free_size_r = new QLabel();


    //кол-во активных файловых процессов
    int count_copy_proc = 0;

public slots:
    //срабытывает при завершении операции с файлами (обновляет виджеты)
    void end_copy();

private slots:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

    //изменилась геометрия левого заголовка
    void change_w_col_l(int logicalIndex, int oldSize, int newSize);
    //изменилась геометрия правого заголовка
    void change_w_col_r(int logicalIndex, int oldSize, int newSize);

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

    //обновляет виджеты главного экрана
    void update_widgets();
    //изменение левого пути
    void on_path_l_returnPressed();
    //изменение правого пути
    void on_path_r_returnPressed();
    //клик по шапке левого дерева
    void header_clicked_l(int col);
    //клик по шапке правого дерева
    void header_clicked_r(int col);
    //двойной клик по файлу/папке
    void treeWidget_l_itemActivated(QTreeWidgetItem *item, int column);
    //двойной клик по файлу/папке
    void treeWidget_r_itemActivated(QTreeWidgetItem *item, int column);
    //контекстное меню левого дерева
    void treeWidget_l_customContextMenuRequested(const QPoint &pos);
    //контекстное меню правого дерева
    void treeWidget_r_customContextMenuRequested(const QPoint &pos);
    //срабатывает при изменении выбора строк
    void treeWidget_l_itemSelectionChanged();
    //срабатывает при изменении выбора строк
    void treeWidget_r_itemSelectionChanged();

    //drop файла в указанную директорию
    void drop_func(QStringList lst, bool remove_after, bool is_right);

    //записывает по ссылке папки назначения, выбранные каталоги, выбранные файлы
    void mass_all_selected(QString &dir_to, QStringList &selected_dirs, QStringList &selected_files);

    //переименование файла/каталога
    void on_pushButton_f4_clicked();
    //копировать как путь
    void copy_as_path_clicked();
    //копирование/перемещение
    void f5_f6_func(bool remove_after);
    //копирование
    void on_pushButton_f5_clicked();
    //перемещение
    void on_pushButton_f6_clicked();
    //создание каталога
    void on_pushButton_f7_clicked();
    //удаление файла/каталога
    void on_pushButton_f8_clicked();
    //безвозвратное удаление файла/каталога
    void shift_del_f();
    //отображение свойств файла/папки (файлов/папок)
    void show_properties();
    //отображение/скрытие скрытых файлов
    void show_hidden_func();
    //вызывает окно открыть с помощью...
    void menu_open_with_wind();
    //открывает проводник в активной директории
    void on_pushButton_open_in_exp_clicked();

    //создает новый файл в активной папке
    void on_pushButton_create_file_clicked();

    //открывает блокнот
    void on_pushButton_notepad_clicked();
    //окно выбора шрифта
    void change_font();


    //Lister
    //просмотр файла
    void on_pushButton_f3_clicked();


    //Find Widget
    //поиск файла
    void on_pushButton_find_clicked();

};
#endif // MAINWINDOW_H
