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
public:
    explicit TreeFilesWidget(QWidget *parent = nullptr);
    ///активная директория
    QString path;
    ///0 - Name, 1 - Type, 2 - Size, 3 - Date
    short index_sort = 0;
    ///обратный порядок заполнения
    bool is_reverse = false;
    ///общий размер файлов в активной директории
    double all_v = 0;
    ///кол-во файлов в активной директории
    int all_f = 0;

    ///индекс предыдущего (по времени выделения) item
    int previous_item_ind = 0;
    ///индекс, который был перед (shift + clickItem)
    int begin_shift = 0;

    ///выделенные item
    QList<QTreeWidgetItem *> selectedItems() const;

signals:
    void drop_signal(QStringList lst, bool remove_after);

public slots:
    ///заполнение дерева по директории
    void Fill(const QString &dir_str, bool hidden_file, const QString& last_dir_l);
    ///кол-во выделенных item
    int count_selected();
    ///второй режим выделения
    void change_select(int ind, bool is_sel);
    ///снять выделение
    void unselected_all();

private:
    ///true - move file, false - copy
    bool is_r_move = true;
    ///является ли shift-выделением
    bool flag_shift = false;

    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QList<QTreeWidgetItem *> &items) const override;
    void startDrag(Qt::DropActions supportedActions) override;


    ///список с цветами файлов
    QList<QColor> list_colors;
    ///список выделенных item
    QList<bool> list_selected;


    QList<QString> suff_zip;
    QList<QString> suff_temp;
    QList<QString> suff_sys;

    //drop menu
    QMenu* drop_menu;
    QAction *copy_action;
    QAction *move_action;

private slots:
    ///смена активного item
    void current_itemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    ///клик по item
    void widget_itemClicked(QTreeWidgetItem *item, int column);

    void keyPressEvent(QKeyEvent *event) override;

    void drop_func_signal(const QStringList &lst, bool remove_after);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

#endif // TREEFILESWIDGET_H
