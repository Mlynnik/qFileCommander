#include "archive_tree.h"
#include "helperfunctions.h"
#include <QHeaderView>
#include <string>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <shlobj.h>
#include <QMessageBox>

using namespace std;

Archive_tree::Archive_tree(QString _fname, const AppSettings *_appSettings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setFocus(Qt::ActiveWindowFocusReason);

    if (!QFile(_fname).exists())
        this->close();
    fname = _fname;
    appSettings = _appSettings;

    setGeometry(round(appSettings->w*100), round(appSettings->h*200), round(appSettings->w*1000), round(appSettings->h*600));
    setWindowTitle(fname);
    setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    setFont(*appSettings->main_font);
    horizontalLayout = new QVBoxLayout(this);
    horizontalLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    line_path = new QLineEdit(this);
    line_path->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    line_path->setReadOnly(true);
    horizontalLayout->addWidget(line_path);

    tree = new QTreeWidget(this);
    tree->setColumnCount(3);
    tree->headerItem()->setText(0, "↑Имя");
    tree->headerItem()->setText(1, "Тип");
    tree->headerItem()->setText(2, "Размер");
    tree->headerItem()->setText(3, "Сжат");
    tree->headerItem()->setText(4, "Дата");
    tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setDragDropMode(QAbstractItemView::NoDragDrop);
    tree->setRootIsDecorated(false);
    tree->setContextMenuPolicy(Qt::NoContextMenu);
    tree->header()->setFont(*appSettings->main_font);
    tree->header()->setStyleSheet("QHeaderView::section {background: rgb(240,240,240);};");
    tree->header()->setSectionsMovable(false);
    tree->header()->setSectionsClickable(false);
    tree->setFont(*appSettings->panel_font);
    tree->header()->setFont(*appSettings->main_font);
    horizontalLayout->addWidget(tree);


    tree->header()->resizeSection(0, round(appSettings->w*350));
    tree->header()->resizeSection(1, round(appSettings->w*150));
    tree->header()->resizeSection(2, round(appSettings->w*150));
    tree->header()->resizeSection(3, round(appSettings->w*150));
    tree->header()->resizeSection(4, round(appSettings->w*200));

    connect(tree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(treeWidget_itemActivated(QTreeWidgetItem*,int)));

    QWidget *wid_btn = new QWidget(this);
    wid_btn->setFixedHeight(50);
    QHBoxLayout *lay_btn = new QHBoxLayout(wid_btn);
    btn_add = new QPushButton("Добавить", this);
    btn_remove = new QPushButton("Удалить", this);
    btn_decompr = new QPushButton("Извлечь", this);
    btn_info = new QPushButton("Сведения", this);

    connect(btn_add, SIGNAL(clicked()), this, SLOT(add_file()));
    connect(btn_remove, SIGNAL(clicked()), this, SLOT(remove_file()));
    connect(btn_decompr, SIGNAL(clicked()), this, SLOT(decompress()));
    connect(btn_info, SIGNAL(clicked()), this, SLOT(info_show()));

    lay_btn->addWidget(btn_add);
    lay_btn->addWidget(btn_remove);
    lay_btn->addWidget(btn_decompr);
    lay_btn->addWidget(btn_info);

    horizontalLayout->addWidget(wid_btn);

    root = new ANode("", "", 0, 0);
    cur_node = root;

    download_arc();
    root->sort_name();
    cur_path.clear();
    if (str_info.size() > 74)
        str_info = str_info.last(str_info.size() - 74);
    set_Active_Dir(root);
}


void Archive_tree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
        remove_file();
    if (event->key() == Qt::Key_Backspace) {
        if (cur_node != root)
            set_Active_Dir(cur_node->parent);
    }
}


void Archive_tree::download_arc()
{
    QString comnd = "7zz.exe l ";
    comnd = "7zz.exe l \"" % fname.replace('/', '\\') % "\"";

    ANode *cur = root;
    ANode *tcur = root;
    QString path_now = "";

    wchar_t *c_str2 = new wchar_t[comnd.length()+1]; comnd.toWCharArray(c_str2);
    c_str2[comnd.length()] = 0;
    FILE *f = _wpopen(c_str2, L"r");

    char *s = new char[100000];
    s[0] = 0;

    const char delim[4] = "---";
    const char atype[7] = "Type =";
    string str1;

    while (fgets(s, 10000, f)) {
        if (!strncmp(s, delim, 3))
            break;
        str1 = string(s);
        cp866_cp1251(str1);
        str_info += QString::fromLocal8Bit(str1);
    }


    QString fdate;

    bool is_d;
    unsigned long long fsize;
    unsigned long long fcompr;

    QString fpath;

    while (fgets(s, 10000, f)) {
        if (!strncmp(s, delim, 3))
            break;

        s[16] = 0;
        fdate = QString(s);

        int i;
        if (s[20] == 'D')
            is_d = true;
        else {
            is_d = false;

            s[38] = 0;
            for (i = 26; i < 38; ++i) {
                if (s[i] != ' ') {
                    fsize = stoull(s+i);
                    break;
                }
            }

            s[51] = 0;
            for (i = 39; i < 51; ++i) {
                if (s[i] != ' ') {
                    fcompr = stoull(s+i);
                    break;
                }
            }
        }

        i = 53;
        while (*(s+i)) {
            if(*(s+i) < -80)
                *(s+i) += 64;
            else if(*(s+i) > -33 && *(s+i) < -16)
                *(s+i) += 16;
            else if (*(s+i) == -1)
                *(s+i) = -96;
            ++i;
        }

        fpath = QString::fromLocal8Bit(s+53);

        fpath.removeLast();
        if (is_d) {
            if (fpath.contains('\\')) {
                QStringList l_path = fpath.split('\\');
                if (path_now != fpath.left(fpath.lastIndexOf('\\'))) {
                    cur = root;
                    path_now = fpath.left(fpath.lastIndexOf('\\'));
                    for (int i = 0; i < l_path.length() - 1; ++i) {
                        tcur = cur->getDir(l_path[i]);
                        if (tcur == nullptr) {
                            tcur = new ANode(l_path[i], fdate, 0, 0);
                            cur->addDir(tcur);
                        }
                        cur = tcur;
                    }
                }
                tcur = cur->getDir(l_path.last());
                if (tcur)
                    tcur->fdate = fdate;
                else
                    cur->addDir(new ANode(l_path.last(), fdate, 0, 0));
            } else {
                tcur = root->getDir(fpath);
                if (tcur)
                    tcur->fdate = fdate;
                else
                    root->addDir(new ANode(fpath, fdate, 0, 0));
            }
        }
        else {
            if (fpath.contains('\\')) {
                QStringList l_path = fpath.split('\\');
                if (path_now != fpath.left(fpath.lastIndexOf('\\'))) {
                    cur = root;
                    path_now = fpath.left(fpath.lastIndexOf('\\'));
                    for (int i = 0; i < l_path.length() - 1; ++i) {
                        tcur = cur->getDir(l_path[i]);
                        if (tcur == nullptr) {
                            tcur = new ANode(l_path[i], fdate, 0, 0);
                            cur->addDir(tcur);
                        }
                        cur = tcur;
                    }
                }
                cur->addFile(new ANode(l_path.last(), fdate, fsize, fcompr));
            } else {
                root->addFile(new ANode(fpath, fdate, fsize, fcompr));
            }
        }
    }

    while (fgets(s, 10000, f)) {
        str1 = string(s);
        cp866_cp1251(str1);
        str_info += QString::fromLocal8Bit(str1);
    }


    _pclose(f);
    delete[] s;
}

void Archive_tree::set_Active_Dir(ANode *n)
{
    cur_path = line_path->text();
    if (n == cur_node->parent)
        cur_path = cur_path.removeLast().left(cur_path.lastIndexOf('\\')+1);
    else if (n == root)
        cur_path.clear();
    else
        cur_path = cur_path % n->fname % "\\";
    line_path->setText(cur_path);
    cur_node = n;
    tree->clear();
    if (n != root) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree);
        item->setText(0, "..");
        item->setText(1, "<DIR>");

    }
    auto it = n->dnodes.begin();
    auto end = n->dnodes.end();
    for (; it != end; ++it) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree);
        item->setText(0, (*it)->fname);
        item->setIcon(0, dir_ic);
        item->setData(0, Qt::UserRole, QVariant::fromValue<ANode*>(*it));
        item->setText(1, "<DIR>");
        item->setText(4, (*it)->fdate);
    }

    QString fname; int pos_d = 0;
    it = n->fnodes.begin();
    end = n->fnodes.end();
    for (; it != end; ++it) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree);
        fname = (*it)->fname;
        pos_d = fname.lastIndexOf('.');
        item->setIcon(0, IC_PR.icon(QFileInfo(fname)));
        item->setData(0, Qt::UserRole, QVariant::fromValue<ANode*>(*it));
        if (pos_d == -1) {
            item->setText(0, fname);
        } else {
            item->setText(0, fname.left(pos_d));
            item->setText(1, fname.right(fname.size() - pos_d - 1));
        }
        item->setText(2, HelperFunctions::reformat_size((*it)->fsize));
        item->setText(3, HelperFunctions::reformat_size((*it)->fcompr));
        item->setText(4, (*it)->fdate);
    }
}

void Archive_tree::add_file()
{
    QFileDialog fd;
    fd.setFileMode(QFileDialog::Directory);
    fd.setOption(QFileDialog::DontUseNativeDialog, true);
    QTreeView *t = fd.findChild<QTreeView*>();
    if (t)
        t->setSelectionMode(QAbstractItemView::MultiSelection);
    fd.setDirectory(QDir::homePath());
    if (!fd.exec() || fd.selectedFiles().size() < 1)
        return;

    QStringList fnames = fd.selectedFiles();
    QString t_path = fd.directory().path();
    QString comnd = " a \"" % fname % "\" ";
    if (fnames.size() == 1 && fnames[0] == t_path)
        return;
    for (int i = 0; i < fnames.size(); ++i) {
        if (fnames[i] != t_path)
            comnd = comnd % " \"" % fnames[i].replace('/', '\\') % "\"";
    }

    wchar_t *c_str2 = new wchar_t[comnd.length()+1]; comnd.toWCharArray(c_str2);
    c_str2[comnd.length()] = 0;
    ShellExecute(NULL, L"open", L"7zz.exe", c_str2, 0, SW_SHOWNORMAL);

    delete root;

    root = new ANode("", "", 0, 0);
    cur_node = root;
    str_info.clear();
    download_arc();
    set_Active_Dir(root);
}

void Archive_tree::remove_file()
{
    if (tree->selectedItems().size() < 1)
        return;

    if (tree->topLevelItem(0)->text(0) == ".." && tree->topLevelItem(0)->text(1) == "<DIR>")
        tree->topLevelItem(0)->setSelected(false);

    QList<QTreeWidgetItem*> sel_items = tree->selectedItems();
    if (sel_items.size() < 1)
        return;

    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    msgBox.setFont(*appSettings->dialog_font);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Удалить выбранные файлы (" % QString::number(sel_items.size()) % " шт) из архива?");
    msgBox.addButton("Да", QMessageBox::YesRole);
    msgBox.addButton("Отмена", QMessageBox::NoRole);
    if (msgBox.exec() == 1) {
        return;
    }

    QString comnd = "d \"" % fname % "\"";

    QList<ANode*> remove_d = QList<ANode*>();
    QList<ANode*> remove_f = QList<ANode*>();

    for (int i = 0; i < sel_items.size(); ++i) {
        if (sel_items[i]->text(1) == "<DIR>") {
            remove_d.push_back(sel_items[i]->data(0, Qt::UserRole).value<ANode*>());
            comnd = comnd % " \"" % cur_path % remove_d.last()->fname % "\"";
        } else {
            remove_f.push_back(sel_items[i]->data(0, Qt::UserRole).value<ANode*>());
            comnd = comnd % " \"" % cur_path % remove_f.last()->fname % "\"";
        }
    }

    wchar_t *c_str2 = new wchar_t[comnd.length()+1]; comnd.toWCharArray(c_str2);
    c_str2[comnd.length()] = 0;
    ShellExecute(NULL, L"open", L"7zz.exe", c_str2, 0, SW_SHOWNORMAL);

    ANode* parent;
    if (remove_d.size() > 0)
        parent = remove_d[0]->parent;
    else
        parent = remove_f[0]->parent;

    for (int i = 0; i < remove_d.size(); ++i) {
        parent->dnodes.removeOne(remove_d[i]);
        delete remove_d[i];
    }

    for (int i = 0; i < remove_f.size(); ++i) {
        parent->fnodes.removeOne(remove_f[i]);
        delete remove_f[i];
    }
    set_Active_Dir(cur_node);
}

void Archive_tree::decompress()
{
    if (tree->selectedItems().size() < 1)
        return;

    if (tree->topLevelItem(0)->text(0) == ".." && tree->topLevelItem(0)->text(1) == "<DIR>")
        tree->topLevelItem(0)->setSelected(false);

    QList<QTreeWidgetItem*> sel_items = tree->selectedItems();
    if (sel_items.size() < 1)
        return;

    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::Directory);
    fd.setDirectory(QDir::homePath());
    if (!fd.exec() || fd.selectedFiles().size() != 1)
        return;
    QString new_path = fd.selectedFiles().at(0);

    QString comnd = "x \"" % fname % "\" \"-o" % new_path % "\"";

    for (int i = 0; i < sel_items.size(); ++i) {
        comnd = comnd % " \"" % cur_path % sel_items[i]->data(0, Qt::UserRole).value<ANode*>()->fname % "\"";
    }

    wchar_t *c_str2 = new wchar_t[comnd.length()+1]; comnd.toWCharArray(c_str2);
    c_str2[comnd.length()] = 0;
    ShellExecute(NULL, L"open", L"7zz.exe", c_str2, 0, SW_SHOWNORMAL);

}

void Archive_tree::info_show()
{
    QPlainTextEdit *text_info = new QPlainTextEdit();
    text_info->window()->setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    text_info->window()->setWindowTitle(fname);
    text_info->window()->setAttribute(Qt::WA_DeleteOnClose);
    text_info->window()->setWindowModality(Qt::ApplicationModal);
    text_info->setGeometry(round(appSettings->w*300), round(appSettings->w*200), round(appSettings->w*800), round(appSettings->w*400));
    text_info->setPlainText(str_info);
    QFont fnt("Consolas"); fnt.setBold(true); fnt.setPointSize(14);
    text_info->setFont(fnt);
    text_info->setReadOnly(true);
    text_info->show();
}

void Archive_tree::treeWidget_itemActivated(QTreeWidgetItem *item, int column)
{
    if (item->text(1) == "<DIR>") {
        if (item->text(0) == "..") {
            set_Active_Dir(cur_node->parent);
            return;
        }
        set_Active_Dir(item->data(0, Qt::UserRole).value<ANode*>());
    }
}

