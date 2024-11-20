#ifndef COPY_PROCESS_H
#define COPY_PROCESS_H

#include <QObject>
#include <QStringList>

class CopyProcess : public QObject
{
    Q_OBJECT
public:
    CopyProcess(const QString& dir_to, const QStringList& selected_dirs, const QStringList& selected_files, bool remove_after);

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
    long long int all_size = 0;
    ///кол-во файлов
    long long int all_count = 0;
    ///скопированный объем
    long long int complited_size = 0;
    ///кол-во скопированных файлов
    long long int complited_count = 0;

    ///файл уже существует
    ///0 - переименовать, 1 - переименовать все,
    ///2 - пропустить, 3 - пропустить все,
    ///4 - заменить, 5 - заменить все, 6 - отмена
    int yet_exists_ind = 0;

    ///действия при невозможности копирования
    /// 0 - пропустить, 1 - пропустить все, 2 - прекратить все операции
    int cant_copy_ind = 0;

    ///действия при невозможности удаления
    /// 0 - пропустить, 1 - пропустить все, 2 - прекратить все операции
    int cant_del_ind = 0;

    ///отмена операции
    bool wasCanceled = false;
    ///первичная отмена (приостановка)
    bool wasCanceled_first = false;

public: signals:
    ///устанавливает размер файлов
    void set_all_size(long long int);
    ///устанавливает кол-во файлов
    void set_all_count(long long int);

    ///устанавливает compiled размер файлов
    void set_comp_size(long long int);
    ///устанавливает compiled кол-во файлов
    void set_comp_count(long long int);

    ///начато копирование нового файла -> передано имя
    void update_name_copy(QString name);

    ///папка/файл уже существует
    void yet_exists(QString name);
    ///не удалось выполнить копирование
    void cant_copy(QString str_error);
    ///не удалось выполнить удаление
    void cant_del(QString str_error);

    ///выйти из петли (снятие с паузы)
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
    ///изменение поведения при невозможности копирования
    void change_cant_copy_ind(int val);
    ///изменение поведения при невозможности удаления
    void change_cant_del_ind(int val);
    ///петля (пауза)
    void func_loop();
    ///первичное нажатие отмена -> пауза
    void cancel_clicked_first();
    ///снятие с паузы
    void cancel_unclicked_first();


private slots:
    ///собирает информацию о директории
    void get_dir_info(const QString& dir);
    ///копирование конкретного файла
    int copy_file(const QString& past_name, const QString& new_name);
    ///рекурсивное удаление папки
    bool removeDir(const QString & dirName);
    ///проход по каталогу
    void dir_iter(const QString& dir, QString dir_to, bool remove_after);
};

#endif // COPY_PROCESS_H
