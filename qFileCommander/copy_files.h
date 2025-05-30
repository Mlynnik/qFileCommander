#ifndef COPY_FILES_H
#define COPY_FILES_H

#include "copy_process.h"
#include "appsettings.h"
#include <QMessageBox>
#include <QDialog>
#include <QProgressBar>
#include <QLabel>

class Copy_files : public QObject
{
    Q_OBJECT

public:
    Copy_files(const AppSettings *appSettings);
    void Work(QString dir_to, const QStringList& selected_dirs, const QStringList& selected_files, bool remove_after);

signals:
    ///отмена копирования
    void cancel_copy_clicked();
    ///изменение поведения при совпадении в папке назначения
    void change_yet_exists_ind(int val);
    ///уйти в петлю (пауза)
    void signal_loop();
    ///выйти из петли (снятие с паузы)
    void signal_exit_loop();
    ///первичное нажатие отмена -> пауза
    void cancel_clicked_first();
    ///снятие с паузы
    void cancel_unclicked_first();
    ///изменение поведения при невозможности копирования
    void change_cant_copy_ind(int);
    ///изменение поведения при невозможности удаления
    void change_cant_del_ind(int);

    ///полное завершение операции (copy_process)
    void end_operation();

public slots:
    ///обновляем значение progress-bar
    void update_value_progress();
    ///обновляет имя текущего копирования
    void update_name_progress(QString val);
    ///окно с ошибкой
    void v_error(QString str_error);
    ///не удалось выполнить копирование
    void cant_copy(QString str_error);
    ///не удалось выполнить удаление
    void cant_del(QString str_error);

private slots:
    ///моментальная отмена операции
    void hard_cancel_clicked();
    ///пауза/старт
    void pause_operation();
    ///В фоне
    void minimize_clicked();
    ///отмена операции
    void cancel_clicked();
    ///изменение поведения при совпадении в папке назначения
    void set_yet_exists_ind(QString val);
    ///полное завершение операции
    void end_copy();

private:
    const QFont *dialog_font;

    QThread *th;
    CopyProcess *cd;


    QDialog *w_progress;
    QProgressBar *pb;
    QLabel *lab_name;
    QLabel *lab_files;
    QLabel *lab_size;
    QPushButton *b_pause;
    QPushButton *b_minimize;
    QPushButton *b_cancel;

    long long int all_size = 0;
    long long int all_count = 0;
    long long int comp_size = 0;
    long long int comp_count = 0;

    ///вызывается при наличии дубликата папки/файла в дикертории назначения
    QMessageBox msgBox;
};

#endif // COPY_FILES_H
