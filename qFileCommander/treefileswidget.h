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
    ///0 - Name, 1 - Type, 2- Size, 3 - Date
    short index_sort = 0;
    ///общий размер файлов в активной директории
    double all_v = 0;
    ///кол-во файлов в активной директории
    int all_f = 0;

    ///выделенные item
    QList<QTreeWidgetItem *> selectedItems() const;

signals:
    void drop_signal(QStringList lst, bool remove_after);

public slots:
    ///заполнение дерева по директории
    void Fill(const QString &dir_str, bool hidden_file, QString last_dir_l);
    ///кол-во выделенных item
    int count_selected();

private:
    ///true - move file, false - copy
    bool is_r_move = true;
    ///индекс предыдущего (по времени выделения) item
    int previous_item_ind;
    ///является ли shift-выделением
    bool flag_shift = false;
    ///индекс, который был перед (shift + clickItem)
    int begin_shift;

    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QList<QTreeWidgetItem *> &items) const override;
    void startDrag(Qt::DropActions supportedActions) override;


    ///список с цветами файлов
    QList<QColor> list_colors = *new QList<QColor>();
    ///список выделенных item
    QList<bool> list_selected = *new QList<bool>();

private slots:
    ///смена активного item
    void current_itemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    ///клик по item
    void widget_itemClicked(QTreeWidgetItem *item, int column);
    ///снять выделение
    void unselected_all();
    ///второй режим выделения
    void change_select(int ind, bool is_sel);

    void keyPressEvent(QKeyEvent *event) override;

    void drop_func_signal(QStringList lst, bool remove_after);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

#endif // TREEFILESWIDGET_H
