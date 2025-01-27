#include "settingswidget.h"
#include <QKeyEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QFontDialog>
#include <QTextEdit>

SettingsWidget::SettingsWidget(QFont *_main_font, QFont *_panel_font, QFont *_dialog_font, QFont *_lister_font, float _w, float _h, QWidget *parent)
    : main_font(_main_font), panel_font(_panel_font),  dialog_font(_dialog_font),  lister_font(_lister_font),
    QTabWidget{parent}
{
    w = _w;
    h = _h;
    SettingsWidget::setAttribute(Qt::WA_DeleteOnClose);
    SettingsWidget::setAttribute(Qt::WA_ShowModal);
    SettingsWidget::setWindowTitle("Настройки");
    SettingsWidget::setWindowIcon(QIcon("appIcon.png"));
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
