#ifndef DELETE_FILES_H
#define DELETE_FILES_H

#include "appsettings.h"
#include <QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>


class Delete_Process : public QObject
{
    Q_OBJECT

private:
    ///удаляемые папки
    QStringList selected_dirs;
    ///удаляемые файлы
    QStringList selected_files;
    ///окончательное удаление
    bool is_final;

    ///кол-во удаленных файлов
    long long int complited_count = 0;

    QString disk;

    ///действия при невозможности удаления
    /// 0 - пропустить, 1 - пропустить все, 2 - прекратить все операции
    int cant_del_ind = 0;

    ///отмена операции
    bool wasCanceled = false;
    ///первичная отмена (приостановка)
    bool wasCanceled_first = false;

public:
    Delete_Process(const QStringList& selected_dirs, const QStringList& selected_files, bool is_final);

public: signals:
    ///устанавливает compiled кол-во файлов
    void set_comp_count(long long int);

    ///начато удаление нового файла -> передано имя
    void update_name_del(QString name);

    ///не удалось выполнить удаление
    void cant_del(QString str_error);
    ///выйти из петли (снятие с паузы)
    void signal_loop();
    ///фатальная ошибка
    void error_operation(QString str_error);

public slots:
    void Work();
    ///отмена копирования
    void del_Cancel_clicked();
    ///петля (пауза)
    void func_loop();
    ///первичное нажатие отмена -> пауза
    void cancel_clicked_first();
    ///снятие с паузы
    void cancel_unclicked_first();
    ///изменение поведения при невозможности удаления
    void change_cant_del_ind(int val);


private:
    ///рекурсивное удаление папки
    bool removeDir(const QString & dirName);
};


class Delete_Files : public QObject
{
    Q_OBJECT

public:
    Delete_Files(const AppSettings *appSettings);
    void Work(const QStringList& selected_dirs, const QStringList& selected_files, bool is_final);

signals:
    ///отмена операции
    void cancel_del_clicked();
    ///уйти в петлю (пауза)
    void signal_loop();
    ///выйти из петли (снятие с паузы)
    void signal_exit_loop();
    ///первичное нажатие отмена -> пауза
    void cancel_clicked_first();
    ///снятие с паузы
    void cancel_unclicked_first();
    ///изменение поведения при невозможности удаления
    void change_cant_del_ind(int);
    ///полное завершение операции (del_process)
    void end_operation();

public slots:
    ///обновляем значение progress-bar
    void update_value_progress();
    ///обновляет имя текущего удаления
    void update_name_progress(QString val);
    ///окно с ошибкой
    void v_error(QString str_error);
    ///не удалось выполнить удаление
    void cant_del(QString str_error);


private:
    const QFont *dialog_font;

    QThread *th;
    Delete_Process *dp;

    QDialog *w_progress;
    QProgressBar *pb;
    QLabel *lab_name;
    QLabel *lab_files;
    QPushButton *b_pause;
    QPushButton *b_minimize;
    QPushButton *b_cancel;

    long long int all_count = 0;
    long long int comp_count = 0;

private slots:
    ///моментальная отмена операции
    void hard_cancel_clicked();
    ///пауза/старт
    void pause_operation();
    ///В фоне
    void minimize_clicked();
    ///отмена операции
    void cancel_clicked();
    ///полное завершение операции
    void end_del();

    QString reformat_size(QString str);
};




#endif // DELETE_FILES_H
