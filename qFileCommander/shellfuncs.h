#ifndef SHELLFUNCS_H
#define SHELLFUNCS_H

#include <QList>
#include <QThread>

namespace shellfuncs {

///вызвать контекстное меню
bool get_context_menu(QStringList& files, int x, int y, void* parentWind);

///копировать в буфер
bool copy_api(QStringList& files, void * parentWind);

///вырезать в буфер
bool cut_api(QStringList& files, void * parentWind);

///вставить из буфера
bool paste_api(QString& dir_to, void * parentWind);

///удалить файлы
bool delete_files_api(QString fname, bool moveToTrash, void * parentWind);

///отобразить свойства
void showProperties(QStringList selected_files);

}


class PasteProcess : public QObject
{
    Q_OBJECT
public:
    QString dir_to;
    void *parentWind;
    PasteProcess(QString _dir_to, void *_parentWind) : dir_to(_dir_to), parentWind(_parentWind) {}
public slots:
    void Work() {

        shellfuncs::paste_api(dir_to, parentWind);

        this->deleteLater();
        this->thread()->exit();
    }
};



#endif // SHELLFUNCS_H
