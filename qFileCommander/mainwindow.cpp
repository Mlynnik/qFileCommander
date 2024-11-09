#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "helperfunctions.h"
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QKeyEvent>
#include <QStorageInfo>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    treeWidget_l = new TreeFilesWidget(this);
    treeWidget_r = new TreeFilesWidget(this);

    MainWindow::setGeometry(0, 0, round(w*1536), round(h*800));
    MainWindow::showMaximized();
    w_max = MainWindow::width();
    h_max = MainWindow::height();
    MainWindow::setWindowIcon(QIcon("appIcon.png"));
    //MainWindow::setWindowIcon(QIcon(style()->standardIcon(QStyle::SP_DirIcon)));
    MainWindow::setWindowTitle("qFileCommander");

    QSettings settings;
    last_path_l = settings.value("/Settings/L_Path", "").toString();
    last_path_r = settings.value("/Settings/R_Path", "").toString();
    hidden_f = settings.value("/Settings/Hidden_F", false).toBool();

    main_font.fromString(settings.value("/Settings/Main_Font", "Times New Roman,12,-1,5,700,0,0,0,0,0,0,0,0,0,0,1").toString());

    MainWindow::setFont(main_font);

    //левое дерево
    treeWidget_l->setObjectName("treeWidget_l");
    treeWidget_l->setGeometry(QRect(5, 100, 705, 580));
    treeWidget_l->setMouseTracking(true);
    QTreeWidgetItem *___qtreewidgetitem = treeWidget_l->headerItem();
    ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "\320\224\320\260\321\202\320\260", nullptr));
    ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "\320\240\320\260\320\267\320\274\320\265\321\200", nullptr));
    ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "\320\242\320\270\320\277", nullptr));
    ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "\320\230\320\274\321\217", nullptr));

    treeWidget_l->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_l->setDragDropMode(QAbstractItemView::DragDrop);
    treeWidget_l->setRootIsDecorated(false);
    treeWidget_l->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget_l->header()->setStyleSheet("QHeaderView::section {background: rgb(240,240,240);};");
    treeWidget_l->header()->setSectionsMovable(false);
    treeWidget_l->header()->setSectionsClickable(true);

    //правое дерево
    treeWidget_r->setObjectName("treeWidget_r");
    treeWidget_r->setGeometry(QRect(715, 100, 705, 580));
    QTreeWidgetItem *___qtreewidgetitem1 = treeWidget_r->headerItem();
    ___qtreewidgetitem1->setText(3, QCoreApplication::translate("MainWindow", "\320\224\320\260\321\202\320\260", nullptr));
    ___qtreewidgetitem1->setText(2, QCoreApplication::translate("MainWindow", "\320\240\320\260\320\267\320\274\320\265\321\200", nullptr));
    ___qtreewidgetitem1->setText(1, QCoreApplication::translate("MainWindow", "\320\242\320\270\320\277", nullptr));
    ___qtreewidgetitem1->setText(0, QCoreApplication::translate("MainWindow", "\320\230\320\274\321\217", nullptr));

    treeWidget_r->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget_r->setDragDropMode(QAbstractItemView::DragDrop);
    treeWidget_r->setRootIsDecorated(false);
    treeWidget_r->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget_r->header()->setStyleSheet("QHeaderView::section {background: rgb(240,240,240);};");
    treeWidget_r->header()->setSectionsMovable(false);
    treeWidget_r->header()->setSectionsClickable(true);

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

    //пути
    ui->path_l->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    ui->path_l->setFocusPolicy(Qt::ClickFocus);
    ui->path_r->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    ui->path_r->setFocusPolicy(Qt::ClickFocus);

    //если в реестре ошибка, то замена на c:\\
    if (last_path_l == "" || !QDir().exists(last_path_l))
        last_path_l = "c:\\";
    if (last_path_r == "" || !QDir().exists(last_path_r))
        last_path_r = "c:\\";

    ui->path_l->setText(last_path_l);
    ui->path_r->setText(last_path_r);


    find_disk();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(find_disk()));

    //TODO header
    on_path_l_returnPressed();
    on_path_r_returnPressed();
    timer->start(3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    float w_now = float(w) * float(MainWindow::width()) / float(w_max);
    float h_now = float(h) * float(MainWindow::height()) / float(h_max);
    treeWidget_l->setGeometry(round(w_now*1), round(h*80), round(w_now*765), round(h_now*665 - (h-h_now)*140));
    treeWidget_l->header()->resizeSection(0, round(w_now*435));
    treeWidget_l->header()->resizeSection(1, round(w_now*65));
    treeWidget_l->header()->resizeSection(2, round(w_now*120));
    treeWidget_l->header()->resizeSection(3, round(w_now*125));
    treeWidget_r->setGeometry(round(w_now*770), round(h*80), round(w_now*765), round(h_now*665 - (h-h_now)*140));
    treeWidget_r->header()->resizeSection(0, round(w_now*430));
    treeWidget_r->header()->resizeSection(1, round(w_now*65));
    treeWidget_r->header()->resizeSection(2, round(w_now*120));
    treeWidget_r->header()->resizeSection(3, round(w_now*125));
    ui->horizontalLayoutWidget->setGeometry(round(w_now*1), 0, round(w_now*325), round(h*20));
    ui->horizontalLayoutWidget_2->setGeometry(round(w_now*770), 0, round(w_now*325), round(h*20));
    ui->pushButton_create_file->setGeometry(round(w_now*1385 - (w-w_now)*150), 0, round(w*25), round(h*25));
    ui->pushButton_notepad->setGeometry(round(w_now*1410 - (w-w_now)*125), 0, round(w*25), round(h*25));
    ui->pushButton_open_in_exp->setGeometry(round(w_now*1435 - (w-w_now)*100), 0, round(w*25), round(h*25));
    ui->pushButton_find->setGeometry(round(w_now*1460 - (w-w_now)*75), 0, round(w*25), round(h*25));
    ui->pushButton_admin->setGeometry(round(w_now*1485 - (w-w_now)*50), 0, round(w*25), round(h*25));
    ui->toolButton->setGeometry(round(w_now*1510 - (w-w_now)*25), 0, round(w*25), round(h*25));
    ui->line->setGeometry(round(w_now*-5), round(h*24), round(w_now*1540), round(h*2));
    ui->line_2->setGeometry(round(w_now*-5), round(h*52), round(w_now*1540), round(h*2));
    ui->line_3->setGeometry(round(w_now*-5), round(h_now*770 - (h-h_now)*35), round(w_now*1540), round(h*2));
    ui->horizontalLayoutWidget_4->setGeometry(round(w_now*1), round(h*24), round(w_now*750), round(h*27));
    disk_progress_l->setMinimumWidth(round(w_now*200));
    disk_progress_l->setMaximumWidth(round(w_now*200));
    disk_progress_l->setMaximumHeight(round(h*15));
    ui->horizontalLayoutWidget_5->setGeometry(round(w_now*770), round(h*24), round(w_now*750), round(h*27));
    disk_progress_r->setMinimumWidth(round(w_now*200));
    disk_progress_r->setMaximumWidth(round(w_now*200));
    disk_progress_r->setMaximumHeight(round(h*15));
    ui->path_l->setGeometry(round(w_now*-1), round(h*57), round(w_now*767), round(h*24));
    ui->path_r->setGeometry(round(w_now*768), round(h*57), round(w_now*767), round(h*24));
    ui->inf_dir_l->setGeometry(round(w_now*1), round(h_now*745 - (h-h_now)*60), round(w_now*650), round(h*22));
    ui->inf_dir_r->setGeometry(round(w_now*770), round(h_now*745 - (h-h_now)*60), round(w_now*650), round(h*22));
    ui->horizontalLayoutWidget_3->setGeometry(round(w_now*1), round(h_now*770 - (h-h_now)*35), round(w_now*1535), round(h*35));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    QSettings settings;
    settings.setValue("/Settings/L_Path", last_path_l);
    settings.setValue("/Settings/R_Path", last_path_r);
    settings.setValue("/Settings/Main_Font", main_font.toString());
    settings.setValue("/Settings/Hidden_F", hidden_f);

    event->accept();
}

//вызвает окно ошибки с переданным текстом
void MainWindow::v_error(QString str_error) {
    QMessageBox v_err;
    v_err.setFont(main_font);
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
        md_old = mass_disk;
        QStringList list;
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
            btn_disk_l_t->setFocusPolicy(Qt::NoFocus);
            btn_disk_l_t->setMaximumWidth(round(w*30));
            btn_disk_l_t->setIcon(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)));
            btn_disk_l_t->setIconSize(QSize(round(w*12), round(h*12)));
            btn_disk_l_t->setText(list[i]);
            connect(btn_disk_l_t, SIGNAL(clicked()), this, SLOT(cl_on_disk_l()));
            ui->horizontalLayout_l->addWidget(btn_disk_l_t);

            combobox_disk_r->addItem(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)), list[i]);
            if (last_path_r.startsWith(list[i]))
                combobox_disk_r->setCurrentIndex(i);
            QPushButton *btn_disk_r_t = new QPushButton(this);
            btn_disk_r_t->setFocusPolicy(Qt::NoFocus);
            btn_disk_r_t->setMaximumWidth(round(w*30));
            btn_disk_r_t->setIcon(QIcon(style()->standardIcon(QStyle::SP_DriveCDIcon)));
            btn_disk_r_t->setIconSize(QSize(round(w*12), round(h*12)));
            btn_disk_r_t->setText(list[i]);
            connect(btn_disk_r_t, SIGNAL(clicked()), this, SLOT(cl_on_disk_r()));
            ui->horizontalLayout_r->addWidget(btn_disk_r_t);
        }
        if (!list.contains(last_path_l.split("\\").first().toLower().remove(":"))) {
            v_error("Устройство было извлечено!");
            ui->path_l->setText("c:\\");
            on_path_l_returnPressed();
        }
        if (!list.contains(last_path_r.split("\\").first().toLower().remove(":"))) {
            v_error("Устройство было извлечено!");
            ui->path_r->setText("c:\\");
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
        ui->path_l->setText(button->text() + ":\\");
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(button->text()));
    }
    on_path_l_returnPressed();
    size_d_l(button->text() + ":\\");
}

//нажатие на кнопку диска справа
void MainWindow::cl_on_disk_r(){
    QPushButton *button = (QPushButton *)sender();
    if (last_path_l.startsWith(button->text())) {
        ui->path_r->setText(last_path_l);
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(button->text()));
    }
    else if(!last_path_r.startsWith(button->text())) {
        ui->path_r->setText(button->text() + ":\\");
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(button->text()));
    }
    on_path_r_returnPressed();
    size_d_r(button->text() + ":\\");
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
        ui->path_l->setText(new_disk + ":\\");
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(new_disk));
    }
    on_path_l_returnPressed();
    size_d_l(new_disk + ":\\");
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
        ui->path_r->setText(new_disk + ":\\");
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(new_disk));
    }
    on_path_r_returnPressed();
    size_d_r(new_disk + ":\\");
}

//изменяет информацию по выбранному левому диску
void MainWindow::size_d_l(QString disk)
{
    QStorageInfo st_inf = QStorageInfo(disk);
    float av_b = st_inf.bytesAvailable();
    float all_b = st_inf.bytesTotal();
    float occup_b = (all_b - av_b)*100 / all_b;
    if (occup_b < 90.0)
        disk_progress_l->setStyleSheet("QProgressBar::chunk {background-color: rgb(38,160,218);}");
    else
        disk_progress_l->setStyleSheet("QProgressBar::chunk {background-color: rgb(218,38,38);}");
    disk_name_l->setText(st_inf.name() + "  (" + disk.remove("\\").toUpper() + ")");
    disk_progress_l->setValue(occup_b);
    disk_free_size_l->setText(HelperFunctions::reformat_size(QString::number(round(float(av_b)/1024), 'g', 20)) + " КБ из " +
                              HelperFunctions::reformat_size(QString::number(round(float(all_b)/1024), 'g', 20)) + " КБ свободно");
    disk_free_size_l->setToolTip(HelperFunctions::reformat_size_2(av_b) + " из " + HelperFunctions::reformat_size_2(all_b) + " свободно");
}

//изменяет информацию по выбранному правому диску
void MainWindow::size_d_r(QString disk)
{
    QStorageInfo st_inf = QStorageInfo(disk);
    float av_b = float(st_inf.bytesAvailable());
    float all_b = float(st_inf.bytesTotal());
    float occup_b = (all_b - av_b)*100 / float(all_b);
    if (occup_b < 90.0)
        disk_progress_r->setStyleSheet("QProgressBar::chunk {background-color: rgb(38,160,218);}");
    else
        disk_progress_r->setStyleSheet("QProgressBar::chunk {background-color: rgb(218,38,38);}");
    disk_name_r->setText(st_inf.name() + "  (" + disk.remove("\\").toUpper() + ")");
    disk_progress_r->setValue(occup_b);
    disk_free_size_r->setText(HelperFunctions::reformat_size(QString::number(round(float(av_b)/1024), 'g', 20)) + " КБ из " +
                              HelperFunctions::reformat_size(QString::number(round(float(all_b)/1024), 'g', 20)) + " КБ свободно");
    disk_free_size_r->setToolTip(HelperFunctions::reformat_size_2(av_b) + " из " + HelperFunctions::reformat_size_2(all_b) + " свободно");
}


//изменение левого пути
void MainWindow::on_path_l_returnPressed()
{
    ui->path_l->setText(QDir::cleanPath(ui->path_l->text()).replace("/", "\\"));
    QString new_disk = ui->path_l->text().split("\\").first();
    ui->path_l->setText(ui->path_l->text().replace(new_disk, new_disk.toLower()));
    new_disk = new_disk.toLower();
    //ui->path_l->setText(ui->path_l->text().replace("/", "\\"));

    if (!ui->path_l->text().endsWith("\\")) {
        if (QFile(ui->path_l->text()).exists() && QFileInfo(ui->path_l->text()).isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(ui->path_l->text()));
            ui->path_l->setText(ui->path_l->text().left(ui->path_l->text().lastIndexOf("\\")) +"\\");
        } else
            ui->path_l->setText(ui->path_l->text() + "\\");
    }
    QDir dir(ui->path_l->text());
    if (dir.exists() && ui->path_l->text().endsWith("\\")) {
        size_d_l(new_disk + "\\");
        combobox_disk_l->setCurrentIndex(combobox_disk_l->findText(new_disk.removeLast()));
        treeWidget_l->clear();


        QString last_dir_l = "";
        if (last_path_l.contains(ui->path_l->text())) {
            last_dir_l = last_path_l.remove(ui->path_l->text()).replace("\\", "");
        }

        treeWidget_l->Fill(ui->path_l->text(), hidden_f, last_dir_l);

        all_l_v = treeWidget_l->all_v;
        all_f_l = treeWidget_l->all_f;
        last_path_l = ui->path_l->text();
        treeWidget_l->path = last_path_l;
        all_l_v = round(all_l_v / 1024);

        ui->inf_dir_l->setText("0 КБ из " + HelperFunctions::HelperFunctions::reformat_size(QString::number(all_l_v, 'g', 20)) + " КБ, файлов: 0 из " + QString::number(all_f_l));
    } else {
        v_error("Путь " % QString('"') % ui->path_l->text() % QString('"') % " не найден.");
        ui->path_l->setText(last_path_l);
    }
}

//изменение правого пути
void MainWindow::on_path_r_returnPressed()
{
    ui->path_r->setText(QDir::cleanPath(ui->path_r->text()).replace("/", "\\"));
    QString new_disk = ui->path_r->text().split("\\").first();
    ui->path_r->setText(ui->path_r->text().replace(new_disk, new_disk.toLower()));
    new_disk = new_disk.toLower();
    //ui->path_r->setText(ui->path_r->text().replace("/", "\\"));

    if (!ui->path_r->text().endsWith("\\")) {
        if (QFile(ui->path_r->text()).exists() && QFileInfo(ui->path_r->text()).isFile()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(ui->path_r->text()));
            ui->path_r->setText(ui->path_r->text().left(ui->path_r->text().lastIndexOf("\\")) +"\\");
        } else
            ui->path_r->setText(ui->path_r->text() + "\\");
    }
    QDir dir(ui->path_r->text());
    if (dir.exists() && ui->path_r->text().endsWith("\\")){
        QString new_disk = ui->path_r->text().split("\\").first();
        size_d_r(new_disk + "\\");
        combobox_disk_r->setCurrentIndex(combobox_disk_r->findText(new_disk.removeLast()));
        treeWidget_r->clear();

        QString last_dir_r = "";
        if (last_path_r.contains(ui->path_r->text())) {
            last_dir_r = last_path_r.remove(ui->path_r->text()).replace("\\", "");
        }

        treeWidget_r->Fill(ui->path_r->text(), hidden_f, last_dir_r);

        all_r_v = treeWidget_r->all_v;
        all_f_r = treeWidget_r->all_f;
        last_path_r = ui->path_r->text();
        treeWidget_r->path = last_path_r;
        all_r_v = round(all_r_v / 1024);

        ui->inf_dir_r->setText("0 КБ из " + HelperFunctions::HelperFunctions::reformat_size(QString::number(all_r_v, 'g', 20)) + " КБ, файлов: 0 из " + QString::number(all_f_r));
    } else {
        v_error("Путь " + QString('"') + ui->path_r->text() + QString('"') + " не найден.");
        ui->path_r->setText(last_path_r);
    }
}
