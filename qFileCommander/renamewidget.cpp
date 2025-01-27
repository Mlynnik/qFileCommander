#include "renamewidget.h"
#include <QHeaderView>
#include <QDropEvent>
#include <QMessageBox>
#include <QThread>
#include <QDir>

Rename_Widget::Rename_Widget(const QFont *_main_font, const QFont *_panel_font, const QFont *_dialog_font, float _w, float _h, QWidget *parent)
    : QMainWindow(parent)
{
    w = _w;
    h = _h;
    main_font = _main_font;
    panel_font = _panel_font;
    dialog_font = _dialog_font;

    Rename_Widget::setFont(*main_font);
    Rename_Widget::setAttribute(Qt::WA_DeleteOnClose);

    Rename_Widget::resize(round(w*1200), round(h*600));
    centralwidget = new QWidget(this);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
    centralwidget->setSizePolicy(sizePolicy);
    verticalLayout = new QVBoxLayout(centralwidget);
    gridLayout = new QGridLayout();
    lineEdit_replace = new QLineEdit(centralwidget);
    lineEdit_replace->setMaximumWidth(round(w*300));

    gridLayout->addWidget(lineEdit_replace, 2, 7, 1, 1);

    pushButton_type_cnt = new QPushButton(centralwidget);

    gridLayout->addWidget(pushButton_type_cnt, 2, 3, 1, 1);

    pushButton_name = new QPushButton(centralwidget);

    gridLayout->addWidget(pushButton_name, 2, 0, 1, 1);

    label_cnt = new QLabel(centralwidget);

    gridLayout->addWidget(label_cnt, 0, 4, 1, 1);

    lineEdit_type = new QLineEdit(centralwidget);

    gridLayout->addWidget(lineEdit_type, 1, 2, 1, 2);

    label_begin = new QLabel(centralwidget);
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_begin->sizePolicy().hasHeightForWidth());
    label_begin->setSizePolicy(sizePolicy1);
    label_begin->setMaximumWidth(round(w*100));

    gridLayout->addWidget(label_begin, 1, 4, 1, 1);

    lineEdit_find = new QLineEdit(centralwidget);
    lineEdit_find->setMaximumWidth(round(w*300));

    gridLayout->addWidget(lineEdit_find, 1, 7, 1, 1);

    label_find = new QLabel(centralwidget);
    sizePolicy1.setHeightForWidth(label_find->sizePolicy().hasHeightForWidth());
    label_find->setSizePolicy(sizePolicy1);
    label_find->setMaximumWidth(round(w*120));

    gridLayout->addWidget(label_find, 1, 6, 1, 1);

    label_step = new QLabel(centralwidget);
    sizePolicy1.setHeightForWidth(label_step->sizePolicy().hasHeightForWidth());
    label_step->setSizePolicy(sizePolicy1);
    label_step->setMaximumWidth(round(w*100));

    gridLayout->addWidget(label_step, 2, 4, 1, 1);

    pushButton_type = new QPushButton(centralwidget);

    gridLayout->addWidget(pushButton_type, 2, 2, 1, 1);

    label_name = new QLabel(centralwidget);

    gridLayout->addWidget(label_name, 0, 0, 1, 2);

    label_type = new QLabel(centralwidget);

    gridLayout->addWidget(label_type, 0, 2, 1, 2);

    lineEdit_step = new QLineEdit(centralwidget);
    sizePolicy1.setHeightForWidth(lineEdit_step->sizePolicy().hasHeightForWidth());
    lineEdit_step->setSizePolicy(sizePolicy1);
    lineEdit_step->setMaximumWidth(round(w*50));

    gridLayout->addWidget(lineEdit_step, 2, 5, 1, 1);

    pushButton_name_cnt = new QPushButton(centralwidget);

    gridLayout->addWidget(pushButton_name_cnt, 2, 1, 1, 1);

    lineEdit_begin = new QLineEdit(centralwidget);
    sizePolicy1.setHeightForWidth(lineEdit_begin->sizePolicy().hasHeightForWidth());
    lineEdit_begin->setSizePolicy(sizePolicy1);
    lineEdit_begin->setMaximumWidth(round(w*50));

    gridLayout->addWidget(lineEdit_begin, 1, 5, 1, 1);

    label_replace = new QLabel(centralwidget);
    sizePolicy1.setHeightForWidth(label_replace->sizePolicy().hasHeightForWidth());
    label_replace->setSizePolicy(sizePolicy1);
    label_replace->setMaximumWidth(round(w*120));

    gridLayout->addWidget(label_replace, 2, 6, 1, 1);

    lineEdit_name = new QLineEdit(centralwidget);

    gridLayout->addWidget(lineEdit_name, 1, 0, 1, 2);

    label_find_replace = new QLabel(centralwidget);

    gridLayout->addWidget(label_find_replace, 0, 6, 1, 2);


    verticalLayout->addLayout(gridLayout);

    label_path = new QLabel(centralwidget);

    verticalLayout->addWidget(label_path);

    treeWidget = new Rename_TreeWidget(centralwidget);
    if (treeWidget->columnCount() < 3)
        treeWidget->setColumnCount(3);
    treeWidget->setRootIsDecorated(false);
    treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
    treeWidget->headerItem()->setText(0, "Имя");
    treeWidget->headerItem()->setText(1, "Новое имя");
    treeWidget->headerItem()->setText(2, "Тип");
    treeWidget->header()->setSectionsMovable(false);
    treeWidget->header()->resizeSection(0, round(w*400));
    treeWidget->header()->resizeSection(1, round(w*650));
    treeWidget->header()->resizeSection(2, round(w*100));

    treeWidget->setFont(*panel_font);
    treeWidget->header()->setFont(*main_font);

    verticalLayout->addWidget(treeWidget);

    pushButton_work = new QPushButton(centralwidget);
    pushButton_work->setMaximumWidth(round(w*200));
    pushButton_work->setLayoutDirection(Qt::RightToLeft);

    verticalLayout->addWidget(pushButton_work);

    Rename_Widget::setCentralWidget(centralwidget);
    Rename_Widget::setWindowTitle("Групповое переименование");
    pushButton_type_cnt->setText("[C] - счетчик");
    pushButton_name->setText("[N] - имя");
    label_cnt->setText("Счетчик");
    label_begin->setText("Начинать с");
    label_find->setText("Найти");
    label_step->setText("Шаг");
    pushButton_type->setText("[E] - тип");
    label_name->setText("Имя");
    label_type->setText("Расширение");
    pushButton_name_cnt->setText("[C] - счетчик");
    label_replace->setText("Заменить");
    label_find_replace->setText("Поиск и замена");
    label_path->setText("Путь");
    pushButton_work->setText("Выполнить");

    pushButton_name->setFocusPolicy(Qt::NoFocus);
    pushButton_name_cnt->setFocusPolicy(Qt::NoFocus);
    pushButton_type->setFocusPolicy(Qt::NoFocus);
    pushButton_type_cnt->setFocusPolicy(Qt::NoFocus);
    pushButton_work->setFocusPolicy(Qt::NoFocus);

    connect(pushButton_name, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_name_clicked()));
    connect(pushButton_name_cnt, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_name_cnt_clicked()));

    connect(pushButton_type, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_type_clicked()));
    connect(pushButton_type_cnt, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_type_cnt_clicked()));

    connect(pushButton_work, SIGNAL(clicked(bool)), this, SLOT(on_pushButton_work_clicked()));

    connect(lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_name_text_changed()));
    connect(lineEdit_type, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_type_text_changed()));

    connect(lineEdit_begin, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_begin_changed()));
    connect(lineEdit_step, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_step_changed()));

    connect(lineEdit_find, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_find_text_changed()));
    connect(lineEdit_replace, SIGNAL(textChanged(QString)), this, SLOT(lineEdit_replace_text_changed()));

    connect(treeWidget, &Rename_TreeWidget::update_cnt, this, [this]() {lineEdit_name_text_changed(); lineEdit_type_text_changed();});
}

void Rename_Widget::Fill(const QString &_dir, QStringList selected_dirs, QStringList selected_files)
{
    dir = _dir;
    if (!dir.endsWith("/"))
        dir += "/";
    label_path->setText(dir);
    QString name;
    QFlags<Qt::ItemFlag> flags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable| Qt::ItemIsEnabled);
    treeWidget->clear();
    for (int i = 0; i < selected_dirs.size(); ++i) {
        name = selected_dirs[i].replace(dir, "");
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setFlags(flags);
        item->setText(0, name);
        item->setText(1, name);
        item->setData(1, Qt::UserRole, name);
        item->setText(2, "<DIR>");
        item->setData(2, Qt::UserRole, "<DIR>");
    }

    QString only_name;
    QString suffix;
    for (int i = 0; i < selected_files.size(); ++i) {
        name = selected_files[i].replace(dir, "");
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setFlags(flags);
        item->setText(0, name);
        if (name.contains(".")) {
            only_name = name.left(name.lastIndexOf("."));
            suffix = name.right(name.size() - only_name.size() - 1);

            item->setText(1, only_name);
            item->setData(1, Qt::UserRole, only_name);
            item->setText(2, suffix);
            item->setData(2, Qt::UserRole, suffix);
        }
        else {
            item->setText(1, name);
            item->setData(1, Qt::UserRole, name);
            item->setText(2, "");
            item->setData(2, Qt::UserRole, "");
        }
    }
    on_pushButton_name_clicked();
    on_pushButton_type_clicked();
}

Rename_Widget::~Rename_Widget()
{
    emit end_operation();
}

void Rename_Widget::on_pushButton_name_clicked()
{
    lineEdit_name->insert("[N]");
}

void Rename_Widget::on_pushButton_name_cnt_clicked()
{
    lineEdit_name->insert("[C]");
}

void Rename_Widget::on_pushButton_type_clicked()
{
    lineEdit_type->insert("[E]");
}

void Rename_Widget::on_pushButton_type_cnt_clicked()
{
    lineEdit_type->insert("[C]");
}

void Rename_Widget::on_pushButton_work_clicked()
{
    QString new_name;
    QString new_suff;
    QStringList past_names;
    QStringList new_names;
    QList<bool> flags_dir;
    QTreeWidgetItem *item;
    qpd = new QProgressDialog(this);
    qpd->setAttribute(Qt::WA_DeleteOnClose);
    qpd->setWindowIcon(QIcon("appIcon.png"));
    qpd->setModal(true);
    qpd->setMaximum(treeWidget->topLevelItemCount());
    qpd->setFont(*dialog_font);
    qpd->setValue(0);
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        item = treeWidget->topLevelItem(i);
        new_name = item->text(1);
        new_suff = item->text(2);
        if ((new_name == "") && ((new_suff == "") || (new_suff == "<DIR>"))) {
            v_error("Пустое имя для файла/папки:\n" % dir % item->text(0));
            return;
        }
        if (new_suff == "<DIR>")
            flags_dir.push_back(true);
        else {
            flags_dir.push_back(false);
            if (new_suff != "")
                new_name += "." + new_suff;
        }
        past_names.push_back(item->text(0));
        new_names.push_back(new_name);
    }
    QThread *th = new QThread(this);
    Rename_Mass *rm = new Rename_Mass(dir, past_names, new_names, flags_dir);


    connect(th, SIGNAL(started()), rm, SLOT(Work()));
    connect(th, SIGNAL(finished()), this, SLOT(end_operation_rename()));


    connect(rm, SIGNAL(error_operation(QString)), this, SLOT(v_error(QString)));
    connect(qpd, SIGNAL(canceled()), rm, SLOT(stop()));
    connect(rm, &Rename_Mass::set_comp_count, this, [this](int i) {qpd->setValue(i);});

    rm->moveToThread(th);
    th->start();
}

void Rename_Widget::lineEdit_name_text_changed()
{
    QString new_name = lineEdit_name->text();
    for (int i = 0; i < ban_symb.size(); ++i) {
        new_name.replace(ban_symb[i], "");
    }
    if (new_name != lineEdit_name->text())
        lineEdit_name->setText(new_name);

    bool ok;
    int beg = lineEdit_begin->text().toInt(&ok);
    if (!ok) {
        lineEdit_begin->setText("0");
        beg = 0;
    }
    int step = lineEdit_step->text().toInt(&ok);
    if (!ok || (step == 0)) {
        lineEdit_step->setText("1");
        step = 1;
    }

    QString temp_name;
    QTreeWidgetItem *item;
    int cnt = beg;
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        item = treeWidget->topLevelItem(i);
        temp_name = new_name;
        temp_name.replace("[N]", item->data(1, Qt::UserRole).toString());
        if (find_now != "")
            temp_name.replace(find_now, replace_now);
        temp_name.replace("[C]", QString::number(cnt));
        item->setText(1, temp_name);
        cnt += step;
    }
}

void Rename_Widget::lineEdit_type_text_changed()
{
    QString new_type = lineEdit_type->text();
    for (int i = 0; i < ban_symb.size(); ++i) {
        new_type.replace(ban_symb[i], "");
    }
    if (new_type != lineEdit_type->text())
        lineEdit_type->setText(new_type);

    bool ok;
    int beg = lineEdit_begin->text().toInt(&ok);
    if (!ok) {
        lineEdit_begin->setText("0");
        beg = 0;
    }
    int step = lineEdit_step->text().toInt(&ok);
    if (!ok || (step == 0)) {
        lineEdit_step->setText("1");
        step = 1;
    }

    QString temp_type;
    QTreeWidgetItem *item;
    int cnt = beg;
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        item = treeWidget->topLevelItem(i);
        if (item->text(2) != "<DIR>") {
            temp_type = new_type;
            temp_type.replace("[C]", QString::number(cnt));
            temp_type.replace("[E]", item->data(2, Qt::UserRole).toString());
            item->setText(2, temp_type);
        }
        cnt += step;
    }
}

void Rename_Widget::lineEdit_begin_changed()
{
    bool ok;
    lineEdit_begin->text().toInt(&ok);
    if (ok) {
        lineEdit_name_text_changed();
        lineEdit_type_text_changed();
    }
}

void Rename_Widget::lineEdit_step_changed()
{
    bool ok;
    int step = lineEdit_step->text().toInt(&ok);
    if (ok || (step != 0)) {
        lineEdit_name_text_changed();
        lineEdit_type_text_changed();
    }
}

void Rename_Widget::lineEdit_find_text_changed()
{
    find_now = lineEdit_find->text();
    for (int i = 0; i < ban_symb.size(); ++i) {
        find_now.replace(ban_symb[i], "");
    }
    lineEdit_name_text_changed();
}

void Rename_Widget::lineEdit_replace_text_changed()
{
    replace_now = lineEdit_replace->text();
    for (int i = 0; i < ban_symb.size(); ++i) {
        replace_now.replace(ban_symb[i], "");
    }
    lineEdit_name_text_changed();
}

void Rename_Widget::end_operation_rename()
{
    this->close();
}


//вызвает окно ошибки с переданным текстом
void Rename_Widget::v_error(QString str_error) {
    QMessageBox v_err;
    v_err.setWindowIcon(QIcon("appIcon.png"));
    v_err.setFont(*dialog_font);
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
}




Rename_TreeWidget::Rename_TreeWidget(QWidget *parent) {}

void Rename_TreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget::dropEvent(event);
    emit update_cnt();
}





Rename_Mass::Rename_Mass(const QString &_dir, const QStringList &_old_names, const QStringList &_new_names, const QList<bool>& _flags_dir)
    : dir(_dir), old_names(_old_names), new_names(_new_names), flags_dir(_flags_dir)    {}

void Rename_Mass::Work()
{
    for (int i = 0; i < old_names.length(); ++i) {
        if (old_names[i] == new_names[i]) {
            emit set_comp_count(i+1);
            continue;
        }
        if (flags_dir[i] && QDir(dir + new_names[i]).exists()) {
            emit error_operation("Папка с именем " % new_names[i] % " уже существует.");
        } else if (!flags_dir[i] && QFile(dir + new_names[i]).exists()) {
            emit error_operation("Папка/файл с именем " % new_names[i] % " уже существует.");
        } else {
            if (!(QDir(dir).rename(old_names[i], new_names[i])))
                emit error_operation("Не удалось переименовать!\n" % old_names[i] % "    в\n" % new_names[i]);
        }
        emit set_comp_count(i+1);
        if (is_stop) {
            this->deleteLater();
            this->thread()->exit();
            return;
        }
    }
    this->deleteLater();
    this->thread()->exit();
}

void Rename_Mass::stop()
{
    is_stop = true;
}

