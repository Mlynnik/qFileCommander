#include "lister.h"
#include "playerwidget.h"
#include <QScrollBar>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QImageReader>

Lister::Lister(const QString& _fpath, const AppSettings *_appSettings, QWidget *parent)
    : QWidget(parent)
{
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_DeleteOnClose);

    appSettings = _appSettings;
    fpath = _fpath;

    setWindowTitle("Lister - [" + fpath + "]");
    setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    resize(round(appSettings->w*800), round(appSettings->h*600));
    setFont(*appSettings->main_font);

    horizontalLayout = new QVBoxLayout(this);
    horizontalLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    menubar = new QMenuBar(this);
    //menubar->setGeometry(0, 0, round(appSettings->w*200), round(appSettings->h*25));
    menubar->setMaximumHeight(round(appSettings->h*25));
    horizontalLayout->addWidget(menubar);

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

    if (QFile(_fpath).exists())
        Fill();
}

Lister::~Lister()
{
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
    if (is_th)
        emit stop();
    else
        event->accept();
}

void Lister::reFill(const QString &_fpath)
{
    if (!QFile(_fpath).exists())
        return;

    fpath = _fpath;
    cod_img->setChecked(false);
    cod_only_text->setChecked(false);
    cod_html->setChecked(false);
    cod_audio->setChecked(false);
    cod_img->setCheckable(true);
    cod_img->setDisabled(false);
    cod_audio->setCheckable(true);
    cod_audio->setDisabled(false);

    if (QFileInfo(fpath).isDir()) {
        if (mode == ListerMode::Dir && widget_now)
            ((DirWidget*)widget_now)->reFill(fpath);
        else
            setMode(ListerMode::Dir);
        return;
    }

    menu_view->setDisabled(false);
    QMimeType mime = db.mimeTypeForFile(fpath);
    QString m_s = mime.name();

    if (m_s.startsWith("audio")) {
        if (mode == ListerMode::Player && widget_now)
            ((PlayerWidget*)widget_now)->reFill(fpath);
        else
            setMode(ListerMode::Player);
        cod_img->setChecked(false);
        cod_img->setDisabled(true);
        cod_audio->setDisabled(false);
        cod_audio->setChecked(true);
        return;
    }

    if (m_s.startsWith("image")) {
        if (mode == ListerMode::Image && widget_now)
            ((ImageWidget*)widget_now)->reFill(fpath);
        else
            setMode(ListerMode::Image);
        cod_audio->setChecked(false);
        cod_audio->setDisabled(true);
        cod_img->setDisabled(false);
        cod_img->setChecked(true);
        return;
    }


    if (m_s.contains("html") or m_s.contains("xml") or m_s.contains("epub")) {
        is_xml = true;
        cod_html->setChecked(true);
    } else {
        is_xml = false;
        cod_only_text->setChecked(true);
    }
    if (mode == ListerMode::Text && widget_now)
        ((TextWidget*)widget_now)->reFill(fpath, is_xml, cod);
    else
        setMode(ListerMode::Text);

    cod_img->setDisabled(true);
    cod_audio->setDisabled(true);
}

void Lister::Fill()
{
    menu_view->setDisabled(false);
    if (QFileInfo(fpath).isDir()) {
        setMode(ListerMode::Dir);
        return;
    }

    QFile file(fpath);
    QMimeType mime = db.mimeTypeForFile(file.fileName());
    QString m_s = mime.name();

    if (m_s.startsWith("audio")) {
        setMode(ListerMode::Player);
        cod_img->setCheckable(false);
        cod_img->setDisabled(true);
        cod_audio->setDisabled(false);
        cod_audio->setChecked(true);
        return;
    }

    if (m_s.startsWith("image")) {
        setMode(ListerMode::Image);
        cod_audio->setCheckable(false);
        cod_audio->setDisabled(true);
        cod_img->setDisabled(false);
        cod_img->setChecked(true);
        return;
    }


    if (m_s.contains("html") or m_s.contains("xml") or m_s.contains("epub")) {
        is_xml = true;
        cod_html->setChecked(true);
    } else {
        is_xml = false;
        cod_only_text->setChecked(true);
    }

    setMode(ListerMode::Text);
    cod_img->setDisabled(true);
    cod_audio->setDisabled(true);
}

void Lister::setMode(ListerMode m)
{
    if (mode == m && widget_now)
        return;

    mode = m;
    if (widget_now) {
        widget_now->close();
        widget_now = nullptr;
    }

    if (m == ListerMode::Text) {
        menu_cod->setDisabled(false);
        TextWidget *tw = new TextWidget(fpath, appSettings, this, is_xml, cod);
        horizontalLayout->addWidget(tw);

        connect(this, SIGNAL(change_xml(bool)), tw, SLOT(change_xml(bool)));
        connect(this, SIGNAL(change_cod(FCodes)), tw, SLOT(change_cod(FCodes)));
        connect(tw, SIGNAL(stop()), this, SLOT(close()));
        widget_now = tw;
    }
    else if (m == ListerMode::Image) {
        menu_cod->setDisabled(true);
        ImageWidget *iw = new ImageWidget(fpath, this);
        horizontalLayout->addWidget(iw);
        widget_now = iw;
    }
    else if (m == ListerMode::Player) {
        PlayerWidget* player = new PlayerWidget(fpath, appSettings, this);
        horizontalLayout->addWidget(player);
        setFocusProxy(player);
        menu_cod->setDisabled(true);
        widget_now = player;
    }
    else if (m == ListerMode::Dir) {
        menu_view->setDisabled(true);
        menu_cod->setDisabled(true);

        is_th = true;
        DirWidget *dw = new DirWidget(fpath, appSettings, this);
        dw->setReadOnly(true);
        horizontalLayout->addWidget(dw);

        connect(this, SIGNAL(closed()), dw, SLOT(close()), Qt::DirectConnection);
        connect(dw, SIGNAL(th_finished()), this, SLOT(th_finished()));
        connect(this, SIGNAL(stop()), dw, SLOT(close()), Qt::DirectConnection);
        connect(dw, SIGNAL(parent_close()), this, SLOT(set_close()));
        widget_now = dw;
    }
}


void Lister::f3_cod_only_text()
{
    cod_only_text->setChecked(true);

    if (mode == ListerMode::Text) {
        if (is_xml) {
            is_xml = false;
            cod_html->setChecked(false);
            emit change_xml(false);
        }
        return;
    }

    cod_html->setChecked(false);
    cod_img->setChecked(false);
    cod_audio->setChecked(false);

    is_xml = false;
    setMode(ListerMode::Text);
}

void Lister::f3_cod_html()
{
    cod_html->setChecked(true);

    if (mode == ListerMode::Text) {
        if (!is_xml) {
            is_xml = true;
            cod_only_text->setChecked(false);
            emit change_xml(true);
        }
        return;
    }

    cod_only_text->setChecked(false);
    cod_img->setChecked(false);
    cod_audio->setChecked(false);

    is_xml = true;
    setMode(ListerMode::Text);
}

void Lister::f3_cod_img()
{
    cod_img->setChecked(true);
    if (mode == ListerMode::Image)
        return;

    cod_only_text->setChecked(false);
    cod_html->setChecked(false);
    cod_audio->setChecked(false);

    setMode(ListerMode::Image);
}

void Lister::f3_cod_audio()
{
    cod_audio->setChecked(true);
    if (mode == ListerMode::Player)
        return;

    cod_only_text->setChecked(false);
    cod_html->setChecked(false);
    cod_img->setChecked(false);

    setMode(ListerMode::Player);
}

void Lister::f3_cod_utf8()
{
    cod_utf8->setChecked(true);
    if (cod == FCodes::UTF8)
        return;

    cod_local->setChecked(false);

    cod = FCodes::UTF8;
    emit change_cod(cod);
}

void Lister::f3_cod_local()
{
    cod_local->setChecked(true);
    if (cod == FCodes::Local)
        return;

    cod_utf8->setChecked(false);

    cod = FCodes::Local;
    emit change_cod(cod);
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


//DirWidget
DirWidget::DirWidget(const QString &_fpath, const AppSettings *appSettings, QWidget *parent) : QPlainTextEdit(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    fpath = _fpath;
    setFont(*appSettings->lister_font);
    Fill();
}

void DirWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    if (th)
        emit stop();
    else
        event->accept();
}

void DirWidget::Fill()
{
    setPlainText(fpath);
    is_reFill = false;
    DirPropProcess *dpp = new DirPropProcess(fpath);
    th = new QThread(this);

    connect(th, SIGNAL(started()), dpp, SLOT(Work()));
    connect(dpp, SIGNAL(setInfo(qlonglong,qlonglong,qlonglong)), this, SLOT(setDirInfo(qlonglong,qlonglong,qlonglong)));
    connect(this, SIGNAL(stop()), dpp, SLOT(stop()), Qt::DirectConnection);
    connect(dpp, SIGNAL(break_proc()), this, SLOT(break_proc()));

    dpp->moveToThread(th);
    th->start();
}

void DirWidget::reFill(const QString &_fpath)
{
    if (th) {
        is_reFill = true;
        emit stop();
    }

    fpath = _fpath;
    clear();
    Fill();
}

void DirWidget::setDirInfo(long long all_size, long long dcnt, long long fcnt)
{
    if (th) {
        th->wait();
        delete th;
        th = nullptr;
    }
    if (is_reFill) {
        is_reFill = false;
        is_break = false;
    }
    if (is_break)
        emit parent_close();
    emit th_finished();
    if (is_break) {
        this->close();
        return;
    }

    appendPlainText("\nВсего файлов:     " % reformat_size(QString::number(fcnt)) % ",\nкаталогов:     "
                    % reformat_size(QString::number(dcnt)) % "\n\nОбщий размер:     "
                    % reformat_size(QString::number(all_size)) % " Б (" % reformat_size_2(all_size) % ")");
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


//TextWidget
TextWidget::TextWidget(const QString &_fpath, const AppSettings *appSettings, QWidget *parent, bool _is_xml, FCodes _cod) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    file = new QFile(_fpath);

    fpath = _fpath;
    is_xml = _is_xml;
    cod = _cod;
    f_size = file->size();

    gridLayout = new QGridLayout(this);

    plainTextEdit = new QPlainTextEdit(this);
    plainTextEdit->setReadOnly(true);
    plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
    plainTextEdit->setFont(*appSettings->lister_font);
    gridLayout->addWidget(plainTextEdit, 1, 0, 1, 6);

    verticalSlider = new QSlider(this);
    verticalSlider->setOrientation(Qt::Vertical);
    verticalSlider->setInvertedAppearance(true);
    verticalSlider->setInvertedControls(true);
    gridLayout->addWidget(verticalSlider, 1, 6);

    pushButton_up = new QPushButton(this);
    pushButton_up->setMaximumWidth(60);
    pushButton_up->setText("up");
    gridLayout->addWidget(pushButton_up, 2, 0);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer, 2, 1);

    lineEdit_size_now = new QLineEdit(this);
    lineEdit_size_now->setMaximumWidth(200);
    gridLayout->addWidget(lineEdit_size_now, 2, 2);

    label_all_size = new QLabel(this);
    label_all_size->setMaximumWidth(300);
    gridLayout->addWidget(label_all_size, 2, 3);

    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer_2, 2, 4);

    pushButton_down = new QPushButton(this);
    pushButton_down->setMaximumWidth(60);
    pushButton_down->setText("down");
    gridLayout->addWidget(pushButton_down, 2, 5);

    label_all_size->setText("из " + QString::number(ceil((double)f_size/1048576.0)) + " МБ");
    lineEdit_size_now->setText("1");
    verticalSlider->setMinimum(1);
    verticalSlider->setMaximum(ceil((double)f_size/1048576.0));
    verticalSlider->setSingleStep(1);
    verticalSlider->setPageStep(1);

    connect(verticalSlider, SIGNAL(valueChanged(int)), this, SLOT(verticalSlider_valueChanged(int)));
    connect(lineEdit_size_now, SIGNAL(returnPressed()), this, SLOT(on_lineEdit_size_now_returnPressed()));
    connect(pushButton_up, SIGNAL(clicked()), this, SLOT(on_pushButton_up_clicked()));
    connect(pushButton_down, SIGNAL(clicked()), this, SLOT(on_pushButton_down_clicked()));

    setFont(*appSettings->main_font);
    plainTextEdit->setFont(*appSettings->lister_font);


    if (file->exists()) {
        Fill();
    } else {
        close();
        if (parentWidget())
            parentWidget()->close();
    }
}

TextWidget::~TextWidget()
{
    if (file->isOpen())
        file->close();
    delete file;
}

void TextWidget::reFill(const QString& _fpath, bool _is_xml, FCodes _cod)
{
    plainTextEdit->clear();
    fpath = _fpath;
    is_xml = _is_xml;
    cod = _cod;

    if (file->isOpen())
        file->close();
    delete file;

    file = new QFile(fpath);
    if (!file->exists()) {
        close();
        if (parentWidget())
            parentWidget()->close();
    }

    Fill();
}

void TextWidget::Fill()
{
    if (!file->open(QIODevice::ReadOnly))
        return;
    //this->close();

    if (f_size < 1048576) {
        pushButton_up->hide();
        pushButton_down->hide();
        lineEdit_size_now->hide();
        label_all_size->hide();
        verticalSlider->hide();
    } else {
        pushButton_up->show();
        pushButton_down->show();
        lineEdit_size_now->show();
        label_all_size->show();
        verticalSlider->show();
    }
    updateText();
}

void TextWidget::updateText()
{
    if (f_size < 1048576) {
        plainTextEdit->clear();
        file->seek(0);
        if (cod == FCodes::UTF8) {
            if (is_xml)
                plainTextEdit->textCursor().insertHtml(file->readAll());
            else
                plainTextEdit->setPlainText(file->readAll());
        } else if (cod == FCodes::Local) {
            if (is_xml)
                plainTextEdit->textCursor().insertHtml(QString::fromLocal8Bit(file->readAll()));
            else
                plainTextEdit->setPlainText(QString::fromLocal8Bit(file->readAll()));
        }
    } else
        verticalSlider_valueChanged(verticalSlider->value());
}

void TextWidget::on_pushButton_up_clicked()
{
    verticalSlider->setValue(verticalSlider->value() - 1);
}

void TextWidget::on_pushButton_down_clicked()
{
    verticalSlider->setValue(verticalSlider->value() + 1);
}

void TextWidget::verticalSlider_valueChanged(int value)
{
    lineEdit_size_now->setText(QString::number(value));
    file->seek((value-1)*1048576);

    if (!(buf = file->read(1048576)).isEmpty()) {
        plainTextEdit->clear();
        if (cod == FCodes::UTF8) {
            if (is_xml)
                plainTextEdit->textCursor().insertHtml(buf);
            else
                plainTextEdit->setPlainText(buf);
        } else if (cod == FCodes::Local) {
            if (is_xml)
                plainTextEdit->textCursor().insertHtml(QString::fromLocal8Bit(buf));
            else
                plainTextEdit->setPlainText(QString::fromLocal8Bit(buf));
        }

        progress += 1048576;
        if (progress > f_size)
            progress = f_size;
    }
}

void TextWidget::on_lineEdit_size_now_returnPressed()
{
    bool ok;
    int now_s = lineEdit_size_now->text().toInt(&ok);
    if (ok) {
        verticalSlider->setValue(now_s);
    }
}


//ImageWidget
ImageWidget::ImageWidget(const QString &_fpath, QWidget *parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    fpath = _fpath;
    label_image = new QLabel(this);
    setWidget(label_image);
    Fill();
}

void ImageWidget::reFill(const QString &_fpath)
{
    fpath = _fpath;
    label_image->clear();
    Fill();
}

void ImageWidget::Fill()
{
    if (!QFileInfo(fpath).isFile())
        return;

    QImage image = QImageReader(fpath).read();
    image = image.scaled(image.size(), Qt::KeepAspectRatio);
    label_image->setFixedSize(image.size());
    label_image->setPixmap(QPixmap::fromImage(image));
}
