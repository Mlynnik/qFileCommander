#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "helperfunctions.h"
#include "delete_files.h"
#include "copy_files.h"
#include "renamewidget.h"
#include "settingswidget.h"
#include "shellfuncs.h"
#include "archive_tree.h"
#include <windows.h>
#include <shlobj.h>
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QKeyEvent>
#include <QStorageInfo>
#include <QDesktopServices>
#include <QMenu>
#include <QInputDialog>
#include <QClipboard>
#include <QToolTip>
#include <QFontDialog>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QPainter>
#pragma comment(lib, "Shell32.lib")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    treeWidget_l = new TreeFilesWidget(this);
    treeWidget_r = new TreeFilesWidget(this);

    //контекстное меню
    cust_menu = new QMenu(this);
    menu_f4 = new QAction("Переименовать", this);
    menu_open = new QAction("Открыть с помощью", this);
    menu_arc_view = new QAction(QIcon(":/resources/icons/7z.png"), "Просмотр архива", this);
    menu_arc_compr = new QAction(QIcon(":/resources/icons/7z.png"), "Добавить в архив...", this);
    menu_arc_decompr = new QAction(QIcon(":/resources/icons/7z.png"), "Разархивировать в...", this);
    copy_as_path = new QAction("Копировать как путь", this);
    menu_ctrl_c = new QAction("Копировать", this);
    menu_ctrl_x = new QAction("Вырезать", this);
    menu_ctrl_v = new QAction("Вставить", this);
    menu_create_file = new QAction("Создать файл", this);
    menu_f8 = new QAction("Удалить", this);
    menu_properties = new QAction("Свойства", this);


    connect(menu_f4, SIGNAL(triggered()), this, SLOT(on_pushButton_f4_clicked()));
    connect(menu_open, SIGNAL(triggered()), this, SLOT(menu_open_with_wind()));
    connect(menu_arc_view, SIGNAL(triggered()), this, SLOT(view_archive()));
    connect(menu_arc_compr, SIGNAL(triggered()), this, SLOT(compress_archive()));
    connect(menu_arc_decompr, SIGNAL(triggered()), this, SLOT(decompress_archive()));
    connect(copy_as_path, SIGNAL(triggered()), this, SLOT(copy_as_path_clicked()));
    connect(menu_ctrl_c, SIGNAL(triggered()), this, SLOT(ctrl_c_clicked()));
    connect(menu_ctrl_x, SIGNAL(triggered()), this, SLOT(ctrl_x_clicked()));
    connect(menu_ctrl_v, SIGNAL(triggered()), this, SLOT(ctrl_v_clicked()));
    connect(menu_f8, SIGNAL(triggered()), this, SLOT(on_pushButton_f8_clicked()));
    connect(menu_properties, SIGNAL(triggered()), this, SLOT(show_properties()));
    connect(menu_create_file, SIGNAL(triggered()), this, SLOT(on_pushButton_create_file_clicked()));

    cust_menu->addAction(menu_f4);
    cust_menu->addSeparator();
    cust_menu->addAction(menu_open);
    cust_menu->addSeparator();
    cust_menu->addAction(menu_arc_view);
    cust_menu->addAction(menu_arc_compr);
    cust_menu->addAction(menu_arc_decompr);
    cust_menu->addSeparator();
    cust_menu->addAction(copy_as_path);
    cust_menu->addSeparator();
    cust_menu->addAction(menu_ctrl_c);
    cust_menu->addAction(menu_ctrl_x);
    cust_menu->addAction(menu_ctrl_v);
    cust_menu->addSeparator();
    cust_menu->addAction(menu_f8);
    cust_menu->addSeparator();
    cust_menu->addAction(menu_create_file);
    cust_menu->addSeparator();
    cust_menu->addAction(menu_properties);


    MainWindow::setGeometry(0, 0, round(w*1536), round(h*800));
    MainWindow::showMaximized();
    w_max = MainWindow::width();
    h_max = MainWindow::height();
    MainWindow::setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    MainWindow::setWindowTitle("qFileCommander");


    QSettings settings;
    last_path_l = settings.value("/Settings/L_Path", "").toString();
    last_path_r = settings.value("/Settings/R_Path", "").toString();
    hidden_f = settings.value("/Settings/Hidden_F", false).toBool();
    is_7zz = settings.value("/Settings/Arc_App", true).toBool();
    is_api = settings.value("/Settings/Use_Api", true).toBool();
    TextWidget::wrap_mode = settings.value("/Settings/Wrap_Mode", false).toBool();
    ImageWidget::img_ratio = settings.value("/Settings/Img_Ratio", true).toBool();

    main_font.fromString(settings.value("/Settings/Main_Font", "Times New Roman,12,-1,5,700,0,0,0,0,0,0,0,0,0,0,1").toString());
    panel_font.fromString(settings.value("/Settings/Panel_Font", "Times New Roman,12,-1,5,700,0,0,0,0,0,0,0,0,0,0,1").toString());
    dialog_font.fromString(settings.value("/Settings/Dialog_Font", "Times New Roman,12,-1,5,700,0,0,0,0,0,0,0,0,0,0,1").toString());
    lister_font.fromString(settings.value("/Settings/Lister_Font", "Times New Roman,12,-1,5,700,0,0,0,0,0,0,0,0,0,0,1").toString());

    appSettings = new AppSettings();
    appSettings->w = w; appSettings->h = h; appSettings->main_font = &main_font;
    appSettings->panel_font = &panel_font; appSettings->dialog_font = &dialog_font; appSettings->lister_font = &lister_font;
    appSettings->is_7zz = &is_7zz; appSettings->is_api = &is_api;

    find_wid = new FindWidget(appSettings);
    connect(find_wid, SIGNAL(open_find_fid_signal(QString)), this, SLOT(open_find_fid(QString)));

    change_main_font();
    change_panel_font();

    {
        QList<QVariant> widthColumns = settings.value("/Settings/L_Col_W").toList();
        if (!widthColumns.empty() && (widthColumns.size() == 4)) {
            for(int i = 0; i < 4; ++i)
                w_col_l[i] = widthColumns[i].toInt();
        }
        widthColumns.clear();
        widthColumns = settings.value("/Settings/R_Col_W").toList();
        if (!widthColumns.empty() && (widthColumns.size() == 4)) {
            for(int i = 0; i < 4; ++i)
                w_col_r[i] = widthColumns[i].toInt();
        }
    }


    QPalette palette = QToolTip::palette();
    palette.setColor(QPalette::Inactive,QPalette::ToolTipBase,Qt::white);
    palette.setColor(QPalette::Inactive,QPalette::ToolTipText,QColor(102, 102, 102, 255));
    QToolTip::setPalette(palette);
    QToolTip::setFont(main_font);

    QPalette palette_header = QToolTip::palette();
    palette_header.setColor(QPalette::Inactive,QPalette::ToolTipBase,Qt::white);
    palette_header.setColor(QPalette::Inactive,QPalette::ToolTipText,QColor(240,240,240));

    //левое дерево
    treeWidget_l->setObjectName("treeWidget_l");
    treeWidget_l->setGeometry(QRect(5, 100, 705, 580));
    //treeWidget_l->setMouseTracking(true);
    treeWidget_l->headerItem()->setText(0, "Имя");
    treeWidget_l->headerItem()->setText(1, "Тип");
    treeWidget_l->headerItem()->setText(2, "Размер");
    treeWidget_l->headerItem()->setText(3, "Дата");

    treeWidget_l->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    treeWidget_l->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_l->setDragDropMode(QAbstractItemView::DragDrop);
    treeWidget_l->setRootIsDecorated(false);
    treeWidget_l->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget_l->header()->setStyleSheet("QHeaderView::section {background: rgb(240,240,240);};");
    treeWidget_l->header()->setSectionsMovable(false);
    treeWidget_l->header()->setSectionsClickable(true);

    connect(treeWidget_l, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(treeWidget_l_itemActivated(QTreeWidgetItem*,int)));
    connect(treeWidget_l, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeWidget_l_customContextMenuRequested(QPoint)));
    connect(treeWidget_l, SIGNAL(itemSelectionChanged()), this, SLOT(treeWidget_l_itemSelectionChanged()));
    connect(treeWidget_l, SIGNAL(paste_signal(QString)), this, SLOT(paste_func(QString)));
    connect(treeWidget_l, SIGNAL(put_to_clipboard(bool,QList<QTreeWidgetItem*>)), this, SLOT(put_to_clipboard(bool,QList<QTreeWidgetItem*>)));

    connect(treeWidget_l->header(), &QHeaderView::sectionClicked, this, [this](int logicalIndex) {header_clicked_l(logicalIndex);});
    connect(treeWidget_l, &TreeFilesWidget::drop_signal, this, &MainWindow::drop_func);

    //правое дерево
    treeWidget_r->setObjectName("treeWidget_r");
    treeWidget_r->setGeometry(QRect(715, 100, 705, 580));
    treeWidget_r->headerItem()->setText(0, "Имя");
    treeWidget_r->headerItem()->setText(1, "Тип");
    treeWidget_r->headerItem()->setText(2, "Размер");
    treeWidget_r->headerItem()->setText(3, "Дата");

    treeWidget_r->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    treeWidget_r->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_r->setDragDropMode(QAbstractItemView::DragDrop);
    treeWidget_r->setRootIsDecorated(false);
    treeWidget_r->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget_r->header()->setStyleSheet("QHeaderView::section {background: rgb(240,240,240);};");
    treeWidget_r->header()->setSectionsMovable(false);
    treeWidget_r->header()->setSectionsClickable(true);

    connect(treeWidget_r, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(treeWidget_r_itemActivated(QTreeWidgetItem*,int)));
    connect(treeWidget_r, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeWidget_r_customContextMenuRequested(QPoint)));
    connect(treeWidget_r, SIGNAL(itemSelectionChanged()), this, SLOT(treeWidget_r_itemSelectionChanged()));
    connect(treeWidget_r, SIGNAL(paste_signal(QString)), this, SLOT(paste_func(QString)));
    connect(treeWidget_r, SIGNAL(put_to_clipboard(bool,QList<QTreeWidgetItem*>)), this, SLOT(put_to_clipboard(bool,QList<QTreeWidgetItem*>)));

    connect(treeWidget_r->header(), &QHeaderView::sectionClicked, this, [this](int logicalIndex) {header_clicked_r(logicalIndex);});
    connect(treeWidget_r, &TreeFilesWidget::drop_signal, this, &MainWindow::drop_func);

    //верхняя панель
    ui->pushButton_settings->setGeometry(round(w*2), 0, round(w*25), round(h*25));
    ui->pushButton_fast_view->setGeometry(round(w*32), 0, round(w*25), round(h*25));
    ui->pushButton_hidden_f->setGeometry(round(w*62), 0, round(w*25), round(h*25));
    ui->pushButton_admin->setGeometry(round(w*92), 0, round(w*25), round(h*25));
    ui->pushButton_open_in_exp->setGeometry(round(w*122), 0, round(w*25), round(h*25));
    ui->pushButton_notepad->setGeometry(round(w*152), 0, round(w*25), round(h*25));
    ui->pushButton_zip->setGeometry(round(w*192), 0, round(w*25), round(h*25));
    ui->pushButton_unzip->setGeometry(round(w*222), 0, round(w*25), round(h*25));
    ui->pushButton_create_file->setGeometry(round(w*262), 0, round(w*25), round(h*25));
    ui->pushButton_mass_rename->setGeometry(round(w*292), 0, round(w*25), round(h*25));
    ui->pushButton_find->setGeometry(round(w*322), 0, round(w*25), round(h*25));
    ui->line_0->setGeometry(round(w*-5), round(h*25), round(w*1540), round(h*2));
    ui->line_1->setGeometry(round(w*-5), round(h*54), round(w*1540), round(h*2));
    ui->line_2->setGeometry(round(w*-5), round(h*82), round(w*1540), round(h*2));
    ui->line_btn_h1->setGeometry(round(w*182), 0, round(w*5), round(h*25));
    ui->line_btn_h2->setGeometry(round(w*250), 0, round(w*5), round(h*25));

    //иконки кнопок
    ui->pushButton_zip->setIcon(QIcon(":/resources/icons/7z_c.png"));
    ui->pushButton_unzip->setIcon(QIcon(":/resources/icons/7z_u.png"));
    ui->pushButton_settings->setIcon(QIcon(":/resources/icons/settings.png"));
    ui->pushButton_fast_view->setIcon(QIcon(":/resources/icons/fastView.png"));
    ui->pushButton_fast_view->setCheckable(true);
    ui->pushButton_mass_rename->setText("A");
    ui->pushButton_open_in_exp->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOpenButton)));
    ui->pushButton_find->setIcon(QIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView)));
    if (QFile("C:\\Windows\\notepad.exe").exists())
        ui->pushButton_notepad->setIcon(ic_pr.icon(QFileInfo("C:\\Windows\\notepad.exe")));
    ui->pushButton_create_file->setText("+");
    ui->pushButton_admin->setIcon(QIcon(style()->standardIcon(QStyle::SP_VistaShield)));
    if(IsUserAnAdmin()) {
        ui->pushButton_admin->setEnabled(false);
        ui->pushButton_admin->setCheckable(false);
    }
    ui->pushButton_hidden_f->setIcon(QIcon(":/resources/icons/hiddenf.png"));
    ui->pushButton_hidden_f->setCheckable(true);
    if (hidden_f)
        ui->pushButton_hidden_f->setChecked(true);
    connect(ui->pushButton_settings, SIGNAL(clicked()), this, SLOT(open_settings()));
    connect(ui->pushButton_fast_view, SIGNAL(clicked()), this, SLOT(change_fast_view()));
    connect(ui->pushButton_hidden_f, SIGNAL(clicked()), this, SLOT(show_hidden_func()));
    connect(ui->pushButton_zip, SIGNAL(clicked()), this, SLOT(compress_archive()));
    connect(ui->pushButton_unzip, SIGNAL(clicked()), this, SLOT(decompress_archive()));

    //диски
    ui->horizontalLayout_l->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_r->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_free_size_l->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_free_size_l->addWidget(combobox_disk_l);
    ui->horizontalLayout_free_size_l->addWidget(disk_name_l);
    ui->horizontalLayout_free_size_l->addWidget(disk_progress_l);
    ui->horizontalLayout_free_size_l->addWidget(disk_free_size_l, 4);
    connect(combobox_disk_l, SIGNAL(activated(int)), this, SLOT(combobox_disk_l_changed(int)));
    disk_progress_l->setFormat("");
    ui->horizontalLayout_free_size_r->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_free_size_r->addWidget(combobox_disk_r);
    ui->horizontalLayout_free_size_r->addWidget(disk_name_r);
    ui->horizontalLayout_free_size_r->addWidget(disk_progress_r);
    ui->horizontalLayout_free_size_r->addWidget(disk_free_size_r, 4);
    connect(combobox_disk_r, SIGNAL(activated(int)), this, SLOT(combobox_disk_r_changed(int)));
    disk_progress_r->setFormat("");

    //запрет на фокус
    ui->pushButton_f3->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_f4->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_f5->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_f6->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_f7->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_f8->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_settings->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_fast_view->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_hidden_f->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_admin->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_open_in_exp->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_notepad->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_zip->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_unzip->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_create_file->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_mass_rename->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_find->setFocusPolicy(Qt::NoFocus);
    combobox_disk_l->setFocusPolicy(Qt::NoFocus);
    combobox_disk_r->setFocusPolicy(Qt::NoFocus);
    ui->toolButton_favourites_l->setFocusPolicy(Qt::NoFocus);
    ui->toolButton_favourites_r->setFocusPolicy(Qt::NoFocus);


    //избранное
    ui->toolButton_favourites_l->setIcon(QIcon(":/resources/icons/favourites.png"));
    ui->toolButton_favourites_r->setIcon(QIcon(":/resources/icons/favourites.png"));
    ui->toolButton_favourites_l->setPopupMode(QToolButton::InstantPopup);
    ui->toolButton_favourites_r->setPopupMode(QToolButton::InstantPopup);
    ui->toolButton_favourites_l->setToolTip("Избранные каталоги");
    ui->toolButton_favourites_r->setToolTip("Избранные каталоги");

    act_add_fav_l = new QAction("Добавить текущий каталог", this);
    ui->toolButton_favourites_l->addAction(act_add_fav_l);
    connect(act_add_fav_l, &QAction::triggered, this, [this](){add_favourite(true);});
    act_add_fav_r = new QAction("Добавить текущий каталог", this);
    ui->toolButton_favourites_r->addAction(act_add_fav_r);
    connect(act_add_fav_r, &QAction::triggered, this, [this](){add_favourite(false);});

    act_remove_fav_l = new QAction("Исключить текущий каталог", this);
    ui->toolButton_favourites_l->addAction(act_remove_fav_l);
    connect(act_remove_fav_l, &QAction::triggered, this, [this](){remove_favourite(true);});
    act_remove_fav_l->setVisible(false);
    act_remove_fav_r = new QAction("Исключить текущий каталог", this);
    ui->toolButton_favourites_r->addAction(act_remove_fav_r);
    connect(act_remove_fav_r, &QAction::triggered, this, [this](){remove_favourite(false);});
    act_remove_fav_r->setVisible(false);

    act_sett_fav_l = new QAction("Настройка", this);
    ui->toolButton_favourites_l->addAction(act_sett_fav_l);
    connect(act_sett_fav_l, SIGNAL(triggered(bool)), this, SLOT(settings_favourite()));
    act_sett_fav_r = new QAction("Настройка", this);
    ui->toolButton_favourites_r->addAction(act_sett_fav_r);
    connect(act_sett_fav_r, SIGNAL(triggered(bool)), this, SLOT(settings_favourite()));

    {
        QList<QVariant> n_fav = settings.value("/Settings/Names_Favourites").toList();
        QList<QVariant> p_fav = settings.value("/Settings/Paths_Favourites").toList();

        if (!n_fav.empty() && !p_fav.empty() && (n_fav.length() == p_fav.length())) {
            QString new_name, path;
            for (int i = 0; i < n_fav.length(); ++i) {
                new_name = n_fav[i].toString();
                path = p_fav[i].toString();

                QAction *act = new QAction(path, this);
                act->setCheckable(true);
                ui->toolButton_favourites_l->insertAction(act_add_fav_l, act);
                act->setText(new_name);
                act->setData(path);
                connect(act, &QAction::triggered, this, [act, this]() {ui->path_l->setText(act->data().toString());on_path_l_returnPressed();});
                QAction *act2 = new QAction(path, this);
                act2->setCheckable(true);
                ui->toolButton_favourites_r->insertAction(act_add_fav_r, act2);
                act2->setText(new_name);
                act2->setData(path);
                connect(act2, &QAction::triggered, this, [act2, this]() {ui->path_r->setText(act2->data().toString());on_path_r_returnPressed();});
            }
        }
    }


    //пути
    ui->path_l->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    ui->path_l->setFocusPolicy(Qt::ClickFocus);
    ui->path_r->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    ui->path_r->setFocusPolicy(Qt::ClickFocus);

    //если в реестре ошибка, то замена на "c:/"
    if (last_path_l == "" || !QDir().exists(last_path_l))
        last_path_l = "c:/";
    if (last_path_r == "" || !QDir().exists(last_path_r))
        last_path_r = "c:/";

    ui->path_l->setText(last_path_l);
    ui->path_r->setText(last_path_r);


    //запуск таймера на обновление
    find_disk();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(find_disk()));



    //вспоминаем сортировку
    int ind_sort = settings.value("/Settings/Sort_L", 0).toInt();
    if ((ind_sort > -1) && (ind_sort < 8)) {
        if (ind_sort > 3) {
            ind_sort -= 4;
            if ((ind_sort == 0) || (ind_sort == 1))
                treeWidget_l->headerItem()->setText(ind_sort, "↑" + treeWidget_l->headerItem()->text(ind_sort));
            else
                treeWidget_l->headerItem()->setText(ind_sort, "↓" + treeWidget_l->headerItem()->text(ind_sort));
        }
    } else {
        ind_sort = 0;
    }
    emit treeWidget_l->header()->sectionClicked(ind_sort);

    ind_sort = settings.value("/Settings/Sort_R", 0).toInt();
    if ((ind_sort > -1) && (ind_sort < 8)) {
        if (ind_sort > 3) {
            ind_sort -= 4;
            if ((ind_sort == 0) || (ind_sort == 1))
                treeWidget_r->headerItem()->setText(ind_sort, "↑" + treeWidget_r->headerItem()->text(ind_sort));
            else
                treeWidget_r->headerItem()->setText(ind_sort, "↓" + treeWidget_r->headerItem()->text(ind_sort));
        }
    } else
        ind_sort = 0;
    emit treeWidget_r->header()->sectionClicked(ind_sort);

    connect(treeWidget_l->header(), SIGNAL(sectionResized(int,int,int)), this, SLOT(change_w_col_l(int,int,int)));
    connect(treeWidget_r->header(), SIGNAL(sectionResized(int,int,int)), this, SLOT(change_w_col_r(int,int,int)));

    timer->start(3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    w_now = w * float(MainWindow::width()) / float(w_max);
    h_now = h * float(MainWindow::height()) / float(h_max);

    treeWidget_l->header()->blockSignals(true);
    treeWidget_r->header()->blockSignals(true);

    treeWidget_l->setGeometry(round(w_now*1), round(h*110), round(w_now*765), round(h_now*635 - (h-h_now)*170));
    treeWidget_l->header()->resizeSection(0, round(w_now*w_col_l[0]));
    treeWidget_l->header()->resizeSection(1, round(w_now*w_col_l[1]));
    treeWidget_l->header()->resizeSection(2, round(w_now*w_col_l[2]));
    treeWidget_l->header()->resizeSection(3, round(w_now*w_col_l[3]));
    treeWidget_r->setGeometry(round(w_now*770), round(h*110), round(w_now*765), round(h_now*635 - (h-h_now)*170));
    treeWidget_r->header()->resizeSection(0, round(w_now*w_col_r[0]));
    treeWidget_r->header()->resizeSection(1, round(w_now*w_col_r[1]));
    treeWidget_r->header()->resizeSection(2, round(w_now*w_col_r[2]));
    treeWidget_r->header()->resizeSection(3, round(w_now*w_col_r[3]));
    ui->horizontalLayoutWidget->setGeometry(round(w_now*1), round(h*30), round(w_now*325), round(h*20));
    ui->horizontalLayoutWidget_2->setGeometry(round(w_now*770), round(h*30), round(w_now*325), round(h*20));

    if (view_widget)
        view_widget->setGeometry(round(w_now*770), round(h*112), round(w_now*765), round(h_now*633 - (h-h_now)*170));

    ui->line_3->setGeometry(round(w_now*-5), round(h_now*770 - (h-h_now)*35), round(w_now*1540), round(h*2));
    ui->horizontalLayoutWidget_4->setGeometry(round(w_now*1), round(h*58), round(w_now*750), round(h*24));
    disk_progress_l->setMinimumWidth(round(w_now*200));
    disk_progress_l->setMaximumWidth(round(w_now*200));
    disk_progress_l->setMaximumHeight(round(h*15));
    ui->horizontalLayoutWidget_5->setGeometry(round(w_now*770), round(h*58), round(w_now*750), round(h*24));
    disk_progress_r->setMinimumWidth(round(w_now*200));
    disk_progress_r->setMaximumWidth(round(w_now*200));
    disk_progress_r->setMaximumHeight(round(h*15));
    ui->path_l->setGeometry(round(w_now*-1), round(h*87), round(w_now*750 - (w - w_now)*15), round(h*24));
    ui->toolButton_favourites_l->setGeometry(round(w_now*750 - (w - w_now)*15), round(h*89), round(w*15), round(h*20));
    ui->path_r->setGeometry(round(w_now*768), round(h*87), round(w_now*750 - (w - w_now)*15), round(h*24));
    ui->toolButton_favourites_r->setGeometry(round(w_now*1518 - (w - w_now)*15), round(h*89), round(w*15), round(h*20));
    ui->inf_dir_l->setGeometry(round(w_now*1), round(h_now*745 - (h-h_now)*60), round(w_now*650), round(h*22));
    ui->inf_dir_r->setGeometry(round(w_now*770), round(h_now*745 - (h-h_now)*60), round(w_now*650), round(h*22));
    ui->horizontalLayoutWidget_3->setGeometry(round(w_now*1), round(h_now*770 - (h-h_now)*35), round(w_now*1535), round(h*35));

    treeWidget_l->header()->blockSignals(false);
    treeWidget_r->header()->blockSignals(false);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() == Qt::ShiftModifier) {
        if (event->key() == Qt::Key_Delete)
            shift_del_f();
    } else if (event->modifiers() == Qt::ControlModifier) {
        if (event->key() == Qt::Key_F)
            on_pushButton_find_clicked();
    } else {
        switch (event->key()){
        case Qt::Key_Tab:
            if (treeWidget_l->hasFocus())
                treeWidget_r->setFocus();
            else
                treeWidget_l->setFocus();
            break;
        case Qt::Key_F1:
            QDesktopServices::openUrl(QUrl::fromLocalFile("qFileCommander.chm"));
            break;
        case Qt::Key_Delete:
            on_pushButton_f8_clicked();
            break;
        case Qt::Key_F3:
            on_pushButton_f3_clicked();
            break;
        case Qt::Key_F4:
            on_pushButton_f4_clicked();
            break;
        case Qt::Key_F5:
            on_pushButton_f5_clicked();
            break;
        case Qt::Key_F6:
            on_pushButton_f6_clicked();
            break;
        case Qt::Key_F7:
            on_pushButton_f7_clicked();
            break;
        case Qt::Key_F8:
            on_pushButton_f8_clicked();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    if (count_proc > 0) {
        v_error("Не все процессы еще завершены!");
        return;
    }

    save_settings();

    for (auto l : lister_list)
        l->close();

    find_wid->close_wid();

    if (view_widget)
        ui->pushButton_fast_view->click();

    event->accept();
}

void MainWindow::save_settings()
{
    QSettings settings;
    settings.setValue("/Settings/L_Path", last_path_l);
    settings.setValue("/Settings/R_Path", last_path_r);
    settings.setValue("/Settings/Main_Font", main_font.toString());
    settings.setValue("/Settings/Panel_Font", panel_font.toString());
    settings.setValue("/Settings/Dialog_Font", dialog_font.toString());
    settings.setValue("/Settings/Lister_Font", lister_font.toString());
    settings.setValue("/Settings/Hidden_F", hidden_f);
    settings.setValue("/Settings/Arc_App", is_7zz);
    settings.setValue("/Settings/Use_Api", is_api);
    settings.setValue("/Settings/Wrap_Mode", TextWidget::wrap_mode);
    settings.setValue("/Settings/Img_Ratio", ImageWidget::img_ratio);

    QList<QVariant> widthColumns;
    for(int i = 0; i < 4; ++i)
        widthColumns << w_col_l[i];
    settings.setValue("/Settings/L_Col_W", widthColumns);

    widthColumns.clear();
    for(int i = 0; i < 4; ++i)
        widthColumns << w_col_r[i];
    settings.setValue("/Settings/R_Col_W", widthColumns);

    int ind_sort = treeWidget_l->index_sort;
    if (treeWidget_l->is_reverse)
        ind_sort += 4;
    settings.setValue("/Settings/Sort_L", ind_sort);

    ind_sort = treeWidget_r->index_sort;
    if (treeWidget_r->is_reverse)
        ind_sort += 4;
    settings.setValue("/Settings/Sort_R", ind_sort);


    QList<QVariant> n_fav;
    int l_fav =  ui->toolButton_favourites_l->actions().size() - 3;
    for (int i = 0; i < l_fav; ++i) {
        n_fav << ui->toolButton_favourites_l->actions().at(i)->text();
    }
    settings.setValue("/Settings/Names_Favourites", n_fav);
    n_fav.clear();
    for (int i = 0; i < l_fav; ++i) {
        n_fav << ui->toolButton_favourites_l->actions().at(i)->data();
    }
    settings.setValue("/Settings/Paths_Favourites", n_fav);
}

void MainWindow::open_settings()
{
    SettingsWidget *sw = new SettingsWidget(appSettings);
    connect(sw, SIGNAL(apply_main_font()), this, SLOT(change_main_font()));
    connect(sw, SIGNAL(apply_panel_font()), this, SLOT(change_panel_font()));
    sw->show();
}

void MainWindow::change_main_font()
{
    MainWindow::setFont(main_font);
    QToolTip::setFont(main_font);
    treeWidget_l->header()->setFont(main_font);
    treeWidget_r->header()->setFont(main_font);
    ui->path_l->setFont(main_font);
    ui->path_r->setFont(main_font);
    find_wid->setFont(main_font);
}

void MainWindow::change_panel_font()
{
    treeWidget_l->setFont(panel_font);
    treeWidget_r->setFont(panel_font);
}


void MainWindow::add_favourite(bool l)
{
    QString path = l ? last_path_l : last_path_r;
    path = QFileInfo(path).absoluteFilePath();
    QString new_name = path; new_name = new_name.removeLast().split("/").last();

    QInputDialog id;
    id.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    id.setFont(main_font);
    id.resize(QSize(400, 60));
    id.setCancelButtonText("Отмена");
    id.setLabelText("Название нового элемента в избранном:");
    id.setTextValue(new_name);
    if (!id.exec())
        return;

    new_name = id.textValue();
    QAction *act = new QAction(path, this);
    act->setCheckable(true);
    ui->toolButton_favourites_l->insertAction(act_add_fav_l, act);
    act->setText(new_name);
    act->setData(path);
    connect(act, &QAction::triggered, this, [act, this]() {ui->path_l->setText(act->data().toString());on_path_l_returnPressed();});
    QAction *act2 = new QAction(path, this);
    act2->setCheckable(true);
    ui->toolButton_favourites_r->insertAction(act_add_fav_r, act2);
    act2->setText(new_name);
    act2->setData(path);
    connect(act2, &QAction::triggered, this, [act2, this]() {ui->path_r->setText(act2->data().toString());on_path_r_returnPressed();});

    QString fname = QFileInfo(last_path_l).absoluteFilePath();

    if (path == fname) {
        act->setChecked(true);
        act_add_fav_l->setVisible(false);
        act_remove_fav_l->setVisible(true);
    }
    fname = QFileInfo(last_path_r).absoluteFilePath();
    if (path == fname) {
        act2->setChecked(true);
        act_add_fav_r->setVisible(false);
        act_remove_fav_r->setVisible(true);
    }
}

void MainWindow::remove_favourite(bool l)
{
    QToolButton *btn, *btn2;
    QAction *act_a, *act_a2, *act_r, *act_r2;
    if (l) {
        btn = ui->toolButton_favourites_l;
        btn2 = ui->toolButton_favourites_r;
        act_a = act_add_fav_l;
        act_a2 = act_add_fav_r;
        act_r = act_remove_fav_l;
        act_r2 = act_remove_fav_r;
    } else {
        btn = ui->toolButton_favourites_r;
        btn2 = ui->toolButton_favourites_l;
        act_a = act_add_fav_r;
        act_a2 = act_add_fav_l;
        act_r = act_remove_fav_r;
        act_r2 = act_remove_fav_l;
    }
    for (auto a : btn->actions()) {
        if (a->isChecked()) {
            for (auto a2 : btn2->actions()) {
                if (a2->text() == a->text()) {
                    if (a2->isChecked()) {
                        act_a2->setVisible(true);
                        act_r2->setVisible(false);
                    }
                    btn2->removeAction(a2);
                    break;
                }
            }
            btn->removeAction(a);
            act_a->setVisible(true);
            act_r->setVisible(false);
            return;
        }
    }
}

void MainWindow::settings_favourite()
{
    SettingsFavWidget *sfw = new SettingsFavWidget(appSettings, ui->toolButton_favourites_l, ui->toolButton_favourites_r);
    connect(sfw, SIGNAL(update_menu(QStringList,QStringList)), this, SLOT(add_favourites(QStringList,QStringList)));
    sfw->show();
}

void MainWindow::add_favourites(QStringList fnames, QStringList fpathes)
{
    for (int i = 0; i < fnames.length(); ++i) {
        QAction *act = new QAction(fnames[i], this);
        act->setCheckable(true);
        ui->toolButton_favourites_l->insertAction(act_add_fav_l, act);
        act->setData(fpathes[i]);
        connect(act, &QAction::triggered, this, [act, this]() {ui->path_l->setText(act->data().toString());on_path_l_returnPressed();});

        QAction *act2 = new QAction(fnames[i], this);
        act2->setCheckable(true);
        ui->toolButton_favourites_r->insertAction(act_add_fav_r, act2);
        act2->setData(fpathes[i]);
        connect(act2, &QAction::triggered, this, [act2, this]() {ui->path_r->setText(act2->data().toString());on_path_r_returnPressed();});
    }
    on_path_l_returnPressed();
    on_path_r_returnPressed();
}


void MainWindow::change_w_col_l(int logicalIndex, int oldSize, int newSize)
{
    w_col_l[logicalIndex] = trunc(newSize/w_now);
    w_col_l[3] = 760 - w_col_l[0] - w_col_l[1] - w_col_l[2];
    treeWidget_l->header()->resizeSection(3, round(w_now*w_col_l[3]));
}

void MainWindow::change_w_col_r(int logicalIndex, int oldSize, int newSize)
{
    w_col_r[logicalIndex] = trunc(newSize/w_now);
    w_col_r[3] = 760 - w_col_r[0] - w_col_r[1] - w_col_r[2];
    treeWidget_r->header()->resizeSection(3, round(w_now*w_col_r[3]));
}


void MainWindow::change_fast_view()
{
    if (ui->pushButton_fast_view->isChecked()) {
        is_fast_view = true;
        treeWidget_r->hide();
        ui->inf_dir_r->hide();
        ui->path_r->setReadOnly(true);
        ui->toolButton_favourites_r->setDisabled(true);

        QString fpath;
        if (treeWidget_l->currentItem())
            fpath = treeWidget_l->currentItem()->data(0, Qt::UserRole).toString();

        if (fpath == "")
            fpath = last_path_l;

        view_widget = new Lister(fpath, appSettings, this);
        view_widget->is_Fast_View = true;
        ui->path_r->setText(fpath.split("/").last());
        view_widget->show();
        view_widget->setFocusPolicy(Qt::StrongFocus);
        connect(treeWidget_l, SIGNAL(curItemUpdate(QTreeWidgetItem*)), this, SLOT(reDrawFastView(QTreeWidgetItem*)));
    } else {
        disconnect(treeWidget_l, SIGNAL(curItemUpdate(QTreeWidgetItem*)), this, SLOT(reDrawFastView(QTreeWidgetItem*)));
        if (view_widget) {
            view_widget->is_Fast_View = false;
            view_widget->close();
        }
        view_widget = nullptr;

        is_fast_view = false;
        treeWidget_r->show();
        ui->inf_dir_r->show();
        ui->path_r->setReadOnly(false);
        ui->toolButton_favourites_r->setDisabled(false);

        ui->path_r->setText(last_path_r);
        on_path_r_returnPressed();
    }

    resizeEvent(nullptr);
}

void MainWindow::reDrawFastView(QTreeWidgetItem *current)
{
    if (!current)
        return;

    QString fpath = current->data(0, Qt::UserRole).toString();
    if (fpath == "")
        fpath = last_path_l;

    QString fname = fpath.split("/").last();
    if (fname == ui->path_r->text())
        return;

    ui->path_r->setText(fname);
    view_widget->reFill(fpath);
    view_widget->setFocusPolicy(Qt::StrongFocus);
}


//вызвает окно ошибки с переданным текстом
void MainWindow::v_error(QString str_error) {
    QMessageBox v_err;
    v_err.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_err.setFont(dialog_font);
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
}

//получает список дисков, при его изменении обновляет текущий
void MainWindow::find_disk()
{
    QFileInfoList mass_disk = QDir::drives();
    if (md_old != mass_disk) {
        l_l_buttons_disk.clear();
        r_l_buttons_disk.clear();
        md_old = mass_disk;
        QStringList list;
        QString old_disk = last_path_l.split("/").first().removeLast().toLower();

        for (int i = 0; i < mass_disk.length(); i++)
            list << mass_disk[i].filePath().remove(":/").toLower();
        int num = list.length();
        qDeleteAll(ui->horizontalLayoutWidget->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
        qDeleteAll(ui->horizontalLayoutWidget_2->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
        combobox_disk_l->clear();
        combobox_disk_r->clear();
        for (int i = 0; i < num; i ++) {
            combobox_disk_l->addItem(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)), list[i]);
            if (last_path_l.startsWith(list[i]))
                combobox_disk_l->setCurrentIndex(i);
            QPushButton *btn_disk_l_t = new QPushButton(this);
            l_l_buttons_disk.push_back(btn_disk_l_t);
            btn_disk_l_t->setFocusPolicy(Qt::NoFocus);
            btn_disk_l_t->setMaximumWidth(round(w*30));
            btn_disk_l_t->setIcon(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)));
            btn_disk_l_t->setIconSize(QSize(round(w*12), round(h*12)));
            btn_disk_l_t->setText(list[i]);
            btn_disk_l_t->setCheckable(true);
            if (list[i] == old_disk)
                btn_disk_l_t->setChecked(true);
            connect(btn_disk_l_t, SIGNAL(clicked()), this, SLOT(cl_on_disk_l()));
            ui->horizontalLayout_l->addWidget(btn_disk_l_t);

            combobox_disk_r->addItem(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)), list[i]);
            if (last_path_r.startsWith(list[i]))
                combobox_disk_r->setCurrentIndex(i);
            QPushButton *btn_disk_r_t = new QPushButton(this);
            r_l_buttons_disk.push_back(btn_disk_r_t);
            btn_disk_r_t->setFocusPolicy(Qt::NoFocus);
            btn_disk_r_t->setMaximumWidth(round(w*30));
            btn_disk_r_t->setIcon(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)));
            btn_disk_r_t->setIconSize(QSize(round(w*12), round(h*12)));
            btn_disk_r_t->setText(list[i]);
            btn_disk_r_t->setCheckable(true);
            if (list[i] == old_disk)
                btn_disk_r_t->setChecked(true);
            connect(btn_disk_r_t, SIGNAL(clicked()), this, SLOT(cl_on_disk_r()));
            ui->horizontalLayout_r->addWidget(btn_disk_r_t);
        }
        if (!list.contains(old_disk)) {
            v_error("Устройство было извлечено!");
            ui->path_l->setText("c:/");
            on_path_l_returnPressed();
        }
        old_disk = last_path_r.split("/").first().removeLast().toLower();
        if (!list.contains(old_disk)) {
            v_error("Устройство было извлечено!");
            ui->path_r->setText("c:/");
            on_path_r_returnPressed();
        }
    }
}

//нажатие на кнопку диска слева
void MainWindow::cl_on_disk_l(){
    QPushButton *button = (QPushButton *)sender();
    if (last_path_r.startsWith(button->text())) {
        ui->path_l->setText(last_path_r);
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(button->text()));
    }
    else if(!last_path_l.startsWith(button->text())) {
        ui->path_l->setText(button->text() + ":/");
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(button->text()));
    }
    on_path_l_returnPressed();
    size_d_l(button->text());
}

//нажатие на кнопку диска справа
void MainWindow::cl_on_disk_r(){
    QPushButton *button = (QPushButton *)sender();
    if (last_path_l.startsWith(button->text())) {
        ui->path_r->setText(last_path_l);
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(button->text()));
    }
    else if(!last_path_r.startsWith(button->text())) {
        ui->path_r->setText(button->text() + ":/");
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(button->text()));
    }
    on_path_r_returnPressed();
    size_d_r(button->text());
}

//изменение выбора combobox диска слева
void MainWindow::combobox_disk_l_changed(int ind)
{
    if (ind == -1)
        return;
    QString new_disk = combobox_disk_l->itemText(ind);
    if (last_path_r.startsWith(new_disk)) {
        ui->path_l->setText(last_path_r);
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(new_disk));
    }
    else if(!last_path_l.startsWith(new_disk)) {
        ui->path_l->setText(new_disk + ":/");
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(new_disk));
    }
    on_path_l_returnPressed();
    size_d_l(new_disk);
}

//изменение выбора combobox диска справа
void MainWindow::combobox_disk_r_changed(int ind)
{
    if (ind == -1)
        return;
    QString new_disk = combobox_disk_r->itemText(ind);
    if (last_path_l.startsWith(new_disk)) {
        ui->path_r->setText(last_path_l);
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(new_disk));
    }
    else if(!last_path_r.startsWith(new_disk)) {
        ui->path_r->setText(new_disk + ":/");
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(new_disk));
    }
    on_path_r_returnPressed();
    size_d_r(new_disk);
}

//изменяет информацию по выбранному левому диску
void MainWindow::size_d_l(QString disk)
{
    for (int i = 0; i < l_l_buttons_disk.size(); ++i) {
        if (l_l_buttons_disk[i]->text() == disk)
            l_l_buttons_disk[i]->setChecked(true);
        else
            l_l_buttons_disk[i]->setChecked(false);
    }

    disk += ":/";

    QStorageInfo st_inf = QStorageInfo(disk);
    qint64 av_b = st_inf.bytesAvailable();
    qint64 all_b = st_inf.bytesTotal();
    float occup_b = (all_b - av_b)*100.0 / all_b;
    if (occup_b < 90.0)
        disk_progress_l->setStyleSheet("QProgressBar::chunk {background-color: rgb(38,160,218);}");
    else
        disk_progress_l->setStyleSheet("QProgressBar::chunk {background-color: rgb(218,38,38);}");
    disk_name_l->setText("[" % st_inf.name() % "]");
    disk_progress_l->setValue(occup_b);
    disk_free_size_l->setText(reformat_size(av_b/1024) % " КБ из " % reformat_size(all_b/1024) % " КБ свободно");
    disk_free_size_l->setToolTip(reformat_size_2(av_b) % " из " % reformat_size_2(all_b) % " свободно");
}

//изменяет информацию по выбранному правому диску
void MainWindow::size_d_r(QString disk)
{
    for (int i = 0; i < r_l_buttons_disk.size(); ++i) {
        if (r_l_buttons_disk[i]->text() == disk)
            r_l_buttons_disk[i]->setChecked(true);
        else
            r_l_buttons_disk[i]->setChecked(false);
    }

    disk += ":/";

    QStorageInfo st_inf = QStorageInfo(disk);
    qint64 av_b = float(st_inf.bytesAvailable());
    qint64 all_b = float(st_inf.bytesTotal());
    float occup_b = (all_b - av_b)*100.0 / float(all_b);
    if (occup_b < 90.0)
        disk_progress_r->setStyleSheet("QProgressBar::chunk {background-color: rgb(38,160,218);}");
    else
        disk_progress_r->setStyleSheet("QProgressBar::chunk {background-color: rgb(218,38,38);}");
    disk_name_r->setText("[" % st_inf.name() % "]");
    disk_progress_r->setValue(occup_b);
    disk_free_size_r->setText(reformat_size(av_b/1024) % " КБ из " % reformat_size(all_b/1024) % " КБ свободно");
    disk_free_size_r->setToolTip(reformat_size_2(av_b) % " из " % reformat_size_2(all_b) % " свободно");
}


//обновляет виджеты главного экрана
void MainWindow::update_widgets()
{
    int l_ind = treeWidget_l->indexOfTopLevelItem(treeWidget_l->currentItem());
    int r_ind = treeWidget_r->indexOfTopLevelItem(treeWidget_r->currentItem());

    if (QDir(last_path_l).exists()) {
        ui->path_l->setText(last_path_l);
        on_path_l_returnPressed();
        if ((l_ind > -1) && (l_ind < treeWidget_l->topLevelItemCount())) {
            treeWidget_l->setCurrentItem(treeWidget_l->topLevelItem(l_ind));
            treeWidget_l->previous_item_ind = l_ind;
            treeWidget_l->begin_shift = l_ind;
        }
    } else {
        ui->path_l->setText("c:/");
        on_path_l_returnPressed();
    }

    if (QDir(last_path_r).exists()) {
        ui->path_r->setText(last_path_r);
        on_path_r_returnPressed();
        if ((r_ind > -1) && (r_ind < treeWidget_r->topLevelItemCount())) {
            treeWidget_r->setCurrentItem(treeWidget_r->topLevelItem(r_ind));
            treeWidget_r->previous_item_ind = r_ind;
            treeWidget_r->begin_shift = r_ind;
        }
    } else {
        ui->path_r->setText("c:/");
        on_path_r_returnPressed();
    }


}

//изменение левого пути
void MainWindow::on_path_l_returnPressed()
{
    if (ui->path_l->text().isEmpty()) {
        ui->path_l->setText(last_path_l);
        on_path_l_returnPressed();
        return;
    }
    ui->path_l->setText(QDir::cleanPath(ui->path_l->text()));
    QString new_disk = ui->path_l->text().split("/").first();
    ui->path_l->setText(ui->path_l->text().replace(new_disk, new_disk.toLower()));
    new_disk = new_disk.toLower().removeLast();

    if (!ui->path_l->text().endsWith("/")) {
        if (QFile(ui->path_l->text()).exists() && QFileInfo(ui->path_l->text()).isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(ui->path_l->text()));
            ui->path_l->setText(ui->path_l->text().left(ui->path_l->text().lastIndexOf("/")) +"/");
        } else
            ui->path_l->setText(ui->path_l->text() + "/");
    }
    QDir dir(ui->path_l->text());
    if (dir.exists()) {
        size_d_l(new_disk);
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(new_disk));
        treeWidget_l->clear();


        QString last_dir_l = "";
        if (last_path_l.contains(ui->path_l->text())) {
            last_dir_l = last_path_l.remove(ui->path_l->text()).replace("/", "");
        }

        last_path_l = ui->path_l->text();
        treeWidget_l->Fill(ui->path_l->text(), hidden_f, last_dir_l);

        all_l_v = treeWidget_l->all_v;
        all_f_l = treeWidget_l->all_f;
        all_l_v = round(all_l_v / 1024);
        treeWidget_l->path = last_path_l;

        ui->inf_dir_l->setText("0 КБ из " % reformat_size(all_l_v) % " КБ, файлов: 0 из " % QString::number(all_f_l));
    } else {
        v_error("Путь \"" % ui->path_l->text() % "\" не найден.");
        ui->path_l->setText(last_path_l);
    }

    for (auto a : ui->toolButton_favourites_l->actions())
        a->setChecked(false);

    QString fname = QFileInfo(last_path_l).absoluteFilePath();
    for (auto a : ui->toolButton_favourites_l->actions()) {
        if (a->data().toString() == fname) {
            a->setChecked(true);
            act_add_fav_l->setVisible(false);
            act_remove_fav_l->setVisible(true);
            return;
        }
    }
    act_add_fav_l->setVisible(true);
    act_remove_fav_l->setVisible(false);
}

//изменение правого пути
void MainWindow::on_path_r_returnPressed()
{
    if (is_fast_view)
        return;
    if (ui->path_r->text().isEmpty()) {
        ui->path_r->setText(last_path_r);
        on_path_r_returnPressed();
        return;
    }
    ui->path_r->setText(QDir::cleanPath(ui->path_r->text()));
    QString new_disk = ui->path_r->text().split("/").first();
    ui->path_r->setText(ui->path_r->text().replace(new_disk, new_disk.toLower()));
    new_disk = new_disk.toLower().removeLast();

    if (!ui->path_r->text().endsWith("/")) {
        if (QFile(ui->path_r->text()).exists() && QFileInfo(ui->path_r->text()).isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(ui->path_r->text()));
            ui->path_r->setText(ui->path_r->text().left(ui->path_r->text().lastIndexOf("/")) + "/");
        } else
            ui->path_r->setText(ui->path_r->text() + "/");
    }
    QDir dir(ui->path_r->text());
    //if (dir.exists() && ui->path_r->text().endsWith("/")) {
    if (dir.exists()){
        size_d_r(new_disk);
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(new_disk));
        treeWidget_r->clear();

        QString last_dir_r = "";
        if (last_path_r.contains(ui->path_r->text())) {
            last_dir_r = last_path_r.remove(ui->path_r->text()).replace("/", "");
        }

        treeWidget_r->Fill(ui->path_r->text(), hidden_f, last_dir_r);

        all_r_v = treeWidget_r->all_v;
        all_f_r = treeWidget_r->all_f;
        last_path_r = ui->path_r->text();
        treeWidget_r->path = last_path_r;
        all_r_v = round(all_r_v / 1024);

        ui->inf_dir_r->setText("0 КБ из " % reformat_size(all_r_v) % " КБ, файлов: 0 из " % QString::number(all_f_r));
    } else {
        v_error("Путь \"" % ui->path_r->text() % "\" не найден.");
        ui->path_r->setText(last_path_r);
    }

    for (auto a : ui->toolButton_favourites_r->actions())
        a->setChecked(false);

    QString fname = QFileInfo(last_path_r).absoluteFilePath();
    for (auto a : ui->toolButton_favourites_r->actions()) {
        if (a->data().toString() == fname) {
            a->setChecked(true);
            act_add_fav_r->setVisible(false);
            act_remove_fav_r->setVisible(true);
            return;
        }
    }
    act_add_fav_r->setVisible(true);
    act_remove_fav_r->setVisible(false);
}

//клик по шапке левого дерева
void MainWindow::header_clicked_l(int col)
{
    QTreeWidgetItem *hi = treeWidget_l->headerItem();
    if (hi->text(col) == "Имя" or hi->text(col) == "↓Имя") {
        treeWidget_l->index_sort = 0;
        treeWidget_l->is_reverse = false;
        hi->setText(col, "↑Имя");
    } else if (hi->text(col) == "↑Имя") {
        treeWidget_l->index_sort = 0;
        treeWidget_l->is_reverse = true;
        hi->setText(col, "↓Имя");
    } else if (hi->text(col) == "Тип" or hi->text(col) == "↓Тип") {
        treeWidget_l->index_sort = 1;
        treeWidget_l->is_reverse = false;
        hi->setText(col, "↑Тип");
    } else if (hi->text(col) == "↑Тип") {
        treeWidget_l->index_sort = 1;
        treeWidget_l->is_reverse = true;
        hi->setText(col, "↓Тип");
    } else if (hi->text(col) == "Размер" or hi->text(col) == "↑Размер") {
        treeWidget_l->index_sort = 2;
        treeWidget_l->is_reverse = false;
        hi->setText(col, "↓Размер");
    } else if (hi->text(col) == "↓Размер") {
        treeWidget_l->index_sort = 2;
        treeWidget_l->is_reverse = true;
        hi->setText(col, "↑Размер");
    } else if (hi->text(col) == "Дата" or hi->text(col) == "↑Дата") {
        treeWidget_l->index_sort = 3;
        treeWidget_l->is_reverse = false;
        hi->setText(col, "↓Дата");
    } else if (hi->text(col) == "↓Дата") {
        treeWidget_l->index_sort = 3;
        treeWidget_l->is_reverse = true;
        hi->setText(col, "↑Дата");
    }
    for (int i = 0; i < 4; i ++) {
        if (i != col)
            treeWidget_l->headerItem()->setText(i, treeWidget_l->headerItem()->text(i).remove("↑").remove("↓"));
    }
    on_path_l_returnPressed();
}

//клик по шапке правого дерева
void MainWindow::header_clicked_r(int col)
{
    QTreeWidgetItem *hi = treeWidget_r->headerItem();
    if (hi->text(col) == "Имя" or hi->text(col) == "↓Имя") {
        treeWidget_r->index_sort = 0;
        treeWidget_r->is_reverse = false;
        hi->setText(col, "↑Имя");
    } else if (hi->text(col) == "↑Имя") {
        treeWidget_r->index_sort = 0;
        treeWidget_r->is_reverse = true;
        hi->setText(col, "↓Имя");
    } else if (hi->text(col) == "Тип" or hi->text(col) == "↓Тип") {
        treeWidget_r->index_sort = 1;
        treeWidget_r->is_reverse = false;
        hi->setText(col, "↑Тип");
    } else if (hi->text(col) == "↑Тип") {
        treeWidget_r->index_sort = 1;
        treeWidget_r->is_reverse = true;
        hi->setText(col, "↓Тип");
    } else if (hi->text(col) == "Размер" or hi->text(col) == "↑Размер") {
        treeWidget_r->index_sort = 2;
        treeWidget_r->is_reverse = false;
        hi->setText(col, "↓Размер");
    } else if (hi->text(col) == "↓Размер") {
        treeWidget_r->index_sort = 2;
        treeWidget_r->is_reverse = true;
        hi->setText(col, "↑Размер");
    } else if (hi->text(col) == "Дата" or hi->text(col) == "↑Дата") {
        treeWidget_r->index_sort = 3;
        treeWidget_r->is_reverse = false;
        hi->setText(col, "↓Дата");
    } else if (hi->text(col) == "↓Дата") {
        treeWidget_r->index_sort = 3;
        treeWidget_r->is_reverse = true;
        hi->setText(col, "↑Дата");
    }
    for (int i = 0; i < 4; i ++) {
        if (i != col)
            treeWidget_r->headerItem()->setText(i, treeWidget_r->headerItem()->text(i).remove("↑").remove("↓"));
    }
    ui->path_r->setText(last_path_r);
    on_path_r_returnPressed();
}

//двойной клик по файлу/папке
void MainWindow::treeWidget_l_itemActivated(QTreeWidgetItem *item, int column)
{
    QString f_name = item->data(0, Qt::UserRole).toString();
    if (item->text(1) == "<DIR>"){
        if (item->text(0) == "..") {
            QString str1 = last_path_l.left(last_path_l.lastIndexOf("/"));
            str1 = str1.left(str1.lastIndexOf("/"));
            ui->path_l->setText(str1 + "/");
        } else {
            ui->path_l->setText(f_name);
        }
        on_path_l_returnPressed();
    } else if (QFileInfo(f_name).isFile()) {
        if (is_7zz && treeWidget_l->is_arc(f_name)) {
            Archive_tree *arch_viewer = new Archive_tree(f_name, appSettings);
            connect(arch_viewer, SIGNAL(destroyed(QObject*)), this, SLOT(update_widgets()));
            arch_viewer->setWindowModality(Qt::ApplicationModal);
            arch_viewer->show();
        } else {
            QDesktopServices::openUrl(QUrl::fromLocalFile(f_name));
        }
    }
}

//двойной клик по файлу/папке
void MainWindow::treeWidget_r_itemActivated(QTreeWidgetItem *item, int column)
{
    QString f_name = item->data(0, Qt::UserRole).toString();
    if (item->text(1) == "<DIR>"){
        if (item->text(0) == "..") {
            QString str1 = last_path_r.left(last_path_r.lastIndexOf("/"));
            str1 = str1.left(str1.lastIndexOf("/"));
            ui->path_r->setText(str1 + "/");
        } else {
            ui->path_r->setText(f_name);
        }
        on_path_r_returnPressed();
    } else if (QFileInfo(f_name).isFile()) {
        if (is_7zz && treeWidget_r->is_arc(f_name)) {
            Archive_tree *arch_viewer = new Archive_tree(f_name, appSettings);
            connect(arch_viewer, SIGNAL(destroyed(QObject*)), this, SLOT(update_widgets()));
            arch_viewer->setWindowModality(Qt::ApplicationModal);
            arch_viewer->show();
        } else {
            QDesktopServices::openUrl(QUrl::fromLocalFile(f_name));
        }
    }
}

void MainWindow::put_to_clipboard(bool is_move, const QList<QTreeWidgetItem*>& items)
{
    if (is_api) {
        QStringList urls;
        int i = 0;
        if ((items[0]->text(0) == "..") && (items[0]->text(1) == "<DIR>"))
            ++i;
        for (; i < items.length(); ++i) {
            urls << items[i]->data(0, Qt::UserRole).toString();
        }

        if (is_move)
            shellfuncs::cut_api(urls, reinterpret_cast<void*>(winId()));
        else
            shellfuncs::copy_api(urls, reinterpret_cast<void*>(winId()));
    } else {
        auto mimeData = new QMimeData;

        QList<QUrl> urls;
        int i = 0;
        if ((items[0]->text(0) == "..") && (items[0]->text(1) == "<DIR>"))
            ++i;
        for (; i < items.length(); ++i) {
            urls << QUrl::fromLocalFile(items[i]->data(0, Qt::UserRole).toString());
        }
        mimeData->setUrls(urls);

        int dropEffect;
        if (is_move)
            dropEffect = 2;
        else
            dropEffect = 5;

        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << dropEffect;
        mimeData->setData("Preferred DropEffect", data);

        QApplication::clipboard()->setMimeData(mimeData);
    }
}

void MainWindow::ctrl_c_clicked()
{
    TreeFilesWidget *tree;
    if (treeWidget_l->hasFocus())
        tree = treeWidget_l;
    else
        tree = treeWidget_r;

    const QList<QTreeWidgetItem*> &items = tree->selectedItems();
    put_to_clipboard(false, items);
}

void MainWindow::ctrl_x_clicked()
{
    TreeFilesWidget *tree;
    if (treeWidget_l->hasFocus())
        tree = treeWidget_l;
    else
        tree = treeWidget_r;

    const QList<QTreeWidgetItem*> &items = tree->selectedItems();
    put_to_clipboard(true, items);
}

void MainWindow::ctrl_v_clicked()
{
    TreeFilesWidget *tree;
    if (treeWidget_l->hasFocus())
        tree = treeWidget_l;
    else
        tree = treeWidget_r;

    paste_func(tree->path);
}

//контекстное меню левого дерева
void MainWindow::treeWidget_l_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = treeWidget_l->itemAt(pos);
    QPoint globPos = treeWidget_l->viewport()->mapToGlobal(pos);
    if (is_api) {
        if (item == NULL) {
            QStringList t { treeWidget_l->path };
            shellfuncs::get_context_menu(t, round(globPos.x() * ratio), round(globPos.y() * ratio), reinterpret_cast<void*>(winId()));
            return;
        }

        QString dir_to; QStringList selected_dirs, selected_files;
        mass_all_selected(dir_to, selected_dirs, selected_files);

        if (selected_dirs.length() + selected_files.length() > 0) {
            selected_dirs.append(selected_files);
            shellfuncs::get_context_menu(selected_dirs, round(globPos.x() * ratio), round(globPos.y() * ratio), reinterpret_cast<void*>(winId()));
        }
        return;
    }

    if (item == NULL) {
        menu_f4->setVisible(false);
        menu_open->setVisible(false);
        menu_arc_compr->setVisible(false);
        menu_arc_view->setVisible(false);
        menu_arc_decompr->setVisible(false);
        menu_ctrl_c->setVisible(false);
        menu_ctrl_x->setVisible(false);
        menu_ctrl_v->setVisible(true);
        menu_f8->setVisible(false);
        menu_create_file->setVisible(true);
        cust_menu_tree = last_path_l;
        cust_menu->popup(globPos);
        return;
    }
    cust_menu_tree = "";

    QList<QTreeWidgetItem*> list = treeWidget_l->selectedItems();
    if (!list.contains(item)) {
        list.clear();
        treeWidget_l->unselected_all();
        treeWidget_l->change_select(treeWidget_l->indexOfTopLevelItem(item), true);
        list.push_back(item);
    }

    if (list.length() >= 1) {
        menu_arc_compr->setVisible(true);
        menu_arc_view->setVisible(false);
        menu_arc_decompr->setVisible(false);
        menu_ctrl_c->setVisible(true);
        menu_ctrl_x->setVisible(true);
        menu_ctrl_v->setVisible(false);
        menu_f8->setVisible(true);
        menu_create_file->setVisible(false);

        if (list.length() == 1) {
            menu_f4->setVisible(true);
            if (item->text(1) != "<DIR>") {
                menu_open->setVisible(true);
                if (treeWidget_l->is_arc(list.first()->data(0, Qt::UserRole).toString())) {
                    menu_arc_compr->setVisible(false);
                    menu_arc_view->setVisible(true);
                    menu_arc_decompr->setVisible(true);
                }
            } else
                menu_open->setVisible(false);
        } else {
            menu_f4->setVisible(false);
        }
        cust_menu->popup(globPos);
    }
}

//контекстное меню правого дерева
void MainWindow::treeWidget_r_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = treeWidget_r->itemAt(pos);
    QPoint globPos = treeWidget_r->viewport()->mapToGlobal(pos);
    if (is_api) {
        if (item == NULL) {
            QStringList t { treeWidget_r->path };
            shellfuncs::get_context_menu(t, round(globPos.x() * ratio), round(globPos.y() * ratio), reinterpret_cast<void*>(winId()));
            return;
        }

        QString dir_to; QStringList selected_dirs, selected_files;
        mass_all_selected(dir_to, selected_dirs, selected_files);

        if (selected_dirs.length() + selected_files.length() > 0) {
            selected_dirs.append(selected_files);
            shellfuncs::get_context_menu(selected_dirs, round(globPos.x() * ratio), round(globPos.y() * ratio), reinterpret_cast<void*>(winId()));
        }
        return;
    }

    if (item == NULL) {
        menu_f4->setVisible(false);
        menu_open->setVisible(false);
        menu_arc_compr->setVisible(false);
        menu_arc_view->setVisible(false);
        menu_arc_decompr->setVisible(false);
        menu_ctrl_c->setVisible(false);
        menu_ctrl_x->setVisible(false);
        menu_ctrl_v->setVisible(true);
        menu_f8->setVisible(false);
        menu_create_file->setVisible(true);
        cust_menu_tree = last_path_r;
        cust_menu->popup(globPos);
        return;
    }
    cust_menu_tree = "";

    QList<QTreeWidgetItem*> list = treeWidget_r->selectedItems();
    if (!list.contains(item)) {
        list.clear();
        treeWidget_r->unselected_all();
        treeWidget_r->change_select(treeWidget_r->indexOfTopLevelItem(item), true);
        list.push_back(item);
    }

    if (list.length() >= 1) {
        menu_arc_compr->setVisible(true);
        menu_arc_view->setVisible(false);
        menu_arc_decompr->setVisible(false);
        menu_ctrl_c->setVisible(true);
        menu_ctrl_x->setVisible(true);
        menu_ctrl_v->setVisible(false);
        menu_f8->setVisible(true);
        menu_create_file->setVisible(false);

        if (list.length() == 1) {
            menu_f4->setVisible(true);
            if (item->text(1) != "<DIR>") {
                menu_open->setVisible(true);
                if (treeWidget_r->is_arc(list.first()->data(0, Qt::UserRole).toString())) {
                    menu_arc_compr->setVisible(false);
                    menu_arc_view->setVisible(true);
                    menu_arc_decompr->setVisible(true);
                }
            } else
                menu_open->setVisible(false);
        } else {
            menu_f4->setVisible(false);
        }
        cust_menu->popup(globPos);
    }
}

//срабатывает при изменении выбора строк
void MainWindow::treeWidget_l_itemSelectionChanged()
{
    float now_v_l = 0;
    int now_f = 0;
    if (treeWidget_l->count_selected() > 0) {
        QList<QTreeWidgetItem*> list = treeWidget_l->selectedItems();
        for (int i = 0; i < list.length(); i++) {
            if (list[i]->text(2) != "") {
                now_v_l += list[i]->text(2).remove(" ").toFloat();
                now_f += 1;
            }
        }
        now_v_l = round(now_v_l / 1024);
    }

    ui->inf_dir_l->setText(reformat_size(now_v_l) % " КБ из " % reformat_size(all_l_v)
                          % " КБ, файлов: " % QString::number(now_f) % " из " % QString::number(all_f_l));
}

//срабатывает при изменении выбора строк
void MainWindow::treeWidget_r_itemSelectionChanged()
{
    float now_v_r = 0;
    int now_f = 0;
    if (treeWidget_r->count_selected() > 0) {
        QList<QTreeWidgetItem*> list = treeWidget_r->selectedItems();
        for (int i = 0; i < list.length(); i ++) {
            if (list[i]->text(2) != "") {
                now_v_r += list[i]->text(2).remove(" ").toFloat();
                now_f += 1;
            }
        }
        now_v_r = round(now_v_r / 1024);
    }
    ui->inf_dir_r->setText(reformat_size(now_v_r) % " КБ из " % reformat_size(all_r_v)
                           % " КБ, файлов: " % QString::number(now_f) % " из " % QString::number(all_f_r));

}



//drop файла в указанную директорию
void MainWindow::drop_func(QStringList lst, QString dir_to, bool remove_after)
{
    if (lst.length() < 1)
        return;

    if (QFileInfo(lst.first()).absoluteDir() == QDir(dir_to))
        return;

    QStringList selected_dirs, selected_files;
    for (int i = 0; i < lst.length(); ++i) {
        if (QFileInfo(lst[i]).isFile())
            selected_files << lst[i].replace("\\", "/");
        else if(QFileInfo(lst[i]).isDir())
            selected_dirs << lst[i].replace("\\", "/");
    }

    if (selected_dirs.length() + selected_files.length() > 0) {
        Copy_files *cp = new Copy_files(appSettings);
        connect(cp, SIGNAL(end_operation()), this, SLOT(end_operation()));
        cp->Work(dir_to, selected_dirs, selected_files, remove_after);
        count_proc++;
    }
}

void MainWindow::paste_func(QString dir_to)
{
    if (is_api) {
        count_proc++;
        QThread *th = new QThread(this);
        PasteProcess *pp = new PasteProcess(dir_to, reinterpret_cast<void*>(winId()));
        connect(th, &QThread::started, pp, &PasteProcess::Work);
        connect(th, &QThread::finished, this, &MainWindow::end_operation);
        pp->moveToThread(th);
        th->start();
    } else {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        if (mimeData->hasUrls()) {
            QByteArray output_data = mimeData->data("Preferred DropEffect");

            int dropEffect = 2;

            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream << dropEffect;

            QStringList pathList;
            QList<QUrl> urlList = mimeData->urls();

            for (int i = 0; i < urlList.size();++i)
            {
                pathList.append(urlList.at(i).toLocalFile());
            }
            drop_func(pathList, dir_to, (data == output_data));
            return;
        }
    }
}


//изменяет по ссылке папки назначения, выбранные каталоги, выбранные файлы
void MainWindow::mass_all_selected(QString& dir_to, QStringList& selected_dirs, QStringList& selected_files)
{
    QList<QTreeWidgetItem*> list;
    if (treeWidget_l->hasFocus()) {
        list = treeWidget_l->selectedItems();
        dir_to = last_path_r;
    } else  if (treeWidget_r->hasFocus()) {
        list = treeWidget_r->selectedItems();
        dir_to = last_path_l;
    }
    for (int i = 0; i < list.size(); ++i) {
        if (list[i]->text(1) == "<DIR>" && list[i]->text(0) == "..")
            continue;
        if (list[i]->text(1) == "<DIR>") {
            selected_dirs << list[i]->data(0, Qt::UserRole).toString();
        } else {
            selected_files << list[i]->data(0, Qt::UserRole).toString();
        }
    }
}


//переименование файла/каталога
void MainWindow::on_pushButton_f4_clicked()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);

    if (selected_files.length() + selected_dirs.length() == 1) {
        QString path_new;
        if (dir_to == last_path_l)
            path_new = last_path_r;
        else
            path_new = last_path_l;

        bool flag_dir = false;

        QInputDialog id;
        id.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
        id.setFont(main_font);
        id.resize(QSize(400, 60));
        id.setCancelButtonText("Отмена");
        id.setLabelText("Переименование:");

        QString past_name;
        QString new_name;

        if (selected_dirs.length() == 1) {
            past_name = selected_dirs.first().split("/").last();
            flag_dir = true;
        }
        else {
            past_name = selected_files.first().split("/").last();
        }

        id.setTextValue(past_name);


        /*QByteArray t_arr = id.textValue().toLatin1();
        for (int i = 0; i < t_arr.size(); ++i) {
            if (t_arr[i] < 31)
                v_error("недопустимое название");
        }*/

        QList<QChar> ban_symb { '<', '>', ':', '"', '/', '\\', '|', '?', '*'};
        bool flag = false;

        while (id.exec()) {
            new_name = id.textValue();
            if (new_name.size() > 260) {
                v_error("Имя файла ограничено 260 символами");
                continue;
            }

            while (new_name.endsWith(".") || new_name.endsWith(" "))
                new_name.removeLast();
            if (new_name.size() < 1) {
                continue;
            }
            for (int i = 0; i < new_name.size(); ++i) {
                if (ban_symb.contains(new_name[i])) {
                    v_error("В названии не могут содержаться знаки: '<, >, :, \", /, \\, |, ?, *");
                    flag = true;
                    break;
                }
            }

            if (flag) {
                flag = false;
                continue;
            }

            if (past_name == new_name) {
                update_widgets();
                break;
            }
            if (flag_dir && QDir(path_new + new_name).exists()) {
                v_error("Папка с именем " % new_name % " уже существует.");
            } else if (!flag_dir && QFile(path_new + new_name).exists()) {
                v_error("Файл с именем " % new_name % " уже существует.");
            } else {
                if (QFileInfo(path_new  % past_name).isFile()) {
                    QFile file(path_new  % past_name);
                    if (!file.rename(path_new  % new_name))
                        v_error("Не удалось переименовать!\n\n" % file.errorString());
                } else if (!(QDir(path_new).rename(past_name, new_name)))
                    v_error("Не удалось переименовать!");
                update_widgets();
                break;
            }
        }
    }
}

void MainWindow::view_archive()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);
    if (selected_files.length() != 1)
        return;
    Archive_tree *arch_viewer = new Archive_tree(selected_files[0], appSettings);
    connect(arch_viewer, SIGNAL(destroyed(QObject*)), this, SLOT(update_widgets()));
    arch_viewer->setWindowModality(Qt::ApplicationModal);
    arch_viewer->show();
}

void MainWindow::compress_archive()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);
    if (selected_files.size() + selected_dirs.size() < 1)
        return;

    QString arc_type;
    QString arc_name, path_new;

    if (selected_dirs.size() == 1 && selected_files.size() == 0) {
        arc_name = selected_dirs.first().last(selected_dirs.first().size() - selected_dirs.first().lastIndexOf('/') - 1);
        path_new = selected_dirs.first().left(selected_dirs.first().lastIndexOf('/'));
    }
    else if (selected_dirs.size() == 0 && selected_files.size() == 1) {
        arc_name = selected_files.first().last(selected_files.first().size() - selected_files.first().lastIndexOf('/') - 1);
        arc_name = arc_name.left(arc_name.lastIndexOf('.'));
        path_new = selected_files.first().left(selected_files.first().lastIndexOf('/'));
    } else {
        QStringList t_list;
        if (selected_dirs.size() > 0) {
            t_list = selected_dirs.first().split('/');
            path_new = selected_dirs.first().left(selected_dirs.first().lastIndexOf('/'));
        } else {
            t_list = selected_files.first().split('/');
            t_list.last() = t_list.last().left(t_list.last().lastIndexOf('.'));
            path_new = selected_files.first().left(selected_files.first().lastIndexOf('/'));
        }
        if (t_list.size() == 2)
            arc_name = t_list[1];
        else
            arc_name = t_list[t_list.size() - 2];
    }
    path_new.replace('/', '\\') += '\\';

    QDialog d;
    d.setWindowModality(Qt::ApplicationModal);
    d.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    d.setWindowTitle("Создание архива");
    d.setFont(dialog_font);
    d.resize(round(600*w), round(200*h));
    QGridLayout vbox(&d);
    QComboBox comboBox_type;
    comboBox_type.addItems({"zip", "7z", "tar"});
    comboBox_type.setCurrentIndex(0);
    QComboBox comboBox_ratio;
    comboBox_ratio.addItems({"1", "2", "3", "4", "5", "6", "7", "8", "9"});
    comboBox_ratio.setCurrentIndex(5);
    QLineEdit lineEdit_name;

    QDialogButtonBox buttonBox = QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QObject::connect(&buttonBox, SIGNAL(accepted()), &d, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &d, SLOT(reject()));

    QLabel lab_name("Имя");
    QLabel lab_type("Тип");
    QLabel lab_ratio("Степень\nсжатия");

    vbox.addWidget(&lineEdit_name, 0, 1);
    vbox.addWidget(&comboBox_type, 1, 1);
    vbox.addWidget(&comboBox_ratio, 2, 1);
    vbox.addWidget(&buttonBox, 3, 1);

    vbox.addWidget(&lab_name, 0, 0);
    vbox.addWidget(&lab_type, 1, 0);
    vbox.addWidget(&lab_ratio, 2, 0);

    vbox.setColumnStretch(0, 0);
    vbox.setColumnStretch(1, 1);

    d.setLayout(&vbox);

    lineEdit_name.setText(arc_name);
    QList<QChar> ban_symb { '<', '>', ':', '"', '/', '\\', '|', '?', '*'};
    bool flag = false;

    while (d.exec()) {
        arc_name = lineEdit_name.text();
        arc_type = comboBox_type.currentText();
        if (arc_name.size() > 256) {
            v_error("Имя файла ограничено 260 символами");
            continue;
        }

        for (int i = 0; i < arc_name.size(); ++i) {
            if (ban_symb.contains(arc_name[i])) {
                v_error("В названии не могут содержаться знаки: '<, >, :, \", /, \\, |, ?, *");
                flag = true;
                break;
            }
        }

        if (flag) {
            flag = false;
            continue;
        }
        if (QFile(path_new % arc_name % '.' % arc_type).exists()) {
            v_error("Файл с именем " % arc_name % '.' % arc_type % " уже существует.");
            continue;
        }


        QString comnd = " a -t" % arc_type % " -mx" % comboBox_ratio.currentText() % " \"" % path_new % arc_name % "." % arc_type % "\" ";

        for (int i = 0; i < selected_dirs.size(); ++i) {
            comnd = comnd % " \"" % selected_dirs[i].replace('/', '\\') % "\"";
        }
        for (int i = 0; i < selected_files.size(); ++i) {
            comnd = comnd % " \"" % selected_files[i].replace('/', '\\') % "\"";
        }
        wchar_t *c_str2 = new wchar_t[comnd.length()+1]; comnd.toWCharArray(c_str2);
        c_str2[comnd.length()] = 0;
        ShellExecute(NULL, L"open", L"7zz.exe", c_str2, 0, SW_SHOWNORMAL);
        break;
    }
    Sleep(1000);
    update_widgets();
}

void MainWindow::decompress_archive()
{
    QStringList selected_files;
    {
        QString dir_to; QStringList selected_dirs;
        mass_all_selected(dir_to, selected_dirs, selected_files);
        if (selected_files.length() != 1 || !treeWidget_l->is_arc(selected_files.first()))
            return;
    }

    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::Directory);
    fd.setDirectory(QDir(selected_files[0].left(selected_files[0].lastIndexOf('/'))));
    if (!fd.exec() || fd.selectedFiles().size() != 1)
        return;
    QString new_path = fd.selectedFiles().at(0);

    QString comnd = "x \"" % selected_files[0].replace('/', '\\') % "\" \"-o" % new_path % "\"";

    wchar_t *c_str2 = new wchar_t[comnd.length()+1]; comnd.toWCharArray(c_str2);
    c_str2[comnd.length()] = 0;
    ShellExecute(NULL, L"open", L"7zz.exe", c_str2, 0, SW_SHOWNORMAL);
}

//копировать как путь
void MainWindow::copy_as_path_clicked()
{
    QString res;

    if (cust_menu_tree.isEmpty() || !QDir(cust_menu_tree).exists()) {
        QString dir_to; QStringList selected_dirs, selected_files;
        mass_all_selected(dir_to, selected_dirs, selected_files);
        for (int i = 0; i < selected_dirs.length(); ++i) {
            res.append("\"" % selected_dirs[i] % "\"\n");
        }
        for (int i = 0; i < selected_files.length(); ++i) {
            res.append("\"" % selected_files[i] % "\"\n");
        }

    } else
        res = "\"" % cust_menu_tree % "\"\n";
    cust_menu_tree = "";

    if (res.length() > 0)
        QApplication::clipboard()->setText(res.removeLast().replace("/", "\\"));
}


//срабытывает при завершении операции с файлами (обновляет виджеты)
void MainWindow::end_operation()
{
    update_widgets();
    count_proc--;
}

//копирование/перемещение
void MainWindow::f5_f6_func(bool remove_after)
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);

    if (selected_dirs.length() + selected_files.length() > 0) {
        Copy_files *cp = new Copy_files(appSettings);
        connect(cp, SIGNAL(end_operation()), this, SLOT(end_operation()));
        cp->Work(dir_to, selected_dirs, selected_files, remove_after);
        count_proc++;
    }
}

//копирование
void MainWindow::on_pushButton_f5_clicked() {
    if (is_fast_view)
        return;
    f5_f6_func(false);
}

//перемещение
void MainWindow::on_pushButton_f6_clicked()
{
    if (is_fast_view)
        return;
    f5_f6_func(true);
}

//создание каталога
void MainWindow::on_pushButton_f7_clicked()
{
    QString path_new;
    if (treeWidget_l->hasFocus()) {
        path_new = last_path_l;
    } else if (treeWidget_r->hasFocus()) {
        path_new = last_path_r;
    } else {
        return;
    }

    QInputDialog id;
    id.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    id.setFont(dialog_font);
    id.resize(QSize(400, 60));
    id.setCancelButtonText("Отмена");
    id.setLabelText("Создать новый каталог:");

    QString new_name;
    QList<QChar> ban_symb { '<', '>', ':', '"', '/', '\\', '|', '?', '*'};
    bool flag = false;

    while (id.exec()) {
        new_name = id.textValue();
        if (new_name.size() > 260) {
            v_error("Имя папки ограничено 260 символами");
            continue;
        }

        while (new_name.endsWith(".") || new_name.endsWith(" "))
            new_name.removeLast();
        if (new_name.size() < 1) {
            continue;
        }
        for (int i = 0; i < new_name.size(); ++i) {
            if (ban_symb.contains(new_name[i])) {
                v_error("В названии не могут содержаться знаки: '<, >, :, \", /, \\, |, ?, *");
                flag = true;
                break;
            }
        }

        if (flag) {
            flag = false;
            continue;
        }

        if (QDir(path_new + new_name).exists()) {
            v_error("Папка с именем " % new_name % " уже существует.");
        } else {
            QDir dir_n(path_new + new_name);
            if (!dir_n.mkpath("."))
                v_error("Не удалось создать каталог!");
            update_widgets();
            break;
        }
    }
}

//удаление файла/каталога
void MainWindow::on_pushButton_f8_clicked()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);

    if (selected_dirs.length() + selected_files.length() > 0) {
        Delete_Files *df = new Delete_Files(appSettings);
        connect(df, SIGNAL(end_operation()), this, SLOT(end_operation()));
        df->Work(selected_dirs, selected_files, false);
        count_proc++;
    }
    update_widgets();
}

//безвозвратное удаление файла/каталога
void MainWindow::shift_del_f()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);
    if (selected_dirs.length() + selected_files.length() > 0) {
        Delete_Files *df = new Delete_Files(appSettings);
        connect(df, SIGNAL(end_operation()), this, SLOT(end_operation()));
        df->Work(selected_dirs, selected_files, true);
        count_proc++;
    }
}

//отображение свойств файла/папки (файлов/папок)
void MainWindow::show_properties()
{
    QStringList selected_files;

    if (cust_menu_tree.isEmpty() || !QDir(cust_menu_tree).exists()) {
        QString dir_to; QStringList selected_dirs;
        mass_all_selected(dir_to, selected_dirs, selected_files);
        selected_files.append(selected_dirs);
    } else
        selected_files.append(cust_menu_tree);
    cust_menu_tree = "";

    if (selected_files.length() > 0) {
        shellfuncs::showProperties(selected_files);
    }
}

//отображение/скрытие скрытых файлов
void MainWindow::show_hidden_func()
{
    if (hidden_f) {
        hidden_f = false;
        ui->pushButton_hidden_f->setChecked(false);
    } else {
        hidden_f = true;
        ui->pushButton_hidden_f->setChecked(true);
    }
    on_path_l_returnPressed();
    on_path_r_returnPressed();
}

//вызывает окно открыть с помощью...
void MainWindow::menu_open_with_wind()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);
    if (selected_files.length() == 1) {
        OPENASINFO info = {0};
        info.pcszFile = (const wchar_t*) selected_files.first().replace("/", "\\").utf16();
        info.pcszClass = NULL;
        info.oaifInFlags = OAIF_ALLOW_REGISTRATION | OAIF_EXEC;
        SHOpenWithDialog(NULL, &info);
    }
}

//открывает проводник в активной директории
void MainWindow::on_pushButton_open_in_exp_clicked()
{
    QString nam_dir;
    if (treeWidget_l->hasFocus())
        nam_dir = last_path_l;
    else
        nam_dir = last_path_r;
    QDesktopServices::openUrl(QUrl::fromLocalFile(nam_dir));
}

//создает новый файл в активной папке
void MainWindow::on_pushButton_create_file_clicked()
{
    QString path_new;
    if (treeWidget_l->hasFocus()) {
        path_new = last_path_l;
    } else if (treeWidget_r->hasFocus()) {
        path_new = last_path_r;
    } else {
        return;
    }


    QInputDialog id;
    id.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    id.setFont(dialog_font);
    id.resize(QSize(400, 60));
    id.setCancelButtonText("Отмена");
    id.setLabelText("Создать новый файл:");

    QString new_name;
    QList<QChar> ban_symb { '<', '>', ':', '"', '/', '\\', '|', '?', '*'};
    bool flag = false;

    while (id.exec()) {
        new_name = id.textValue();
        if (new_name.size() > 260) {
            v_error("Имя файла ограничено 260 символами");
            continue;
        }

        while (new_name.endsWith(".") || new_name.endsWith(" "))
            new_name.removeLast();
        if (new_name.size() < 1) {
            continue;
        }
        for (int i = 0; i < new_name.size(); ++i) {
            if (ban_symb.contains(new_name[i])) {
                v_error("В названии не могут содержаться знаки: '<, >, :, \", /, \\, |, ?, *");
                flag = true;
                break;
            }
        }

        if (flag) {
            flag = false;
            continue;
        }

        if (QFile(path_new + new_name).exists()) {
            v_error("Файл с именем " % new_name % " уже существует.");
        } else {
            QFile file(path_new + new_name);
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
                update_widgets();
                break;
            } else
                v_error("Не удалось создать файл!");
            update_widgets();
            break;
        }
    }
}

//открывает блокнот
void MainWindow::on_pushButton_notepad_clicked()
{
    if (QFile("C:\\Windows\\notepad.exe").exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile("C:\\Windows\\notepad.exe"));
}


void MainWindow::on_pushButton_admin_clicked()
{
    if(IsUserAnAdmin()) {
        ui->pushButton_admin->setEnabled(false);
        ui->pushButton_admin->setCheckable(false);
    } else if (count_proc > 0) {
        v_error("Не все процессы еще завершены!");
    } else if ((int)(size_t)ShellExecute(NULL, L"runas", L"qFileCommander.exe", 0, 0, SW_SHOWNORMAL) > 32) {
        //>32 - значит даны права админа
        //передать данные и
        close();
    }
}

//просмотр файла
void MainWindow::on_pushButton_f3_clicked()
{
    QStringList selected_dirs; QStringList selected_files;
    {
        QString dir_to;
        mass_all_selected(dir_to, selected_dirs, selected_files);
    }
    for (int i = 0; i < selected_dirs.length(); ++i) {
        Lister *lister = new Lister(selected_dirs[i], appSettings);
        lister_list.push_back(lister);
        connect(lister, &Lister::closed, this, [lister, this](){lister_list.remove(lister);});
        lister->show();
    }
    for (int i = 0; i < selected_files.length(); ++i) {
        Lister *lister = new Lister(selected_files[i], appSettings);
        lister_list.push_back(lister);
        connect(lister, &Lister::closed, this, [lister, this](){lister_list.remove(lister);});
        lister->show();
    }
}

//поиск файла
void MainWindow::on_pushButton_find_clicked()
{
    if (treeWidget_l->hasFocus()) {
        find_lr = true;
        find_wid->set_dir(last_path_l);
    } else {
        find_lr = false;
        find_wid->set_dir(last_path_r);
    }
    find_wid->show();
}

//открывает директорию найденного файла
void MainWindow::open_find_fid(QString f_name)
{
    f_name = f_name.replace("/", "\\");
    f_name = f_name.left(f_name.lastIndexOf("\\")) + "\\";
    if (find_lr) {
        ui->path_l->setText(f_name);
        on_path_l_returnPressed();
    } else {
        ui->path_r->setText(f_name);
        on_path_r_returnPressed();
    }
    MainWindow::activateWindow();
}

//групповое переименование
void MainWindow::on_pushButton_mass_rename_clicked()
{
    QString dir_to; QStringList selected_dirs, selected_files;
    mass_all_selected(dir_to, selected_dirs, selected_files);

    if (selected_dirs.length() + selected_files.length() > 0) {
        if (selected_dirs.length() > 0)
            dir_to = selected_dirs[0].left(selected_dirs[0].lastIndexOf("/")) + "/";
        else
            dir_to = selected_files[0].left(selected_files[0].lastIndexOf("/")) + "/";
        Rename_Widget *rw = new Rename_Widget(appSettings, this);
        rw->Fill(dir_to, selected_dirs, selected_files);
        rw->setWindowModality(Qt::WindowModal);
        rw->show();
        connect(rw, SIGNAL(end_operation()), this, SLOT(end_operation()));
        count_proc++;
    }
}


