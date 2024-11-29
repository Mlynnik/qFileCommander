#include "lister.h"
#include <QScrollBar>
#include <QMimeDatabase>
#include <QImageReader>

Lister::Lister(const QString& _f_name, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    if (!QFile(_f_name).exists())
        this->close();

    f_name = _f_name;
    Lister::setWindowTitle("Lister - [" + f_name + "]");
    Lister::resize(800, 600);
    centralwidget = new QWidget(this);
    horizontalLayout = new QHBoxLayout(centralwidget);
    gridLayout = new QGridLayout();

    label_image = new QLabel(centralwidget);

    image_area = new QScrollArea(centralwidget);
    image_area->setWidget(label_image);
    gridLayout->addWidget(image_area, 0, 0, 1, 7);
    image_area->hide();

    plainTextEdit = new QPlainTextEdit(centralwidget);
    plainTextEdit->setReadOnly(true);
    plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
    gridLayout->addWidget(plainTextEdit, 1, 0, 1, 6);

    verticalSlider = new QSlider(centralwidget);
    verticalSlider->setOrientation(Qt::Vertical);
    verticalSlider->setInvertedAppearance(true);
    verticalSlider->setInvertedControls(true);
    gridLayout->addWidget(verticalSlider, 1, 6);

    pushButton_up = new QPushButton(centralwidget);
    pushButton_up->setMaximumWidth(60);
    pushButton_up->setText("up");
    gridLayout->addWidget(pushButton_up, 2, 0);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer, 2, 1);

    lineEdit_size_now = new QLineEdit(centralwidget);
    lineEdit_size_now->setMaximumWidth(200);
    gridLayout->addWidget(lineEdit_size_now, 2, 2);

    label_all_size = new QLabel(centralwidget);
    label_all_size->setMaximumWidth(300);
    gridLayout->addWidget(label_all_size, 2, 3);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer_2, 2, 4);

    pushButton_down = new QPushButton(centralwidget);
    pushButton_down->setMaximumWidth(60);
    pushButton_down->setText("down");
    gridLayout->addWidget(pushButton_down, 2, 5);

    horizontalLayout->addLayout(gridLayout);

    Lister::setCentralWidget(centralwidget);
    menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, 800, 25));
    Lister::setMenuBar(menubar);

    cod_only_text = new QAction("Только текст", this);
    cod_html = new QAction("HTML (без показа тэгов)", this);
    cod_img = new QAction("Графика", this);
    cod_utf8 = new QAction("UTF-8", this);
    cod_local = new QAction("Local", this);
    cod_only_text->setCheckable(true);
    cod_html->setCheckable(true);
    cod_img->setCheckable(true);
    cod_utf8->setCheckable(true);
    cod_local->setCheckable(true);

    menu_view = new QMenu(menubar);
    menu_view->setTitle("Вид");
    menu_view->addAction(cod_only_text);
    menu_view->addAction(cod_html);
    menu_view->addAction(cod_img);

    menu_cod = new QMenu(menubar);
    menu_cod->setTitle("Кодировка");
    menu_cod->addAction(cod_utf8);
    menu_cod->addAction(cod_local);
    cod_utf8->setChecked(true);


    menubar->addAction(menu_view->menuAction());
    menubar->addAction(menu_cod->menuAction());

    connect(cod_only_text, SIGNAL(triggered()), this, SLOT(f3_cod_only_text()));
    connect(cod_html, SIGNAL(triggered()), this, SLOT(f3_cod_html()));
    connect(cod_img, SIGNAL(triggered()), this, SLOT(f3_cod_img()));
    connect(cod_utf8, SIGNAL(triggered()), this, SLOT(f3_cod_utf8()));
    connect(cod_local, SIGNAL(triggered()), this, SLOT(f3_cod_local()));


    file = new QFile(f_name);
    f_size = file->size();

    label_all_size->setText("из " + QString::number(ceil((double)f_size/1048576.0)) + " МБ");
    lineEdit_size_now->setText("1");
    verticalSlider->setMinimum(1);
    verticalSlider->setMaximum(ceil((double)f_size/1048576.0));
    connect(verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(verticalSlider_valueChanged(int)));
    connect(lineEdit_size_now, SIGNAL(returnPressed()), this, SLOT(on_lineEdit_size_now_returnPressed()));
    connect(pushButton_up, SIGNAL(clicked()), this, SLOT(on_pushButton_up_clicked()));
    connect(pushButton_down, SIGNAL(clicked()), this, SLOT(on_pushButton_down_clicked()));

    Fill();
}

Lister::~Lister()
{
    if (file->isOpen())
        file->close();
    delete file;
}

void Lister::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        this->close();
}

void Lister::Fill()
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(file->fileName());
    QString m_s = mime.name();

    if (m_s.contains("image")) {
        f_type = 'i';
        f_type_now = 'i';
        cod_img->setChecked(true);
        plainTextEdit->hide();
        pushButton_up->hide();
        pushButton_down->hide();
        lineEdit_size_now->hide();
        label_all_size->hide();
        verticalSlider->hide();
        image_area->show();

        QImage image = QImageReader(f_name).read();
        image = image.scaled(image.size(), Qt::KeepAspectRatio);

        label_image->setFixedSize(image.size());
        label_image->setPixmap(QPixmap::fromImage(image));
        return;
    } else if (m_s.contains("html") or m_s.contains("xml") or m_s.contains("epub")) {
        f_type = 'x';
        f_type_now = 'x';
        cod_html->setChecked(true);
    } else {
        f_type = 't';
        f_type_now = 't';
        cod_only_text->setChecked(true);
    }

    cod_img->setCheckable(false);
    cod_img->setEnabled(false);


    if (!file->open(QIODevice::ReadOnly))
        this->close();

    if (f_size < 10485760) {
        pushButton_up->hide();
        pushButton_down->hide();
        lineEdit_size_now->hide();
        label_all_size->hide();
        verticalSlider->hide();

        plainTextEdit->setPlainText(file->readAll());
    } else {
        verticalSlider_valueChanged(1);
    }
}


void Lister::on_pushButton_up_clicked()
{
    verticalSlider->setValue(verticalSlider->value() - 1);
}


void Lister::on_pushButton_down_clicked()
{
    verticalSlider->setValue(verticalSlider->value() + 1);
}


void Lister::verticalSlider_valueChanged(int value)
{
    lineEdit_size_now->setText(QString::number(value));
    file->seek((value-1)*1048576);

    plainTextEdit->clear();
    if (!(buf = file->read(1048576)).isEmpty()) {
        if (f_type_now == 't') {
            if (f_cod == '8')
                plainTextEdit->setPlainText(buf);
            else
                plainTextEdit->setPlainText(QString::fromLocal8Bit(buf));
        } else {
            if (f_cod == '8')
                plainTextEdit->appendHtml(buf);
            else
                plainTextEdit->appendHtml(QString::fromLocal8Bit(buf));
            plainTextEdit->textCursor().setPosition(0);
        }
        progres += 1048576;
        if (progres > f_size)
            progres = f_size;
    }
}


void Lister::on_lineEdit_size_now_returnPressed()
{
    bool ok;
    int now_s = lineEdit_size_now->text().toInt(&ok);
    if (ok) {
        verticalSlider->setValue(now_s);
    }
}

void Lister::f3_cod_only_text()
{
    cod_only_text->setChecked(true);
    cod_html->setChecked(false);
    cod_img->setChecked(false);
    if (label_image->isVisible()) {
        f_type_now = 't';
        image_area->hide();
        plainTextEdit->clear();
        plainTextEdit->show();
        if (!file->isOpen()) {
            if (!file->open(QIODevice::ReadOnly))
                this->close();
        }
        if (f_size < 10485760) {
            file->seek(0);
            if (f_cod == '8')
                plainTextEdit->setPlainText(file->readAll());
            else
                plainTextEdit->setPlainText(QString::fromLocal8Bit(file->readAll()));
        } else {
            pushButton_up->show();
            pushButton_down->show();
            lineEdit_size_now->show();
            label_all_size->show();
            verticalSlider->show();
            verticalSlider_valueChanged(verticalSlider->value());
        }
    } else if (f_type_now == 'x') {
        f_type_now = 't';
        plainTextEdit->clear();
        if (!file->isOpen()) {
            if (!file->open(QIODevice::ReadOnly))
                this->close();
        }
        if (f_size < 10485760) {
            file->seek(0);
            if (f_cod == '8')
                plainTextEdit->setPlainText(file->readAll());
            else
                plainTextEdit->setPlainText(QString::fromLocal8Bit(file->readAll()));
        } else {
            pushButton_up->show();
            pushButton_down->show();
            lineEdit_size_now->show();
            label_all_size->show();
            verticalSlider->show();
            verticalSlider_valueChanged(verticalSlider->value());
        }
    }
}

void Lister::f3_cod_html()
{
    cod_only_text->setChecked(false);
    cod_html->setChecked(true);
    cod_img->setChecked(false);

    if (label_image->isVisible()) {
        f_type_now = 'x';
        image_area->hide();
        plainTextEdit->clear();
        plainTextEdit->show();
        if (!file->isOpen()) {
            if (!file->open(QIODevice::ReadOnly))
                this->close();
        }
        if (f_size < 10485760) {
            file->seek(0);
            if (f_cod == '8')
                plainTextEdit->appendHtml(file->readAll());
            else
                plainTextEdit->appendHtml(QString::fromLocal8Bit(file->readAll()));
            plainTextEdit->textCursor().setPosition(0);
        } else {
            pushButton_up->show();
            pushButton_down->show();
            lineEdit_size_now->show();
            label_all_size->show();
            verticalSlider->show();
            verticalSlider_valueChanged(verticalSlider->value());
        }
    } else if (f_type_now == 't') {
        f_type_now = 'x';
        plainTextEdit->clear();
        if (!file->isOpen()) {
            if (!file->open(QIODevice::ReadOnly))
                this->close();
        }
        if (f_size < 10485760) {
            file->seek(0);
            if (f_cod == '8')
                plainTextEdit->appendHtml(file->readAll());
            else
                plainTextEdit->appendHtml(QString::fromLocal8Bit(file->readAll()));
            plainTextEdit->textCursor().setPosition(0);
        } else {
            pushButton_up->show();
            pushButton_down->show();
            lineEdit_size_now->show();
            label_all_size->show();
            verticalSlider->show();
            verticalSlider_valueChanged(verticalSlider->value());
        }
    }
}

void Lister::f3_cod_img()
{
    cod_only_text->setChecked(false);
    cod_html->setChecked(false);
    cod_img->setChecked(true);
    if (f_type_now != 'i') {
        f_type_now = 'i';
        plainTextEdit->hide();
        plainTextEdit->clear();
        pushButton_up->hide();
        pushButton_down->hide();
        lineEdit_size_now->hide();
        label_all_size->hide();
        verticalSlider->hide();
        image_area->show();
    }
}

void Lister::f3_cod_utf8()
{
    cod_utf8->setChecked(true);
    cod_local->setChecked(false);
    if (f_cod != '8') {
        f_cod = '8';
        if (plainTextEdit->isVisible()) {
            if (f_size < 10485760) {
                file->seek(0);
                if (f_type_now == 't')
                    plainTextEdit->setPlainText(file->readAll());
                else {
                    plainTextEdit->appendHtml(file->readAll());
                    plainTextEdit->textCursor().setPosition(0);
                }
            } else
                verticalSlider_valueChanged(verticalSlider->value());
        }
    }
}

void Lister::f3_cod_local()
{
    cod_utf8->setChecked(false);
    cod_local->setChecked(true);
    if (f_cod != 'l') {
        f_cod = 'l';
        if (plainTextEdit->isVisible()) {
            if (f_size < 10485760) {
                file->seek(0);
                if (f_type_now == 't')
                    plainTextEdit->setPlainText(QString::fromLocal8Bit(file->readAll()));
                else {
                    plainTextEdit->appendHtml(QString::fromLocal8Bit(file->readAll()));
                    plainTextEdit->textCursor().setPosition(0);
                }
            } else
                verticalSlider_valueChanged(verticalSlider->value());
        }
    }
}

