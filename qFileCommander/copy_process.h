#ifndef COPY_PROCESS_H
#define COPY_PROCESS_H

#include <QObject>
#include <QStringList>

class CopyProcess : public QObject
{
    Q_OBJECT
public:
    CopyProcess(QString dir_to, QStringList selected_dirs, QStringList selected_files, bool remove_after);

private:
    ///куда
    QString dir_to;
    ///копируемые папки
    QStringList selected_dirs;
    ///копируемые файлы
    QStringList selected_files;
    ///удалять после копирования
    bool remove_after;
    ///объем копирования
    unsigned long long int all_size = 0;
    ///кол-во файлов
    unsigned long long int all_count = 0;
    ///скопированный объем
    unsigned long long int complited_size = 0;
    ///кол-во скопированных файлов
    unsigned long long int complited_count = 0;

    ///файл уже существует
    ///0 - переименовать, 1 - переименовать все,
    ///2 - пропустить, 3 - пропустить все,
    ///4 - заменить, 5 - заменить все, 6 - отмена
    int yet_exists_ind = 0;

    ///отмена операции
    bool wasCanceled = false;
    ///первичная отмена (приостановка)
    bool wasCanceled_first = false;

public: signals:
    ///сигнал передает текущий процент копии
    void update_value_copy(int val);
    ///начато копирование нового файла -> передано имя
    void update_name_copy(QString name);
    ///папка/файл уже существует
    void yet_exists(QString name);
    ///отправить в петлю (пауза)
    void signal_loop();
    ///фатальная ошибка
    void error_operation(QString str_error);

public slots:
    ///общий процесс копирования
    void Copy();
    ///отмена копирования
    void copy_Cancel_clicked();
    ///изменение поведения при совпадении в папке назначения
    void change_yet_exists_ind(int val);
    ///петля (пауза)
    void func_loop();
    ///первичное нажатие отмена -> пауза
    void cancel_clicked_first();
    ///снятие с паузы
    void cancel_unclicked_first();

private slots:
    ///собирает информацию о директории
    void get_dir_info(QString dir);
    ///копирование конкретного файла
    int copy_file(QString past_name, QString new_name);
};

#endif // COPY_PROCESS_H
