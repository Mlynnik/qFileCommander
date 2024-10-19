#ifndef TREEFILESWIDGET_H
#define TREEFILESWIDGET_H

#include <QTreeWidget>
#include <QObject>

class TreeFilesWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit TreeFilesWidget(QWidget *parent = nullptr);
};

#endif // TREEFILESWIDGET_H
