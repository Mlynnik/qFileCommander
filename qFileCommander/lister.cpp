#include "lister.h"
#include "playerwidget.h"
#include <QScrollBar>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QMimeDatabase>
#include <QImageReader>

Lister::Lister(const QString& _fpath, const AppSettings *_appSettings, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_DeleteOnClose);
    if (!QFile(_fpath).exists())
        this->close();

    appSettings = _appSettings;
    fpath = _fpath;
    main_font = appSettings->main_font;
    dialog_font = appSettings->dialog_font;
    lister_font = appSettings->lister_font;

    setWindowTitle("Lister - [" + fpath + "]");
    setWindowIcon(QIcon("appIcon.png"));
    resize(round(appSettings->w*800), round(appSettings->h*600));
    setFont(*main_font);

    menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, round(appSettings->w*800), round(appSettings->h*25)));
    Lister::setMenuBar(menubar);

    cod_only_text = new QAction("Только текст", this);
    cod_html = new QAction("HTML (без показа тэгов)", this);
    cod_img = new QAction("Графика", this);
    cod_audio = new QAction("Аудио", this);
    cod_utf8 = new QAction("UTF-8", this);
    cod_local = new QAction("Local", this);
    cod_only_text->setCheckable(true);
    cod_html->setCheckable(true);
    cod_img->setCheckable(true);
    cod_audio->setCheckable(true);
    cod_utf8->setCheckable(true);
    cod_local->setCheckable(true);

    menu_view = new QMenu(menubar);
    menu_view->setTitle("Вид");
    menu_view->addAction(cod_only_text);
    menu_view->addAction(cod_html);
    menu_view->addAction(cod_img);
    menu_view->addAction(cod_audio);

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
    connect(cod_audio, SIGNAL(triggered()), this, SLOT(f3_cod_audio()));
    connect(cod_utf8, SIGNAL(triggered()), this, SLOT(f3_cod_utf8()));
    connect(cod_local, SIGNAL(triggered()), this, SLOT(f3_cod_local()));

    file = new QFile(fpath);
    f_size = file->size();

    Fill();
}

Lister::~Lister()
{
    if (file->isOpen())
        file->close();
    delete file;

    emit closed();
}

void Lister::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        this->close();
}

void Lister::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if (th)
        emit stop();
    else
        event->accept();
}

void Lister::Fill()
{
    if (QFileInfo(fpath).isDir()) {
        setMode(ListerMode::Dir);
        return;
    }

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(file->fileName());
    QString m_s = mime.name();

    if (m_s.startsWith("audio")) {
        setMode(ListerMode::Player);
        f_type = 'a';
        f_type_now = 'a';
        cod_img->setCheckable(false);
        cod_img->setDisabled(true);
        cod_audio->setChecked(true);
        return;
    }

    if (m_s.startsWith("image")) {
        setMode(ListerMode::Image);
        f_type = 'i';
        f_type_now = 'i';
        cod_audio->setCheckable(false);
        cod_audio->setDisabled(true);
        cod_img->setChecked(true);
        return;
    }


    setMode(ListerMode::Text);
    if (m_s.contains("html") or m_s.contains("xml") or m_s.contains("epub")) {
        f_type = 'x';
        f_type_now = 'x';
        cod_html->setChecked(true);
    } else {
        f_type = 't';
        f_type_now = 't';
        cod_only_text->setChecked(true);
    }

    cod_img->setCheckable(false);
    cod_img->setDisabled(true);
    cod_audio->setCheckable(false);
    cod_audio->setDisabled(true);


    if (!file->open(QIODevice::ReadOnly))
        this->close();

    if (f_size < 1048576) {
        pushButton_up->hide();
        pushButton_down->hide();
        lineEdit_size_now->hide();
        label_all_size->hide();
        verticalSlider->hide();

        if (f_type == 'x')
            plainTextEdit->textCursor().insertHtml(file->readAll());
        else
            plainTextEdit->setPlainText(file->readAll());
    } else {
        verticalSlider_valueChanged(1);
    }
}

void Lister::setMode(ListerMode m)
{
    if (mode == m)
        return;

    mode = m;

    if (centralwidget)
        centralwidget->close();
    centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);
    centralwidget->setAttribute(Qt::WA_DeleteOnClose);
    horizontalLayout = new QHBoxLayout(centralwidget);
    horizontalLayout->setAlignment(Qt::AlignTop);

    {
        gridLayout = nullptr;
        horizontalSpacer = nullptr;
        pushButton_up = nullptr;
        lineEdit_size_now = nullptr;
        image_area = nullptr;
        label_image = nullptr;
        label_all_size = nullptr;
        plainTextEdit = nullptr;
        horizontalSpacer_2 = nullptr;
        pushButton_down = nullptr;
        verticalSlider = nullptr;
    }

    if (m == ListerMode::Text) {
        menu_cod->setDisabled(false);

        gridLayout = new QGridLayout();

        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setReadOnly(true);
        plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
        plainTextEdit->setFont(*lister_font);
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

        if (f_size < 1048576) {
            pushButton_up->hide();
            pushButton_down->hide();
            lineEdit_size_now->hide();
            label_all_size->hide();
            verticalSlider->hide();
        }

        horizontalLayout->addLayout(gridLayout);

        label_all_size->setText("из " + QString::number(ceil((double)f_size/1048576.0)) + " МБ");
        lineEdit_size_now->setText("1");
        verticalSlider->setMinimum(1);
        verticalSlider->setMaximum(ceil((double)f_size/1048576.0));
        connect(verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(verticalSlider_valueChanged(int)));
        connect(lineEdit_size_now, SIGNAL(returnPressed()), this, SLOT(on_lineEdit_size_now_returnPressed()));
        connect(pushButton_up, SIGNAL(clicked()), this, SLOT(on_pushButton_up_clicked()));
        connect(pushButton_down, SIGNAL(clicked()), this, SLOT(on_pushButton_down_clicked()));
    } else if (m == ListerMode::Image) {
        if (file->isOpen())
            file->close();

        menu_cod->setDisabled(true);
        label_image = new QLabel(centralwidget);
        image_area = new QScrollArea(centralwidget);
        image_area->setWidget(label_image);
        horizontalLayout->addWidget(image_area);

        QImage image = QImageReader(fpath).read();
        image = image.scaled(image.size(), Qt::KeepAspectRatio);
        label_image->setFixedSize(image.size());
        label_image->setPixmap(QPixmap::fromImage(image));
    } else if (m == ListerMode::Player) {
        if (file->isOpen())
            file->close();

        PlayerWidget* player = new PlayerWidget(fpath, appSettings, centralwidget);
        horizontalLayout->addWidget(player);
        setFocusProxy(player);
        menu_cod->setDisabled(true);
    } else if (m == ListerMode::Dir) {
        menu_view->setDisabled(true);
        menu_cod->setDisabled(true);

        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setReadOnly(true);
        horizontalLayout->addWidget(plainTextEdit);

        plainTextEdit->setPlainText(fpath);

        DirPropProcess *dpp = new DirPropProcess(fpath);
        th = new QThread(this);

        connect(th, SIGNAL(started()), dpp, SLOT(Work()));
        connect(dpp, SIGNAL(setInfo(qlonglong,qlonglong,qlonglong)), this, SLOT(setDirInfo(qlonglong,qlonglong,qlonglong)));
        connect(this, SIGNAL(stop()), dpp, SLOT(stop()), Qt::DirectConnection);
        connect(dpp, SIGNAL(break_proc()), this, SLOT(break_proc()));

        dpp->moveToThread(th);
        th->start();
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
                plainTextEdit->textCursor().insertHtml(buf);
            else
                plainTextEdit->textCursor().insertHtml(QString::fromLocal8Bit(buf));
        }
        progress += 1048576;
        if (progress > f_size)
            progress = f_size;
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
    if (f_type_now == 't')
        return;

    cod_html->setChecked(false);
    cod_img->setChecked(false);
    cod_audio->setChecked(false);

    if (mode == ListerMode::Image || mode == ListerMode::Player) {
        f_type_now = 't';
        setMode(ListerMode::Text);
    } else if (f_type_now == 'x') {
        f_type_now = 't';
        plainTextEdit->clear();
    }

    if (!file->isOpen()) {
        if (!file->open(QIODevice::ReadOnly))
            this->close();
    }
    if (f_size < 1048576) {
        file->seek(0);
        if (f_cod == '8')
            plainTextEdit->setPlainText(file->readAll());
        else
            plainTextEdit->setPlainText(QString::fromLocal8Bit(file->readAll()));
    } else {
        verticalSlider_valueChanged(verticalSlider->value());
    }
}

void Lister::f3_cod_html()
{
    cod_html->setChecked(true);
    if (f_type_now == 'x')
        return;

    cod_only_text->setChecked(false);
    cod_img->setChecked(false);
    cod_audio->setChecked(false);

    if (mode == ListerMode::Image || mode == ListerMode::Player) {
        f_type_now = 'x';
        setMode(ListerMode::Text);
    } else if (f_type_now == 't') {
        f_type_now = 'x';
        plainTextEdit->clear();
    }

    if (!file->isOpen()) {
        if (!file->open(QIODevice::ReadOnly))
            this->close();
    }
    if (f_size < 1048576) {
        file->seek(0);
        if (f_cod == '8')
            plainTextEdit->textCursor().insertHtml(file->readAll());
        else
            plainTextEdit->textCursor().insertHtml(QString::fromLocal8Bit(file->readAll()));
    } else {
        verticalSlider_valueChanged(verticalSlider->value());
    }
}

void Lister::f3_cod_img()
{
    cod_img->setChecked(true);
    if (f_type_now == 'i')
        return;

    cod_only_text->setChecked(false);
    cod_html->setChecked(false);
    cod_audio->setChecked(false);

    setMode(ListerMode::Image);
    f_type_now = 'i';
    image_area->show();
}

void Lister::f3_cod_audio()
{
    cod_audio->setChecked(true);
    if (f_type_now == 'a')
        return;

    cod_only_text->setChecked(false);
    cod_html->setChecked(false);
    cod_img->setChecked(false);

    setMode(ListerMode::Player);
    f_type_now = 'a';
}

void Lister::f3_cod_utf8()
{
    cod_utf8->setChecked(true);
    if (f_cod == '8')
        return;

    cod_local->setChecked(false);

    f_cod = '8';
    if (f_size < 1048576) {
        plainTextEdit->clear();
        file->seek(0);
        if (f_type_now == 't')
            plainTextEdit->setPlainText(file->readAll());
        else
            plainTextEdit->textCursor().insertHtml(file->readAll());
    } else
        verticalSlider_valueChanged(verticalSlider->value());
}

void Lister::f3_cod_local()
{
    cod_local->setChecked(true);
    if (f_cod == 'l')
        return;

    cod_utf8->setChecked(false);

    f_cod = 'l';
    if (f_size < 1048576) {
        plainTextEdit->clear();
        file->seek(0);
        if (f_type_now == 't')
            plainTextEdit->setPlainText(QString::fromLocal8Bit(file->readAll()));
        else
            plainTextEdit->textCursor().insertHtml(QString::fromLocal8Bit(file->readAll()));
    } else
        verticalSlider_valueChanged(verticalSlider->value());
}


QString reformat_size(QString str)
{
    int x = str.length() - 3;
    while(x > 0) {str.insert(x, QString(" ")); x -= 3;}
    return str;
}

QString reformat_size_2(double num_0)
{
    QString str1;
    if (num_0 >= 1000) {
        num_0 = round(num_0 / 10.24) / 100;
        str1 = QString::number(num_0) + " КБ";
        if (num_0 >= 1000) {
            num_0 = round(num_0 / 10.24) / 100;
            str1 = QString::number(num_0) + " MБ";
            if (num_0 >= 1000) {
                num_0 = round(num_0 / 10.24) / 100;
                str1 = QString::number(num_0) + " ГБ";
                if (num_0 >= 1000) {
                    num_0 = round(num_0 / 10.24) / 100;
                    str1 = QString::number(num_0) + " ТБ";
                }
            }
        }
    } else
        str1 = QString::number(num_0) + " Б";
    return str1;
}

void Lister::setDirInfo(long long all_size, long long dcnt, long long fcnt)
{
    if (th) {
        delete th;
        th = nullptr;
    }
    if (is_break)
        this->close();
    if (plainTextEdit) {
        plainTextEdit->appendPlainText("\nВсего файлов:     " % reformat_size(QString::number(fcnt)) % ",\nкаталогов:     "
                                       % reformat_size(QString::number(dcnt)) % "\n\nОбщий размер:     "
                                       % reformat_size(QString::number(all_size)) % " Б (" % reformat_size_2(all_size) % ")");
    }
}


DirPropProcess::DirPropProcess(const QString &_fpath) : fpath(_fpath){}

void DirPropProcess::Work()
{
    if (QDir().exists(fpath)) {
        QDirIterator it(fpath, QDir::Dirs | QDir::Files | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            if (is_stop)
                break;
            it.next();
            if (it.fileInfo().isDir()) {
                ++dcnt;
            } else if (it.fileInfo().isFile()) {
                ++fcnt;
                all_size += it.fileInfo().size();
            }
        }
    }
    if (is_stop)
        emit break_proc();
    emit setInfo(all_size, dcnt, fcnt);

    this->deleteLater();
    this->thread()->exit();
}
