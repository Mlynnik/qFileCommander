#include "treefileswidget.h"
#include "helperfunctions.h"
#include "shellfuncs.h"
#include <QClipboard>
#include <QDrag>
#include <QDragMoveEvent>
#include <QMenu>
#include <QApplication>

TreeFilesWidget::TreeFilesWidget(QWidget *parent) : QTreeWidget{parent}
{
    suff_zip = { "7z", "zip", "rar", "arc", "tar", "xz", "gz" };
    suff_temp = { "tmp", "temp",  "dat" };
    suff_sys = { "sys", "drv", "dll", "bin", "dmp", "hlp", "prf", "reg", "rom", "scr" };

    drop_menu = new QMenu(this);
    copy_action = new QAction("Копировать", this);
    move_action = new QAction("Переместить", this);

    drop_menu->addAction(copy_action);
    drop_menu->addAction(move_action);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(widget_itemClicked(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(current_itemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

QList<QTreeWidgetItem *> TreeFilesWidget::selectedItems() const
{
    QList<QTreeWidgetItem*> res;
    for (int i = 0; i < this->topLevelItemCount(); ++i) {
        if (list_selected[i])
            res.append(this->topLevelItem(i));
    }
    if ((res.length() < 1) && (this->topLevelItemCount() > 0)) {
        if (indexOfTopLevelItem(currentItem()) > -1)
            res.append(this->currentItem());
        else
            res.append(topLevelItem(0));
    }
    return res;
}

bool TreeFilesWidget::is_arc(const QString &fname)
{
    qsizetype ind = fname.lastIndexOf('.');
    if (ind == -1)
        return false;
    QString suf = fname.last(fname.size() - ind - 1);
    if (suff_zip.contains(suf))
        return true;
    return false;
}

void TreeFilesWidget::Fill(const QString &dir_str, bool hidden_file, const QString &last_dir_l)
{
    QDir dir(dir_str);
    QFileInfoList list;
    QFileInfo fileInfo;
    QString tool_tip;

    all_v = 0.0;
    all_f = 0;

    list_colors.clear();
    list_selected.clear();

    previous_item_ind = 0;
    begin_shift = 0;

    if (index_sort == 3) {
        if (hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Time);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
    } else {
        if (hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    }

    if (is_reverse && ((index_sort == 0) || (index_sort == 3))) {
        std::reverse(list.begin(), list.end());
    }


    if (dir_str.count("/") > 1) {
        QTreeWidgetItem *item0 = new QTreeWidgetItem(this);
        item0->setIcon(0, style()->standardIcon(QStyle::SP_FileDialogToParent));
        item0->setForeground(0, QColor(72,72,0));
        item0->setForeground(1, QColor(72,72,0));
        item0->setForeground(2, QColor(72,72,0));
        item0->setForeground(3, QColor(72,72,0));
        item0->setText(0, "..");
        item0->setText(1, "<DIR>");

        if (QDir(dir_str + "..").exists()) {
            fileInfo = QFileInfo(dir_str + "..");
            item0->setText(3, fileInfo.lastModified().toString("dd.MM.yyyy hh:mm"));
            item0->setTextAlignment(3, Qt::AlignLeft);
            tool_tip = fileInfo.fileName() % "\nДата создания: " % fileInfo.birthTime().toString("dd.MM.yyyy hh:mm");
            item0->setToolTip(0, tool_tip);
            item0->setToolTip(1, tool_tip);
            item0->setToolTip(2, tool_tip);
            item0->setToolTip(3, tool_tip);
        }

        list_colors.append(QColor(72,72,0));
        list_selected.append(false);

        this->setCurrentItem(item0);
    }

    QColor color;
    for (int i = 0; i < list.size(); ++i) {
        fileInfo = list.at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        if (fileInfo.isHidden()) {
            color = QColor(120, 120, 120);
        } else {
            color = QColor(72,72,0);//QColor(168, 106, 0);
        }
        item->setIcon(0, IC_PR.icon(fileInfo));
        item->setText(0, fileInfo.fileName());
        item->setData(0, Qt::UserRole, fileInfo.filePath());
        item->setText(1, "<DIR>");
        item->setText(3, fileInfo.lastModified().toString("dd.MM.yyyy hh:mm"));
        item->setTextAlignment(2, Qt::AlignRight);
        item->setTextAlignment(3, Qt::AlignLeft);
        tool_tip = fileInfo.fileName() % "\nДата создания: " % fileInfo.birthTime().toString("dd.MM.yyyy hh:mm");
        item->setToolTip(0, tool_tip);
        item->setToolTip(1, tool_tip);
        item->setToolTip(2, tool_tip);
        item->setToolTip(3, tool_tip);
        if (fileInfo.fileName() == last_dir_l) {
            this->setCurrentItem(item);
            begin_shift = i;
            previous_item_ind = i;
        }

        item->setForeground(0, color);
        item->setForeground(1, color);
        item->setForeground(2, color);
        item->setForeground(3, color);

        list_colors.append(color);
        list_selected.append(false);
    }


    list.clear();
    switch (index_sort) {
    case 0:
        if (hidden_file)
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
        else
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        break;
    case 1:
        if (hidden_file)
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Type);
        else
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Type);
        break;
    case 2:
        if (hidden_file)
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Size);
        else
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot,  QDir::Size);
        break;
    case 3:
        if (hidden_file)
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Time);
        else
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
        break;
    default:
        if (hidden_file)
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
        else
            list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        break;
    }

    if (is_reverse) {
        std::reverse(list.begin(), list.end());
    }

    QDateTime time_now = QDateTime::currentDateTime();
    QDateTime l_mod;
    QMimeType mime;
    QString m_s;
    for (int i = 0; i < list.size(); ++i) {
        fileInfo = list.at(i);
        l_mod = fileInfo.lastModified();
        color = QColor(0, 0, 0);
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        if (fileInfo.isExecutable())
            color = QColor(143, 24, 36);
        else if(suff_zip.contains(fileInfo.suffix()))
            color = QColor(72, 72, 0);
        else if(suff_temp.contains(fileInfo.suffix()))
            color = QColor(126, 126, 126);
        else if (suff_sys.contains(fileInfo.suffix()))
            color = QColor(88, 61, 83);
        else if(l_mod.daysTo(time_now) < 2)
            color = QColor(0, 85, 170);
        else if(l_mod.daysTo(time_now) < 4)
            color = QColor(0, 54, 108);
        else {
            mime = DB.mimeTypeForFile(fileInfo);
            m_s = mime.name();
            if (m_s.contains("text"))
                color = QColor(43, 94, 0);
            else if (m_s.contains("image"))
                color = QColor(136, 0, 136);
            else if (m_s.contains("video"))
                color = QColor(171, 5, 125);
        }

        item->setIcon(0, IC_PR.icon(fileInfo));
        item->setText(0, fileInfo.fileName().left(fileInfo.fileName().lastIndexOf(".")));

        item->setData(0, Qt::UserRole, fileInfo.filePath());

        item->setText(1, fileInfo.suffix());
        all_v += fileInfo.size();
        all_f += 1;
        item->setText(2, reformat_size(fileInfo.size()) + "      ");
        item->setText(3, l_mod.toString("dd.MM.yyyy hh:mm"));
        item->setTextAlignment(2, Qt::AlignRight);
        item->setTextAlignment(3, Qt::AlignLeft);
        tool_tip = fileInfo.fileName() % "\nРазмер: " % reformat_size_2(fileInfo.size()) % "\nДата создания: "
                           % fileInfo.birthTime().toString("dd.MM.yyyy hh:mm");
        item->setToolTip(0, tool_tip);
        item->setToolTip(1, tool_tip);
        item->setToolTip(2, tool_tip);
        item->setToolTip(3, tool_tip);
        item->setForeground(0, color);
        item->setForeground(1, color);
        item->setForeground(2, color);
        item->setForeground(3, color);

        list_colors.append(color);
        list_selected.append(false);
    }

    if ((indexOfTopLevelItem(currentItem()) < 0) && (topLevelItemCount() > 0))
        setCurrentItem(topLevelItem(0));
}


void TreeFilesWidget::unselected_all()
{
    for (int i = 0; i < this->topLevelItemCount(); ++i) {
        change_select(i, false);
    }
}

void TreeFilesWidget::change_select(int ind, bool is_sel)
{
    if (is_sel) {
        list_selected[ind] = true;
        this->topLevelItem(ind)->setForeground(0, QColor(255, 0, 0));
        this->topLevelItem(ind)->setForeground(1, QColor(255, 0, 0));
        this->topLevelItem(ind)->setForeground(2, QColor(255, 0, 0));
        this->topLevelItem(ind)->setForeground(3, QColor(255, 0, 0));
    } else {
        list_selected[ind] = false;
        this->topLevelItem(ind)->setForeground(0, list_colors[ind]);
        this->topLevelItem(ind)->setForeground(1, list_colors[ind]);
        this->topLevelItem(ind)->setForeground(2, list_colors[ind]);
        this->topLevelItem(ind)->setForeground(3, list_colors[ind]);
    }
}

int TreeFilesWidget::count_selected()
{
    return list_selected.count(true);
}


void TreeFilesWidget::current_itemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    previous_item_ind = this->indexOfTopLevelItem(previous);
    if (QGuiApplication::keyboardModifiers() != Qt::ShiftModifier)
        flag_shift = false;
    emit curItemUpdate(current);
}

void TreeFilesWidget::widget_itemClicked(QTreeWidgetItem *item, int column)
{
    int ind = this->indexOfTopLevelItem(item);
    if (previous_item_ind == -1)
        previous_item_ind = 0;

    if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        if (list_selected[ind]) {
            change_select(ind, false);
        } else {
            if (list_selected.indexOf(true) == -1) {
                change_select(previous_item_ind, true);
            }

            change_select(ind, true);
        }
        flag_shift = false;
    } else if (QGuiApplication::keyboardModifiers() == Qt::ShiftModifier) {

        if (!(flag_shift && (list_selected.count(true) > 1))) {
            begin_shift = previous_item_ind;
        }

        int f, l;
        if (ind < begin_shift) {
            f = ind;
            l = begin_shift;
        } else {
            f = begin_shift;
            l = ind;
        }

        for (int i = 0; i < f; ++i) {
            change_select(i, false);
        }

        for (int i = f; i <= l; ++i) {
            change_select(i, true);
        }

        for (int i = l + 1; i < this->topLevelItemCount(); ++i) {
            change_select(i, false);
        }

        flag_shift = true;
    } else {
        flag_shift = false;
        if (count_selected() > 0)
            this->unselected_all();
    }

    if (!item->isSelected()) {
        item->setSelected(true);
    }
    previous_item_ind = this->indexOfTopLevelItem(item);
    emit this->itemSelectionChanged();
}


void TreeFilesWidget::keyPressEvent(QKeyEvent *event)
{
    if (indexOfTopLevelItem(currentItem()) == -1) {
        setCurrentItem(topLevelItem(0));
        previous_item_ind = 0;
    }
    if (event->modifiers() == Qt::ShiftModifier) {
        if (event->key() == Qt::Key_Up) {
            if (!flag_shift) {
                unselected_all();
                begin_shift = previous_item_ind;
                flag_shift = true;

                change_select(begin_shift, true);
            }

            if (this->currentIndex().row() > 0) {
                int n_ind = this->indexOfTopLevelItem(currentItem());
                if (list_selected[n_ind]) {
                    if (list_selected[n_ind - 1]) {
                        change_select(n_ind, false);
                    } else {
                        --n_ind;
                        change_select(n_ind, true);
                    }
                } else {
                    --n_ind;
                    change_select(n_ind, true);
                }
                this->setCurrentItem(this->topLevelItem(this->currentIndex().row() - 1));
            }
            return;
        } else if (event->key() == Qt::Key_Down) {
            if (!flag_shift) {
                unselected_all();
                begin_shift = previous_item_ind;
                flag_shift = true;

                change_select(begin_shift, true);
            }

            if (this->currentIndex().row() + 1 < this->topLevelItemCount()) {
                int n_ind = this->indexOfTopLevelItem(currentItem());
                if (list_selected[n_ind]) {
                    if (list_selected[n_ind + 1]) {
                        change_select(n_ind, false);
                    } else {
                        ++n_ind;
                        change_select(n_ind, true);
                    }
                } else {
                    ++n_ind;
                    change_select(n_ind, true);
                }
                this->setCurrentItem(this->topLevelItem(this->currentIndex().row() + 1));
            }
            return;
        }
    } else if (flag_shift)
        flag_shift = false;

    if (event->modifiers() == Qt::ControlModifier) {
        if (event->key() == Qt::Key_A) {
            if (!((this->topLevelItem(0)->text(0) == "..") && (this->topLevelItem(0)->text(1) == "<DIR>"))) {
                change_select(0, true);
            }

            for (int i = 1; i < this->topLevelItemCount(); ++i) {
                change_select(i, true);
            }
            return;
        } else if ((event->key() == Qt::Key_C) || (event->key() == Qt::Key_X)) {

            const QList<QTreeWidgetItem*> &items = this->selectedItems();
            if (event->key() == Qt::Key_C)
                emit put_to_clipboard(false, items);
            else
                emit put_to_clipboard(true, items);
            return;

        } else if (event->key() == Qt::Key_V) {
            emit paste_signal(path);
            return;
        }
    } else if (event->key() == Qt::Key_Insert) {
        int ind = this->currentIndex().row();
        if (list_selected[ind]) {
            change_select(ind, false);
        } else {
            change_select(ind, true);
        }
        if (this->currentIndex().row() + 1 < this->topLevelItemCount())
            this->setCurrentItem(this->topLevelItem(this->currentIndex().row() + 1));
        emit this->itemSelectionChanged();
        return;
    } else if (event->key() == Qt::Key_Space) {
        int ind = this->currentIndex().row();
        if (list_selected[ind]) {
            change_select(ind, false);
        } else {
            change_select(ind, true);
        }
        emit this->itemSelectionChanged();
        return;
    } else if (event->key() == Qt::Key_Backspace) {
        if ((topLevelItem(0)->text(0) == "..") && (topLevelItem(0)->text(1) == "<DIR>"))
            emit itemActivated(topLevelItem(0), 0);
        return;
    }
    QTreeWidget::keyPressEvent(event);
}


QStringList TreeFilesWidget::mimeTypes() const
{
    return QStringList(QLatin1String("text/uri-list"));
}

QMimeData* TreeFilesWidget::mimeData(const QList<QTreeWidgetItem *> &items) const
{
    QList<QUrl> urls;
    int i = 0;
    if ((items[0]->text(0) == "..") && (items[0]->text(1) == "<DIR>"))
        ++i;
    for (; i < items.length(); ++i) {
        urls << QUrl::fromLocalFile(items[i]->data(0, Qt::UserRole).toString());
    }
    QMimeData* data = new QMimeData();
    data->setUrls(urls);
    return data;
}

void TreeFilesWidget::startDrag(Qt::DropActions supportedActions)
{
    if(supportedActions){
        QList<QTreeWidgetItem*> m_items = selectedItems();
        if(m_items.isEmpty())
            return;
        QMimeData *data = mimeData(m_items);
        QDrag *drag = new QDrag(this);
        QPixmap pixmap = QIcon(style()->standardIcon(QStyle::SP_FileIcon)).pixmap(30, 50);
        drag->setPixmap(pixmap);
        drag->setMimeData(data);
        drag->setHotSpot(pixmap.rect().center());
        drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
        //drag->exec();
    }
    else
        QTreeWidget::startDrag(supportedActions);
}

void TreeFilesWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->buttons() == Qt::LeftButton) {
        is_r_move = false;
    } else {
        is_r_move = true;
    }
    event->acceptProposedAction();
}

void TreeFilesWidget::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void TreeFilesWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void TreeFilesWidget::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        for (int i = 0; i < urlList.size();++i)
        {
            pathList.append(urlList.at(i).toLocalFile());
        }

        if (QFileInfo(pathList.first()).absoluteDir() != QDir(path)) {
            if (is_r_move) {
                disconnect(copy_action, &QAction::triggered, 0, 0);
                disconnect(move_action, &QAction::triggered, 0, 0);
                connect(copy_action, &QAction::triggered, this, [this, pathList] { drop_func_signal(pathList, false); });
                connect(move_action, &QAction::triggered, this, [this, pathList] { drop_func_signal(pathList, true); });
                drop_menu->popup(this->viewport()->mapToGlobal(event->position().toPoint()));
            } else {
                drop_func_signal(pathList, true);
            }
        }
        event->acceptProposedAction();
    }
}

void TreeFilesWidget::drop_func_signal(const QStringList& lst, bool remove_after)
{
    emit drop_signal(lst, path, remove_after);
}

