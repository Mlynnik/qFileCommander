#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHeaderView>
#include <QFile>

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



    ui->horizontalLayout_l->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_r->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_free_size_l->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_free_size_l->addWidget(combobox_disk_l);
    ui->horizontalLayout_free_size_l->addWidget(disk_name_l);
    ui->horizontalLayout_free_size_l->addWidget(disk_progress_l);
    ui->horizontalLayout_free_size_l->addWidget(disk_free_size_l, 4);
    //connect(combobox_disk_l, SIGNAL(activated(int)), this, SLOT(combobox_disk_l_changed(int)));
    disk_progress_l->setFormat("");
    ui->horizontalLayout_free_size_r->setAlignment(Qt::AlignLeft);
    ui->horizontalLayout_free_size_r->addWidget(combobox_disk_r);
    ui->horizontalLayout_free_size_r->addWidget(disk_name_r);
    ui->horizontalLayout_free_size_r->addWidget(disk_progress_r);
    ui->horizontalLayout_free_size_r->addWidget(disk_free_size_r, 4);
    //connect(combobox_disk_r, SIGNAL(activated(int)), this, SLOT(combobox_disk_r_changed(int)));
    disk_progress_r->setFormat("");
    ui->path_l->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    ui->path_l->setFocusPolicy(Qt::ClickFocus);
    ui->path_r->setStyleSheet("QLineEdit {background: rgb(153, 180, 209);}");
    ui->path_r->setFocusPolicy(Qt::ClickFocus);
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
