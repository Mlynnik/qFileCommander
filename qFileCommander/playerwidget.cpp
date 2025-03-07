#include "playerwidget.h"
#include <QFile>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>


PlayerWidget::PlayerWidget(const QString& _fpath, const AppSettings *appSettings, QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    if (!QFile(_fpath).exists()) {
        close();
    }
    w = appSettings->w;
    h = appSettings->h;
    fpath = _fpath;
    dialog_font = appSettings->dialog_font;
    setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    setFont(*appSettings->main_font);
    setMinimumWidth(round(w*500));
    setMinimumHeight(round(h*60));
    setMaximumHeight(round(h*120));
    if (parent && parent->isWindow()) {
        parent->setMinimumWidth(round(w*500));
        parent->setMinimumHeight(round(h*60));
        parent->setMaximumHeight(round(h*120));
        parent->resize(round(w*500), round(h*80));
    }

    player = new QMediaPlayer(this);

    pushButton_play = new QPushButton(this);
    pushButton_play->setFixedSize(round(w*40), round(h*40));
    pushButton_play->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
    connect(pushButton_play, SIGNAL(clicked()), this, SLOT(play_clicked()));

    label_time_max = new QLabel(this);
    label_time_now = new QLabel(this);

    hSlider = new MySlider(Qt::Horizontal, this);
    hSlider->setMinimumWidth(round(w*200));
    hSlider->setStyleSheet("QSlider::groove {"
                               "border-radius: 5px;"
                               "height: 12px;"
                               "margin: -1px 0;"
                           "}"

                           "QSlider::handle {"
                               "background-color: rgb(217,217,217);"
                               "border: 2px solid #666;"
                               "height: 12px;"
                               "width: 16px;"
                               "margin: -4px 0;"
                               "border-radius: 7px;"
                               "padding: -4px 0px;"
                           "}"

                            "QSlider::add-page {"
                               "background: rgb(192,192,192);"
                           "}"

                            "QSlider::sub-page {"
                               "background: rgb(42,130,213);"
                           "}"
    );
    hSlider->setMinimum(0);
    hSlider->setSingleStep(10);
    hSlider->triggerAction(QAbstractSlider::SliderMove);
    connect(hSlider, SIGNAL(valueChanged(int)), this, SLOT(hSlider_valueChanged(int)));

    pushButton_volume = new QPushButton(this);
    pushButton_volume->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaVolume)));
    pushButton_volume->setFixedSize(round(w*40), round(h*40));
    connect(pushButton_volume, SIGNAL(clicked()), this, SLOT(volume_clicked()));

    vSlider = new MySlider(Qt::Vertical, this);
    vSlider->setMinimum(0);
    vSlider->setMaximum(100);
    vSlider->setSingleStep(10);
    vSlider->setMaximumHeight(round(h*100));
    connect(vSlider, SIGNAL(valueChanged(int)), this, SLOT(vSlider_valueChanged(int)));
    vSlider->setVisible(false);

    label_volume = new QLabel(this);

    QHBoxLayout *lay = new QHBoxLayout();
    lay->setAlignment(Qt::AlignTop);
    lay->addWidget(pushButton_play);
    lay->addWidget(hSlider, 1);
    lay->addWidget(label_time_now);
    lay->addWidget(label_time_max);
    lay->addWidget(pushButton_volume);
    lay->addWidget(vSlider);
    lay->addWidget(label_volume);
    setLayout(lay);

    pushButton_play->setFocusPolicy(Qt::NoFocus);
    hSlider->setFocusPolicy(Qt::NoFocus);
    pushButton_volume->setFocusPolicy(Qt::NoFocus);
    vSlider->setFocusPolicy(Qt::NoFocus);

    connect(player, SIGNAL(errorOccurred(QMediaPlayer::Error,QString)), this, SLOT(v_error(QMediaPlayer::Error,QString)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(setMaximumDuration(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(setPosition(qint64)));
    connect(player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)), this, SLOT(playingChanged(QMediaPlayer::PlaybackState)));
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(fpath));
    connect(audioOutput, SIGNAL(volumeChanged(float)), this, SLOT(volumeChanged(float)));
    audioOutput->setVolume(0.6);
    play_clicked();
}

PlayerWidget::~PlayerWidget()
{
    if (parentWidget() && parentWidget()->isWindow()) {
        parentWidget()->setMinimumSize(0, 0);
        parentWidget()->setMaximumSize(16777215, 16777215);
        parentWidget()->resize(round(w*800), round(h*600));
        parentWidget()->setWindowState(parentWidget()->windowState() & ~(Qt::WindowMinimized
                                                         | Qt::WindowMaximized
                                                         | Qt::WindowFullScreen));
    }
}

void PlayerWidget::reFill(const QString &_fpath)
{
    if (QFile(_fpath).exists()) {
        fpath = _fpath;
        player->stop();
        player->setSource(QUrl::fromLocalFile(fpath));
        play_clicked();
    }
}

void PlayerWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()){
    case Qt::Key_Escape:
        this->close();
        QApplication::sendEvent(parent(), e);
        break;
    case Qt::Key_Space:
    case Qt::Key_MediaTogglePlayPause:
        play_clicked();
        break;
    case Qt::Key_MediaPlay:
    case Qt::Key_Play:
        if (!player->isPlaying())
            play_clicked();
        break;
    case Qt::Key_MediaPause:
    case Qt::Key_Pause:
        if (player->isPlaying())
            play_clicked();
        break;
    case Qt::Key_Left:
        hSlider->setValue(hSlider->value() - 10);
        break;
    case Qt::Key_Right:
        hSlider->setValue(hSlider->value() + 10);
        break;
    case Qt::Key_Down:
        audioOutput->setVolume(roundf((audioOutput->volume() - 0.1)*10)/10);
        break;
    case Qt::Key_Up:
        audioOutput->setVolume(roundf((audioOutput->volume() + 0.1)*10)/10);
        break;
    }
}

void PlayerWidget::play_clicked()
{
    if (!player->isPlaying()) {
        pushButton_play->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPause)));
        player->play();
    } else {
        pushButton_play->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
        player->pause();
    }
}

void PlayerWidget::playingChanged(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::StoppedState) {
        pushButton_play->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
        hSlider->setValue(0);
    }
}

QString sec_to_time(qint64 i) {
    short int s = i % 60; i /= 60;
    short int m = i % 60; i /= 60;
    if (s < 10 && m < 10)
        return (QString::number(i) % ":0" % QString::number(m) % ":0" % QString::number(s));
    if (s < 10)
        return (QString::number(i) % ":" % QString::number(m) % ":0" % QString::number(s));
    if (m < 10)
        return (QString::number(i) % ":0" % QString::number(m) % ":" % QString::number(s));
    return (QString::number(i) % ":" % QString::number(m) % ":" % QString::number(s));
}

void PlayerWidget::setMaximumDuration(qint64 md)
{
    setWindowTitle("Player - [" % fpath % "]");
    md /= 1000;
    label_time_now->setText(sec_to_time(0));
    label_time_max->setText("/ " + sec_to_time(md));
    hSlider->setMaximum(md);
    hSlider->setValue(0);
}

void PlayerWidget::setPosition(qint64 pos)
{
    hSlider->blockSignals(true);
    pos /= 1000;
    label_time_now->setText(sec_to_time(pos));
    hSlider->setValue(pos);
    hSlider->blockSignals(false);
}

void PlayerWidget::hSlider_valueChanged(int value)
{
    player->setPosition((qint64)value * 1000);
}

void PlayerWidget::vSlider_valueChanged(int value)
{
    audioOutput->setVolume((float)value/100);
}

void PlayerWidget::volumeChanged(float volume)
{
    int v = 100*volume;
    vSlider->blockSignals(true);
    vSlider->setValue(v);
    label_volume->setText(QString::number(v) + "%");
    vSlider->blockSignals(false);
}

void PlayerWidget::volume_clicked()
{
    if (vSlider->isVisible())
        vSlider->setVisible(false);
    else
        vSlider->setVisible(true);
}

void PlayerWidget::v_error(QMediaPlayer::Error error, QString errorString)
{
    if (!error)
        return;

    if (error == QMediaPlayer::ResourceError) {
        errorString = "QMediaPlayer::ResourceError\n" % errorString % "\n\nНе удалось разрешить проблему с медиа-ресурсом.\n";
    } else if (error == QMediaPlayer::FormatError) {
        errorString = "QMediaPlayer::FormatError\n" % errorString % "\n\nФормат медиаресурса (полностью) не поддерживается. Воспроизведение может быть возможно, но без аудио- или видеокомпонента.\n";
    } else if (error == QMediaPlayer::NetworkError) {
        errorString = "QMediaPlayer::NetworkError\n" % errorString % "\n\nПроизошла сетевая ошибка.\n";
    } else if (error == QMediaPlayer::AccessDeniedError) {
        errorString = "QMediaPlayer::AccessDeniedError\n" % errorString % "\n\nНет соответствующих разрешений для воспроизведения медиаресурса.\n";
    }

    QMessageBox v_err;
    v_err.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_err.setFont(*dialog_font);
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(errorString);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
    if (error != QMediaPlayer::FormatError)
        close();
}
