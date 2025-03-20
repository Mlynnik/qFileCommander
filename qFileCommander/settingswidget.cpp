#include "settingswidget.h"
#include <QKeyEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QFontDialog>
#include <QTextEdit>
#include <QToolButton>
#include <QInputDialog>
#include <QFileDialog>

SettingsWidget::SettingsWidget(AppSettings* appsettings, QWidget *parent) : QTabWidget{parent}
{
    w = appsettings->w;
    h = appsettings->h;
    main_font = appsettings->main_font;
    panel_font = appsettings->panel_font;
    dialog_font = appsettings->dialog_font;
    lister_font = appsettings->lister_font;

    SettingsWidget::setAttribute(Qt::WA_DeleteOnClose);
    SettingsWidget::setAttribute(Qt::WA_ShowModal);
    SettingsWidget::setWindowTitle("Настройки");
    SettingsWidget::setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    setFixedSize(round(400*w), round(500*h));
    setFont(*main_font);
    page_fonts = new QWidget(this);
    QGridLayout *lay_fonts = new QGridLayout(page_fonts);

    QLabel *lab_main_font = new QLabel(page_fonts);
    lab_main_font->setText("Шрифт основного окна");
    QLabel *lab_panel_font = new QLabel(page_fonts);
    lab_panel_font->setText("Шрифт файловых панелей");
    QLabel *lab_dialog_font = new QLabel(page_fonts);
    lab_dialog_font->setText("Шрифт диалоговых окон");
    QLabel *lab_lister_font = new QLabel(page_fonts);
    lab_lister_font->setText("Шрифт в Lister");

    lab_main_font_now = new QLabel(page_fonts);
    lab_main_font_now->setText(main_font->toString());
    lab_panel_font_now = new QLabel(page_fonts);
    lab_panel_font_now->setFont(*panel_font);
    lab_panel_font_now->setText(panel_font->toString());
    lab_dialog_font_now = new QLabel(page_fonts);
    lab_dialog_font_now->setFont(*dialog_font);
    lab_dialog_font_now->setText(dialog_font->toString());
    lab_lister_font_now = new QLabel(page_fonts);
    lab_lister_font_now->setFont(*lister_font);
    lab_lister_font_now->setText(lister_font->toString());

    QPushButton *btn_main_font = new QPushButton(page_fonts);
    btn_main_font->setText("Изменить");
    connect(btn_main_font, SIGNAL(clicked()), this, SLOT(change_main_font()));
    QPushButton *btn_panel_font = new QPushButton(page_fonts);
    btn_panel_font->setText("Изменить");
    connect(btn_panel_font, SIGNAL(clicked()), this, SLOT(change_panel_font()));
    QPushButton *btn_dialog_font = new QPushButton(page_fonts);
    btn_dialog_font->setText("Изменить");
    connect(btn_dialog_font, SIGNAL(clicked()), this, SLOT(change_dialog_font()));
    QPushButton *btn_lister_font = new QPushButton(page_fonts);
    btn_lister_font->setText("Изменить");
    connect(btn_lister_font, SIGNAL(clicked()), this, SLOT(change_lister_font()));


    QFrame *l00 = new QFrame(page_fonts);
    l00->setFrameShape(QFrame::HLine);
    l00->setLineWidth(1);
    l00->setFixedHeight(10);
    lay_fonts->addWidget(l00, 0, 0, 1, 2);

    lay_fonts->addWidget(lab_main_font, 1, 0 , 1, 2);
    lay_fonts->addWidget(lab_main_font_now, 2, 0);
    lay_fonts->addWidget(btn_main_font, 2, 1);

    QFrame *l01 = new QFrame(page_fonts);
    l01->setFrameShape(QFrame::HLine);
    l01->setLineWidth(1);
    l01->setFixedHeight(10);
    lay_fonts->addWidget(l01, 3, 0, 1, 2);

    QSpacerItem *hs1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay_fonts->addItem(hs1, 4, 0, 1, 2);

    QFrame *l10 = new QFrame(page_fonts);
    l10->setFrameShape(QFrame::HLine);
    l10->setLineWidth(1);
    l10->setFixedHeight(10);
    lay_fonts->addWidget(l10, 5, 0, 1, 2);

    lay_fonts->addWidget(lab_panel_font, 6, 0 , 1, 2);
    lay_fonts->addWidget(lab_panel_font_now, 7, 0);
    lay_fonts->addWidget(btn_panel_font, 7, 1);

    QFrame *l11 = new QFrame(page_fonts);
    l11->setFrameShape(QFrame::HLine);
    l11->setLineWidth(1);
    l11->setFixedHeight(10);
    lay_fonts->addWidget(l11, 8, 0, 1, 2);

    QSpacerItem *hs2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay_fonts->addItem(hs2, 9, 0, 1, 2);

    QFrame *l20 = new QFrame(page_fonts);
    l20->setFrameShape(QFrame::HLine);
    l20->setLineWidth(1);
    l20->setFixedHeight(10);
    lay_fonts->addWidget(l20, 10, 0, 1, 2);

    lay_fonts->addWidget(lab_dialog_font, 11, 0 , 1, 2);
    lay_fonts->addWidget(lab_dialog_font_now, 12, 0);
    lay_fonts->addWidget(btn_dialog_font, 12, 1);

    QFrame *l21 = new QFrame(page_fonts);
    l21->setFrameShape(QFrame::HLine);
    l21->setLineWidth(1);
    l21->setFixedHeight(10);
    lay_fonts->addWidget(l21, 13, 0, 1, 2);

    QSpacerItem *hs3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay_fonts->addItem(hs3, 14, 0, 1, 2);

    QFrame *l30 = new QFrame(page_fonts);
    l30->setFrameShape(QFrame::HLine);
    l30->setLineWidth(1);
    l30->setFixedHeight(10);
    lay_fonts->addWidget(l30, 15, 0, 1, 2);

    lay_fonts->addWidget(lab_lister_font, 16, 0 , 1, 2);
    lay_fonts->addWidget(lab_lister_font_now, 17, 0);
    lay_fonts->addWidget(btn_lister_font, 17, 1);

    QFrame *l31 = new QFrame(page_fonts);
    l31->setFrameShape(QFrame::HLine);
    l31->setLineWidth(1);
    l31->setFixedHeight(10);
    lay_fonts->addWidget(l31, 18, 0, 1, 2);

    lay_fonts->setColumnStretch(0, 1);
    lay_fonts->setColumnStretch(1, 0);

    //page_ref = new QWidget(this);
    QTextEdit *page_ref = new QTextEdit(this);
    page_ref->setReadOnly(true);
    page_ref->setText("Когда-нибудь здесь появится информация о приложении...");

    page_fonts->setLayout(lay_fonts);
    addTab(page_fonts, "Шрифты");
    addTab(page_ref, "Справка");
}

void SettingsWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void SettingsWidget::change_main_font()
{
    bool ok;
    QFont font = QFontDialog::getFont(
        &ok, *main_font, this);
    if (ok) {
        *main_font = font;
        setFont(*main_font);
        lab_main_font_now->setText(main_font->toString());
        emit apply_main_font();
    }
}

void SettingsWidget::change_panel_font()
{
    bool ok;
    QFont font = QFontDialog::getFont(
        &ok, *panel_font, this);
    if (ok) {
        *panel_font = font;
        lab_panel_font_now->setFont(*panel_font);
        lab_panel_font_now->setText(panel_font->toString());
        emit apply_panel_font();
    }
}

void SettingsWidget::change_dialog_font()
{
    bool ok;
    QFont font = QFontDialog::getFont(
        &ok, *dialog_font, this);
    if (ok) {
        *dialog_font = font;
        lab_dialog_font_now->setFont(*dialog_font);
        lab_dialog_font_now->setText(dialog_font->toString());
    }
}

void SettingsWidget::change_lister_font()
{
    bool ok;
    QFont font = QFontDialog::getFont(
        &ok, *lister_font, this);
    if (ok) {
        *lister_font = font;
        lab_lister_font_now->setFont(*lister_font);
        lab_lister_font_now->setText(lister_font->toString());
    }
}

SettingsFavWidget::SettingsFavWidget(AppSettings *_appSettings, QToolButton *_menu_l, QToolButton *_menu_r, QWidget *parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setWindowTitle("Избранное");
    setWindowIcon(QIcon(":/resources/icons/appIcon.png"));

    menu_l = _menu_l;
    menu_r = _menu_r;
    appSettings = _appSettings;
    float w = appSettings->w;
    float h = appSettings->h;
    resize(round(600*w), round(400*h));
    setFont(*appSettings->main_font);

    gridLayout = new QGridLayout(this);
    setLayout(gridLayout);

    listWidget = new QListWidget(this);
    listWidget->setDragDropMode(QAbstractItemView::InternalMove);

    btn_add = new QPushButton("Добавить", this);
    connect(btn_add, SIGNAL(clicked()), this, SLOT(add_clicked()));
    btn_del = new QPushButton("Удалить", this);
    connect(btn_del, SIGNAL(clicked()), this, SLOT(del_clicked()));
    btn_rename = new QPushButton("Переименовать", this);
    connect(btn_rename, SIGNAL(clicked()), this, SLOT(rename_clicked()));

    btn_ok = new QPushButton("Ok", this);
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(ok_clicked()));
    btn_cancel = new QPushButton("Отмена", this);
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));

    verticalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addWidget(listWidget, 0, 0, 6, 1);
    gridLayout->addWidget(btn_add, 0, 1);
    gridLayout->addWidget(btn_del, 1, 1);
    gridLayout->addWidget(btn_rename, 2, 1);
    gridLayout->addItem(verticalSpacer, 3, 1);
    gridLayout->addWidget(btn_ok, 4, 1);
    gridLayout->addWidget(btn_cancel, 5, 1);

    gridLayout->setColumnStretch(0, 1);

    int l_fav =  menu_l->actions().size() - 3;
    for (int i = 0; i < l_fav; ++i) {
        QListWidgetItem *item = new QListWidgetItem(listWidget);
        item->setText(menu_l->actions().at(i)->text());
        item->setData(Qt::UserRole, menu_l->actions().at(i)->data());
    }
}

void SettingsFavWidget::ok_clicked()
{
    int l_fav =  menu_l->actions().size() - 3;
    {
        QList<QAction*> la = menu_l->actions();
        for (int i = 0; i < l_fav; ++i)
            menu_l->removeAction(la.at(i));

        la = menu_r->actions();
        for (int i = 0; i < l_fav; ++i)
            menu_r->removeAction(la.at(i));
    }

    QListWidgetItem *item;
    QStringList fnames, fpathes;
    for (int i = 0; i < listWidget->count(); ++i) {
        item = listWidget->item(i);
        fnames << item->text();
        fpathes << item->data(Qt::UserRole).toString();
    }
    emit update_menu(fnames, fpathes);
    this->close();
}

void SettingsFavWidget::cancel_clicked()
{
    this->close();
}

void SettingsFavWidget::rename_clicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return;

    QString new_name = item->text();

    QInputDialog id;
    id.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    id.setFont(*appSettings->main_font);
    id.resize(QSize(400, 60));
    id.setCancelButtonText("Отмена");
    id.setLabelText("Название нового элемента в избранном:");
    id.setTextValue(new_name);
    if (!id.exec())
        return;

    new_name = id.textValue();

    item->setText(new_name);
}

void SettingsFavWidget::del_clicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return;

    delete listWidget->takeItem(listWidget->row(item));
}

void SettingsFavWidget::add_clicked()
{
    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::Directory);
    fd.setDirectory(QDir::homePath());

    QString new_path;
    if (fd.exec())
        new_path = fd.selectedFiles()[0];

    if (new_path.isEmpty())
        return;

    new_path = QFileInfo(new_path).absoluteFilePath() + "/";
    for (int i = 0; i < listWidget->count(); ++i) {
        if (listWidget->item(i)->data(Qt::UserRole).toString() == new_path)
            return;
    }
    QString new_name = new_path; new_name = new_name.removeLast().split("/").last();

    QInputDialog id;
    id.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    id.setFont(*appSettings->main_font);
    id.resize(QSize(400, 60));
    id.setCancelButtonText("Отмена");
    id.setLabelText("Название нового элемента в избранном:");
    id.setTextValue(new_name);
    if (!id.exec())
        return;

    new_name = id.textValue();

    QListWidgetItem *item = new QListWidgetItem(new_name, listWidget);
    item->setData(Qt::UserRole, new_path);
}

void SettingsFavWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}
