#ifndef TREEFILESWIDGET_H
#define TREEFILESWIDGET_H

#include <QTreeWidget>
#include <QObject>
#include <QMimeData>
#include <QUrl>
#include <QDir>

class TreeFilesWidget : public QTreeWidget
{
    Q_OBJECT
private:
    ///список с цветами файлов
    QList<QColor> list_colors = *new QList<QColor>();
    ///список выделенных item
    QList<bool> list_selected = *new QList<bool>();


public:
    explicit TreeFilesWidget(QWidget *parent = nullptr);
    ///активная директория
    QString path;
    ///0 - Name, 1 - Type, 2- Size, 3 - Date
    short index_sort = 0;
    ///общий размер файлов в активной директории
    double all_v = 0;
    ///кол-во файлов в активной директории
    int all_f = 0;

public slots:
    void Fill(const QString &dir_str, bool hidden_file, QString last_dir_l);

};

#endif // TREEFILESWIDGET_H
