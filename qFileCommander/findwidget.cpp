#include "findwidget.h"
#include <QLineEdit>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QThread>
#include <QMenu>
#include <QDesktopServices>
#include <qevent.h>
#include <windows.h>

FindWidget::FindWidget(QWidget *parent, float _w, float _h) : QWidget{parent}
{
    w = _w;
    h = _h;
    this->setWindowTitle("Find files");
    this->setGeometry(round(w*350), round(h*200), round(w*800), round(h*500));
    lab_find->setText("Найти");
    comb_file->setEditable(true);
    connect(comb_file->lineEdit(), &QLineEdit::returnPressed, this, &FindWidget::find_f_0);
    find_file->setText("Поиск");
    find_cont->setText("Содержит текст");
    connect(find_file, SIGNAL(clicked()), this, SLOT(find_f_0()));
    comb_cont->setEditable(true);
    cancel_comb->setText("Отмена");
    connect(cancel_comb, &QPushButton::clicked, this, &FindWidget::cancel_clicked);
    lab_dir_find->setText("Место");
    comb_dir->setEditable(true);
    connect(comb_dir->lineEdit(), &QLineEdit::returnPressed, this, &FindWidget::find_f_0);
    select_dir_b->setText("..");
    select_dir_b->setMaximumWidth(round(w*60));
    connect(select_dir_b, SIGNAL(clicked()), this, SLOT(dir_select_comb()));
    find_res_0->setText("Результаты поиска");
    find_res_0->setMaximumWidth(round(w*142));
    gr_lay->setVerticalSpacing(round(h*10));
    gr_lay->addWidget(lab_find, 0, 0);
    gr_lay->addWidget(comb_file, 0, 1);
    gr_lay->addWidget(find_file, 0, 2);
    gr_lay->addWidget(find_cont, 1, 0);
    gr_lay->addWidget(comb_cont, 1, 1);
    gr_lay->addWidget(cancel_comb, 1, 2);
    gr_lay->addWidget(lab_dir_find, 2, 0);
    gr_lay->addWidget(comb_dir, 2, 1);
    gr_lay->addWidget(select_dir_b, 2, 2);
    gr_lay->addWidget(find_res_0, 3, 0);
    gr_lay->addWidget(find_res_1, 3, 1);
    gr_lay->addWidget(tree_find, 4, 0, 1, 3);
    tree_find->setColumnCount(1);
    tree_find->setHeaderHidden(true);
    tree_find->setRootIsDecorated(false);
    tree_find->setContextMenuPolicy(Qt::CustomContextMenu);

    menu = new QMenu(this);
    menu_open = new QAction("Открыть", this);
    menu_open_in_dir = new QAction("Показать в папке", this);
    menu_open_in_dir_exp = new QAction("Показать в папке (Проводник)", this);
    menu->addAction(menu_open);
    menu->addSeparator();
    menu->addAction(menu_open_in_dir);
    menu->addAction(menu_open_in_dir_exp);


    connect(tree_find, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(open_find_f(QTreeWidgetItem*,int)));
    connect(tree_find, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tree_find_customContextMenuRequested(QPoint)));
}


void FindWidget::dir_select_comb()
{
    QString dir_0 = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Find files"), comb_dir->itemText(comb_dir->currentIndex())));
    if (!dir_0.isEmpty()) {
        if (comb_dir->findText(dir_0) == -1)
            comb_dir->addItem(dir_0);
        comb_dir->setCurrentIndex(comb_dir->findText(dir_0));
    }
    this->activateWindow();
}

void FindWidget::find_f_0()
{
    if (find_file->text() == "Поиск") {

        find_file->setText("Прервать");
        stop = false;
        QString f_n = comb_file->lineEdit()->text();
        QString d_n = comb_dir->lineEdit()->text();
        QString f_cont = comb_cont->lineEdit()->text();

        tree_find->clear();
        find_res_1->clear();


        if (d_n.isEmpty()) {
            find_file->setText("Поиск");
            return;
        }

        if (!QDir(d_n).exists()) {
            v_error("Указанного пути не существует");
            find_file->setText("Поиск");
            return;
        }

        QThread *th = new QThread(this);
        Find_Process *fp = new Find_Process(f_n, d_n, f_cont);

        connect(th, SIGNAL(started()), fp, SLOT(Work()));
        connect(fp, SIGNAL(add_item(QString,QFileInfo)), this, SLOT(add_item(QString,QFileInfo)));
        connect(fp, SIGNAL(end_of_search(int,int)), this, SLOT(end_of_search(int,int)));

        connect(this, SIGNAL(stop_find()), fp, SLOT(stop_find()), Qt::DirectConnection);

        fp->moveToThread(th);
        th->start();
    }
    else {
        find_file->setText("Поиск");
        emit stop_find();
    }
}

void FindWidget::add_item(QString path, QFileInfo fi)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(tree_find);
    item->setText(0, path);
    item->setIcon(0, ic_pr.icon(fi));
}

void FindWidget::end_of_search(int n_f, int n_d)
{
    find_file->setText("Поиск");
    find_res_1->setText("Найдено: файлов - " + QString::number(n_f) + ", каталогов - " + QString::number(n_d));
}

void FindWidget::cancel_clicked()
{
    emit stop_find();
    tree_find->clear();
    find_res_1->clear();
    this->hide();
}

void FindWidget::close_wid()
{
    is_close = true;
    this->close();
}

void FindWidget::set_dir(QString dir)
{
    if (comb_dir->findText(dir) == -1)
        comb_dir->addItem(dir);
    comb_dir->setCurrentIndex(comb_dir->findText(dir));
}

void FindWidget::open_find_f(QTreeWidgetItem *item, int)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(item->text(0)));
}

void FindWidget::open_find_fid(QString f_name)
{
    emit open_find_fid_signal(f_name);
}

void FindWidget::open_find_fid_exp(QString f_name)
{
    f_name.replace("/", "\\");
    f_name = "/select, \"" + f_name + "\"";
    std::vector<wchar_t> res(f_name.size() + 1);
    ShellExecute(NULL, L"open", L"explorer.exe", (const wchar_t*)f_name.utf16(), NULL, SW_SHOWDEFAULT);
}

void FindWidget::tree_find_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = tree_find->itemAt(pos);
    if (item) {
        menu_open->disconnect();
        menu_open_in_dir->disconnect();
        menu_open_in_dir_exp->disconnect();
        connect(menu_open, &QAction::triggered, this, [this, item] {open_find_f(item, 0);});
        connect(menu_open_in_dir, &QAction::triggered, this, [this, item] {open_find_fid(item->text(0));});
        connect(menu_open_in_dir_exp, &QAction::triggered, this, [this, item] {open_find_fid_exp(item->text(0));});
        menu->popup(tree_find->viewport()->mapToGlobal(pos));
    }
}

void FindWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    emit stop_find();
    if (is_close) {
        event->accept();
        return;
    }
    tree_find->clear();
    find_res_1->clear();
    this->hide();
}


void FindWidget::v_error(QString str_error) {
    QMessageBox v_err;
    v_err.setFont(this->font());
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
}













Find_Process::Find_Process(QString file_name, QString path, QString f_cont)
    :file_name(file_name), path(path), f_cont(f_cont)
{}

void Find_Process::Work()
{
    last_f_n = file_name;
    last_d_n = path;
    last_f_cont = f_cont;
    QStringList filter;
    if (!file_name.isEmpty())
        filter << file_name;
    QDirIterator it(path, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    float n_f = 0;
    float n_d = 0;
    if (f_cont.isEmpty()) {
        while (it.hasNext()) {
            emit add_item(it.next(), it.fileInfo());
            if (it.fileInfo().isDir())
                ++n_d;
            else
                ++n_f;
            if (stop)
                goto lab_end;
        }
    } else {
        while (it.hasNext()) {
            QString fileName = it.next();
            QMimeType mime = db.mimeTypeForFile(fileName);
            if (mime.isValid() && !mime.inherits(QStringLiteral("text/plain"))) {
                continue;
            }
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly)) {
                QString line;
                QTextStream strm(&file);
                while (!strm.atEnd()) {
                    line = strm.readLine();
                    if (line.contains(f_cont)) {
                        emit add_item(fileName, it.fileInfo());
                        if (it.fileInfo().isDir())
                            ++n_d;
                        else
                            ++n_f;
                        break;
                    }
                    if (stop)
                        goto lab_end;
                }
            }
        }
    }

    lab_end:
    emit end_of_search(n_f, n_d);
    this->deleteLater();
    this->thread()->exit();
}


void Find_Process::stop_find()
{
    stop = true;
}
