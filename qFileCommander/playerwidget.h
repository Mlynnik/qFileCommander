#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include "appsettings.h"
#include <QWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionComplex>


class MySlider : public QSlider
{
public:
    MySlider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent) {}
protected:
    void mousePressEvent ( QMouseEvent * event )
    {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        if (event->button() == Qt::LeftButton && !sr.contains(event->pos()))
        {
            int newVal;
            if (orientation() == Qt::Vertical)
                newVal = minimum() + ((maximum()-minimum() + 1) * (height()-event->pos().y())) / height();
            else
                newVal = minimum() + ((maximum()-minimum() + 1) * event->pos().x()) / width();

            if (invertedAppearance() == true)
                setValue( maximum() - newVal );
            else
                setValue(newVal);

            event->accept();
        }
        else {
            QSlider::mousePressEvent(event);
        }
    }
};


class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    PlayerWidget(const QString &_fpath, const AppSettings *appSettings, QWidget *parent = nullptr);
    ~PlayerWidget();

private:
    float w;
    float h;
    const QFont *dialog_font;
    QString fpath;
    QPushButton *pushButton_play;
    QLabel *label_time_max;
    QLabel *label_time_now;
    QPushButton *pushButton_volume;
    QLabel *label_volume;

    MySlider *hSlider;
    MySlider *vSlider;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;


private slots:
    void keyPressEvent(QKeyEvent* e);

    void setMaximumDuration(qint64 md);
    void setPosition(qint64 pos);
    void play_clicked();
    void playingChanged(QMediaPlayer::PlaybackState newState);
    void hSlider_valueChanged(int value);
    void vSlider_valueChanged(int value);
    void volumeChanged(float volume);
    void volume_clicked();

    void v_error(QMediaPlayer::Error error, QString errorString);
};
#endif // PLAYERWIDGET_H
