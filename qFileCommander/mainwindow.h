#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "treefileswidget.h"
#include <QMainWindow>
#include <QGuiApplication>
#include <QScreen>
#include <QComboBox>
#include <QLabel>
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
};
#endif // MAINWINDOW_H
