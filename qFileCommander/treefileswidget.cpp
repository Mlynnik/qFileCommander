#include "treefileswidget.h"
#include "helperfunctions.h"
#include <QMimeData>
#include <QFileIconProvider>
#include <QMimeDatabase>

TreeFilesWidget::TreeFilesWidget(QWidget *parent) : QTreeWidget{parent} {}

//заполнение дерева по директории
void TreeFilesWidget::Fill(const QString &dir_str, bool hidden_file, QString last_dir_l)
{
    QDir dir(dir_str);
    QFileIconProvider IC_PR;
    QMimeDatabase DB;
    QFileInfoList list;

    all_v = 0.0;
    all_f = 0;

    switch (index_sort) {
    case 0:
        if (!hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
        break;
    case 1:
        if (!hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Type);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Type);
        break;
    case 2:
        if (!hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Size);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden,  QDir::Size);
        break;
    case 3:
        if (!hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Time);
        break;
    default:
        if (!hidden_file)
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        else
            list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
        break;
    }

    for (int i = 0; i < 4; ++i) {
        if (this->headerItem()->text(i).startsWith("↓")) {
            std::reverse(list.begin(), list.end());
            break;
        }
    }


    list_colors.clear();
    list_selected.clear();

    if (dir_str.count("\\") > 1) {
        QTreeWidgetItem *item0 = new QTreeWidgetItem(this);
        item0->setIcon(0, style()->standardIcon(QStyle::SP_FileDialogToParent));
        item0->setForeground(0, QColor(168,106,0));
        item0->setText(0, "..");
        item0->setText(1, "<DIR>");

        list_colors.append(QColor(168,106,0));
        list_selected.append(false);

        this->setCurrentItem(item0);
    }

    QColor color;
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        if (fileInfo.isDir()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(this);
            if (fileInfo.isHidden()) {
                color = QColor(120, 120, 120);
            } else {
                color = QColor(72,72,0);//QColor(168, 106, 0);
            }
            item->setIcon(0, IC_PR.icon(fileInfo));
            item->setText(0, fileInfo.fileName());
            item->setText(1, "<DIR>");
            item->setText(3, fileInfo.lastModified().toString("dd.MM.yyyy hh:mm"));
            item->setTextAlignment(2, Qt::AlignRight);
            item->setTextAlignment(3, Qt::AlignRight);
            QString tool_tip = fileInfo.fileName() + "\nДата создания: " + fileInfo.birthTime().toString("dd.MM.yyyy hh:mm");
            item->setToolTip(0, tool_tip);
            item->setToolTip(1, tool_tip);
            item->setToolTip(2, tool_tip);
            item->setToolTip(3, tool_tip);
            if (fileInfo.fileName() == last_dir_l)
                this->setCurrentItem(item);

            item->setForeground(0, color);
            item->setForeground(1, color);
            item->setForeground(2, color);
            item->setForeground(3, color);

            list_colors.append(color);
            list_selected.append(false);
        }
    }

    QDateTime time_now = QDateTime::currentDateTime();
    static QList<QString> *suff_zip = new QList<QString> { "zip", "rar", "arc", "tar" };
    static QList<QString> *suff_temp = new QList<QString> { "tmp", "temp",  "dat" };
    static QList<QString> *suff_sys = new QList<QString> { "sys", "drv", "dll", "bin", "dmp", "hlp", "prf", "reg", "rom", "scr" };
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isFile()) {
            color = QColor(0, 0, 0);
            QTreeWidgetItem *item = new QTreeWidgetItem(this);
            if (fileInfo.isExecutable())
                color = QColor(143, 24, 36);
            else if(suff_zip->contains(fileInfo.suffix()))
                color = QColor(72, 72, 0);
            else if(suff_temp->contains(fileInfo.suffix()))
                color = QColor(126, 126, 126);
            else if (suff_sys->contains(fileInfo.suffix()))
                color = QColor(88, 61, 83);
            else if(fileInfo.lastModified().daysTo(time_now) < 2)
                color = QColor(0, 85, 170);
            else if(fileInfo.lastModified().daysTo(time_now) < 4)
                color = QColor(0, 54, 108);
            else {
                QMimeType mime = DB.mimeTypeForFile(fileInfo);
                QString m_s = mime.name();
                if (m_s.contains("text"))
                    color = QColor(43, 94, 0);
                else if (m_s.contains("image"))
                    color = QColor(136, 0, 136);
                else if (m_s.contains("video"))
                    color = QColor(171, 5, 125);
            }

            item->setIcon(0, IC_PR.icon(fileInfo));
            item->setText(0, fileInfo.fileName().left(fileInfo.fileName().lastIndexOf(".")));

            item->setText(1, fileInfo.suffix());
            all_v += fileInfo.size();
            all_f += 1;
            item->setText(2, HelperFunctions::reformat_size(QString::number(fileInfo.size(), 'g', 20)));
            item->setText(3, fileInfo.lastModified().toString("dd.MM.yyyy hh:mm"));
            item->setTextAlignment(2, Qt::AlignRight);
            item->setTextAlignment(3, Qt::AlignRight);
            QString tool_tip = fileInfo.fileName() + "\nРазмер: " + HelperFunctions::reformat_size_2(fileInfo.size()) + "\nДата создания: "
                               + fileInfo.birthTime().toString("dd.MM.yyyy hh:mm");
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
    }
}
