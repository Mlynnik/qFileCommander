#include "copy_process.h"
#include "qeventloop.h"
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QThread>
#include <shlobj.h>

CopyProcess::CopyProcess(const QString &dir_to, const QStringList &selected_dirs, const QStringList &selected_files, bool remove_after)
    :dir_to(dir_to), selected_dirs(selected_dirs), selected_files(selected_files), remove_after(remove_after)
{
    wasCanceled = false;
}

void CopyProcess::Copy()
{
    for (int i = 0; i < selected_dirs.length(); ++i) {
        if (!get_dir_info(selected_dirs[i])) {
            this->deleteLater();
            this->thread()->exit();
            return;
        }
    }
    for (int i = 0; i < selected_files.length(); ++i) {
        if (QFile().exists(selected_files[i])) {
            all_size += QFile(selected_files[i]).size();
            ++all_count;
        }
    }

    if (all_size == 0)
        all_size = 1;
    if (all_count == 0)
        all_count = 1;

    emit set_all_size(all_size);
    emit set_all_count(all_count);

    QString past_name;
    QString new_name;
    QString temp_name;


    QString path_old;
    if (selected_dirs.length() > 0)
        path_old = selected_dirs[0].left(selected_dirs[0].lastIndexOf("/")) % "/";
    else if (selected_files.length() > 0)
        path_old = selected_files[0].left(selected_files[0].lastIndexOf("/")) % "/";
    else
        goto lab_end;

    past_disk = path_old.split("/").first().toLower();
    new_disk = dir_to.split("/").first().toCaseFolded().toLower();

    st_inf = QStorageInfo(new_disk);
    {
        long long int av_s = st_inf.bytesAvailable();
        if (av_s < all_size) {
            emit error_operation("Недостаточно свободного места!");
            goto lab_end;
        }
    }

    //операция перемещение на одном диске
    if (remove_after && (past_disk == new_disk)) {
        int all_count_replace = selected_dirs.length() + selected_files.length();
        emit set_all_size(0);
        emit set_all_count(all_count_replace);
        int now_count_replace = 0;
        for (int i = 0; i < selected_dirs.length(); i++) {
            past_name = selected_dirs[i].split("/").last();
            new_name = dir_to % past_name;
            if (QDir().exists(new_name)) {
                if (yet_exists_ind == 1) {
                    temp_name = new_name;
                    int j = 0;
                    while (QDir(new_name).exists()) {
                        j++;
                        new_name = temp_name % " (" % QString::number(j) % ")";
                    }
                } else if (yet_exists_ind == 3)
                    continue;
                else if (yet_exists_ind == 5) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    if (!removeDir(new_name)) {
                        if (cant_del_ind == 0) {
                            emit cant_del(new_name);
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            goto lab_end;
                        continue;
                    }
                } else {
                    emit yet_exists("Каталог с именем '" % past_name % "' уже существует.");
                    func_loop();

                    if (yet_exists_ind == 1) {
                        temp_name = new_name;
                        int j = 0;

                        while (QDir(new_name).exists()) {
                            j++;
                            new_name = temp_name % " (" % QString::number(j) % ")";
                        }
                    }
                    else if (yet_exists_ind == 2)
                        continue;
                    else if (yet_exists_ind == 3) {
                        continue;
                    } else if (yet_exists_ind == 4) {
                        SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                        if (!removeDir(new_name)) {
                            if (cant_del_ind == 0) {
                                emit cant_del(new_name);
                                func_loop();
                            }
                            if (cant_del_ind == 2)
                                goto lab_end;
                            continue;
                        }
                    } else if (yet_exists_ind == 5) {
                        SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                        if (!removeDir(new_name)) {
                            if (cant_del_ind == 0) {
                                emit cant_del(new_name);
                                func_loop();
                            }
                            if (cant_del_ind == 2)
                                goto lab_end;
                            continue;
                        }
                    } else if (yet_exists_ind == 6) {
                        return;
                    } else {
                        temp_name = new_name;
                        int j = 0;
                        while (QDir(new_name).exists()) {
                            j++;
                            new_name = temp_name % " (" % QString::number(j) % ")";
                        }
                    }
                }
            }

            if (!QDir().rename(selected_dirs[i], new_name)) {
                if (!QDir(past_disk).exists() || !QDir(new_disk).exists()) {
                    emit error_operation("Операция прервана!\nУстройство извлечено!");
                    goto lab_end;
                }
            }

            if (QDir(selected_dirs[i]).exists()) {
                SetFileAttributesA(selected_dirs[i].toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                if (!removeDir(selected_dirs[i])) {
                    if (wasCanceled_first)
                        func_loop();
                    if (wasCanceled || (cant_del_ind == 2))
                        goto lab_end;

                    if (cant_del_ind == 0) {
                        emit cant_del(selected_dirs[i]);
                        func_loop();
                    }
                    if (cant_del_ind == 2)
                        goto lab_end;
                    continue;
                }
            }

            if (wasCanceled_first)
                func_loop();

            if (wasCanceled || (cant_copy_ind == 2) || (cant_del_ind == 2))
                goto lab_end;

            now_count_replace++;
            emit set_comp_count(now_count_replace);
        }

        for (int i = 0; i < selected_files.length(); i++) {
            past_name = selected_files[i].split("/").last();
            new_name = dir_to % past_name;
            if (QFile().exists(new_name)) {
                QString name_f = new_name;
                QString type_f;
                if (past_name.indexOf(".") != -1) {
                    type_f = "." % past_name.split(".").last();
                    name_f.remove(type_f);
                } else {
                    type_f = "";
                }

                if (yet_exists_ind == 1) {
                    temp_name = name_f;
                    int j = 0;
                    while (QFile(new_name).exists()) {
                        j++;
                        new_name = temp_name % " (" % QString::number(j) % ")" % type_f;
                    }
                } else if (yet_exists_ind == 3)
                    continue;
                else if (yet_exists_ind == 5) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    QFile file(new_name);
                    if (!file.remove()) {
                        if (cant_del_ind == 0) {
                            emit cant_del(selected_files[i] % "\n\n" % file.errorString());
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            goto lab_end;
                    }
                } else {
                    emit yet_exists("Файл с именем '" % past_name % "' уже существует.");
                    func_loop();

                    if (yet_exists_ind == 1) {
                        temp_name = name_f;
                        int j = 0;
                        while (QFile(new_name).exists()) {
                            j++;
                            new_name = temp_name % " (" % QString::number(j) % ")" % type_f;
                        }
                    }
                    else if (yet_exists_ind == 2)
                        continue;
                    else if (yet_exists_ind == 3) {
                        continue;
                    } else if (yet_exists_ind == 4) {
                        SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                        QFile file(new_name);
                        if (!file.remove()) {
                            if (cant_del_ind == 0) {
                                emit cant_del(selected_files[i] % "\n\n" % file.errorString());
                                func_loop();
                            }
                            if (cant_del_ind == 2)
                                goto lab_end;
                        }
                    } else if (yet_exists_ind == 5) {
                        SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                        QFile file(new_name);
                        if (!file.remove()) {
                            if (cant_del_ind == 0) {
                                emit cant_del(selected_files[i] % "\n\n" % file.errorString());
                                func_loop();
                            }
                            if (cant_del_ind == 2)
                                goto lab_end;
                        }
                    } else if (yet_exists_ind == 6) {
                        this->deleteLater();
                        this->thread()->exit();
                        return;
                    } else {
                        temp_name = name_f;
                        int j = 0;
                        while (QFile(new_name).exists()) {
                            j++;
                            new_name = temp_name % " (" % QString::number(j) % ")" % type_f;
                        }
                    }
                }
            }

            if (!QFile().rename(selected_files[i], new_name)) {
                if (!QDir(past_disk).exists() || !QDir(new_disk).exists()) {
                    emit error_operation("Операция прервана!\nУстройство извлечено!");
                    goto lab_end;
                }
            }

            if (QFile(selected_files[i]).exists()) {
                SetFileAttributesA(selected_files[i].toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                QFile file(selected_files[i]);
                if (!file.remove()) {
                    if (wasCanceled_first)
                        func_loop();
                    if (wasCanceled || (cant_del_ind == 2))
                        goto lab_end;

                    if (cant_del_ind == 0) {
                        emit cant_del(selected_files[i] % "\n\n" % file.errorString());
                        func_loop();
                    }
                    if (cant_del_ind == 2)
                        goto lab_end;
                }
            }

            if (wasCanceled_first)
                func_loop();

            if (wasCanceled || (cant_copy_ind == 2) || (cant_del_ind == 2))
                goto lab_end;

            now_count_replace++;
            emit set_comp_count(now_count_replace);
        }

        this->deleteLater();
        this->thread()->exit();
        return;
    }

    for (int i = 0; i < selected_dirs.length(); i++) {
        past_name = selected_dirs[i].split("/").last();
        new_name = dir_to % past_name;
        if (QDir().exists(new_name)) {
            if (yet_exists_ind == 1) {
                temp_name = new_name;
                int j = 0;
                while (QDir(new_name).exists()) {
                    j++;
                    new_name = temp_name % " (" % QString::number(j) % ")";
                }
            } else if (yet_exists_ind == 3)
                continue;
            else if (yet_exists_ind == 5) {
                SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                if (!removeDir(new_name)) {
                    if (cant_del_ind == 0) {
                        emit cant_del(new_name);
                        func_loop();
                    }
                    if (cant_del_ind == 2)
                        goto lab_end;
                    continue;
                }
            } else {
                emit yet_exists("Каталог с именем '" % past_name % "' уже существует.");
                func_loop();

                if (yet_exists_ind == 1) {
                    temp_name = new_name;
                    int j = 0;

                    while (QDir(new_name).exists()) {
                        j++;
                        new_name = temp_name % " (" % QString::number(j) % ")";
                    }
                }
                else if (yet_exists_ind == 2)
                    continue;
                else if (yet_exists_ind == 3) {
                    continue;
                } else if (yet_exists_ind == 4) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    if (!removeDir(new_name)) {
                        if (cant_del_ind == 0) {
                            emit cant_del(new_name);
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            goto lab_end;
                        continue;
                    }
                } else if (yet_exists_ind == 5) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    if (!removeDir(new_name)) {
                        if (cant_del_ind == 0) {
                            emit cant_del(new_name);
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            goto lab_end;
                        continue;
                    }
                } else if (yet_exists_ind == 6) {
                    goto lab_end;
                } else {
                    temp_name = new_name;
                    int j = 0;
                    while (QDir(new_name).exists()) {
                        j++;
                        new_name = temp_name % " (" % QString::number(j) % ")";
                    }
                }
            }
        }

        if (!QDir().mkdir(new_name)) {
            if (!QDir(past_disk).exists() || !QDir(new_disk).exists()) {
                emit error_operation("Операция прервана!\nУстройство извлечено!");
                goto lab_end;
            }
        }

        if (dir_iter(selected_dirs[i], new_name, remove_after))
            goto lab_end;


        if (wasCanceled || (cant_copy_ind == 2) || (cant_del_ind == 2))
            goto lab_end;

        if (remove_after) {
            SetFileAttributesA(selected_dirs[i].toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
            if (!removeDir(selected_dirs[i])) {
                if (cant_del_ind == 0) {
                    emit cant_del(selected_dirs[i]);
                    func_loop();
                }
                if (cant_del_ind == 2)
                    goto lab_end;
            }
        }
    }

    for (int i = 0; i < selected_files.length(); i++) {
        past_name = selected_files[i].split("/").last();
        new_name = dir_to % past_name;
        if (QFile().exists(new_name)) {
            QString name_f = new_name;
            QString type_f;
            if (past_name.indexOf(".") != -1) {
                type_f = "." % past_name.split(".").last();
                name_f.remove(type_f);
            } else {
                type_f = "";
            }


            if (yet_exists_ind == 1) {
                temp_name = name_f;
                int j = 0;
                while (QFile(new_name).exists()) {
                    j++;
                    new_name = temp_name % " (" % QString::number(j) % ")" % type_f;
                }
            } else if (yet_exists_ind == 3)
                continue;
            else if (yet_exists_ind == 5) {
                SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                QFile file(new_name);
                if (!file.remove()) {
                    if (cant_del_ind == 0) {
                        emit cant_del(new_name % "\n\n" % file.errorString());
                        func_loop();
                    }
                    if (cant_del_ind == 2)
                        goto lab_end;
                    continue;
                }
            } else {
                emit yet_exists("Файл с именем '" % past_name % "' уже существует.");
                func_loop();

                if (yet_exists_ind == 1) {
                    temp_name = name_f;
                    int j = 0;
                    while (QFile(new_name).exists()) {
                        j++;
                        new_name = temp_name % " (" % QString::number(j) % ")" % type_f;
                    }
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                }
                else if (yet_exists_ind == 2)
                    continue;
                else if (yet_exists_ind == 3) {
                    continue;
                } else if (yet_exists_ind == 4) {
                    QString temp_new_name = QFileInfo(new_name).absoluteFilePath();
                    //SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    SetFileAttributesA(temp_new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    QFile file(new_name);
                    if (!file.remove()) {
                        if (cant_del_ind == 0) {
                            emit cant_del(new_name % "\n\n" % file.errorString());
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            goto lab_end;
                        continue;
                    }
                } else if (yet_exists_ind == 5) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    QFile file(new_name);
                    if (!file.remove()) {
                        if (cant_del_ind == 0) {
                            emit cant_del(new_name % "\n\n" % file.errorString());
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            goto lab_end;
                        continue;
                    }
                } else if (yet_exists_ind == 6) {
                    goto lab_end;
                } else {
                    temp_name = name_f;
                    int j = 0;
                    while (QFile(new_name).exists()) {
                        j++;
                        new_name = temp_name % " (" % QString::number(j) % ")" % type_f;
                    }
                }
            }
        }

        if (wasCanceled || (cant_copy_ind == 2) || (cant_del_ind == 2))
            goto lab_end;

        if (copy_file(selected_files[i], new_name))
            goto lab_end;
        if (remove_after) {
            SetFileAttributesA(selected_files[i].toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);

            QFile file(selected_files[i]);
            if (!file.remove()) {
                if (cant_del_ind == 0) {
                    emit cant_del(selected_files[i] % "\n\n" % file.errorString());
                    func_loop();
                }
                if (cant_del_ind == 2)
                    goto lab_end;
            }
        }
    }

    lab_end:
    this->deleteLater();
    this->thread()->exit();
}

void CopyProcess::copy_Cancel_clicked()
{
    wasCanceled = true;
    wasCanceled_first = false;
    emit signal_loop();
}

void CopyProcess::change_yet_exists_ind(int val)
{
    yet_exists_ind = val;
    emit signal_loop();
}

void CopyProcess::change_cant_copy_ind(int val)
{
    cant_copy_ind = val;
    emit signal_loop();
}

///рекурсивное удаление папки
bool CopyProcess::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            SetFileAttributesA(info.absoluteFilePath().toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());

                if (!result) {
                    if (wasCanceled_first)
                        func_loop();
                    if (wasCanceled || (cant_del_ind == 2))
                        return false;

                    if (!QDir(dirName.split("/").first()).exists()) {
                        emit error_operation("Операция прервана!\nУстройство извлечено!");
                        return false;
                    }
                    if (cant_del_ind == 0) {
                        emit cant_del(info.absoluteFilePath());
                        func_loop();
                    }
                    if (cant_del_ind == 2)
                        return result;
                }
            } else {
                QFile file(info.absoluteFilePath());

                if (!file.remove()) {
                    result = false;
                    if (wasCanceled_first)
                        func_loop();
                    if (wasCanceled || (cant_del_ind == 2))
                        return false;

                    if (!QDir(dirName.split("/").first()).exists()) {
                        emit error_operation("Операция прервана!\nУстройство извлечено!");
                        return false;
                    }
                    if (cant_del_ind == 0) {
                        emit cant_del(info.absoluteFilePath() % "\n\n" % file.errorString());
                        func_loop();
                    }
                    if (cant_del_ind == 2)
                        return result;
                }
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

int CopyProcess::dir_iter(const QString &dir, QString dir_to, bool remove_after)
{
    if (QDir().exists(dir)) {
        QDirIterator it(dir, QDir::Dirs | QDir::Files | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QString past_name;
        QString new_name;

        QDir pd(past_disk);
        QDir nd(new_disk);

        if (dir_to.endsWith("/"))
            dir_to.remove("/");

        emit update_name_copy(dir);
        while (it.hasNext()) {
            if (wasCanceled || (cant_copy_ind == 2) || (cant_del_ind == 2))
                return 1;


            it.next();
            if (it.fileInfo().isDir()) {
                past_name = it.filePath();
                new_name = it.filePath().replace(dir, dir_to);
                if (QDir().exists(new_name)) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);

                    if (!removeDir(new_name)) {
                        if (wasCanceled_first)
                            func_loop();
                        if (wasCanceled || (cant_del_ind == 2))
                            return 1;

                        if (cant_del_ind == 0) {
                            emit cant_del(new_name);
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            return 1;
                        continue;
                    }
                }

                if (!QDir().mkdir(new_name)) {
                    if (!pd.exists() || !nd.exists())
                        return 1;
                }
            } else if (it.fileInfo().isFile()) {
                past_name = it.filePath();
                new_name = it.filePath().replace(dir, dir_to);
                if (QFile().exists(new_name)) {
                    SetFileAttributesA(new_name.toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    QFile file(new_name);
                    if (!file.remove()) {
                        if (wasCanceled_first)
                            func_loop();
                        if (wasCanceled || (cant_del_ind == 2))
                            return 1;

                        if (cant_del_ind == 0) {
                            emit cant_del(new_name % "\n\n" % file.errorString());
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            return 1;
                        continue;
                    }
                }
                int res_copy = copy_file(past_name, new_name);
                if (res_copy == 2)
                    return 1;
                if (res_copy == 1)
                    continue;

                if (remove_after) {
                    SetFileAttributesA(QFileInfo(past_name).absoluteFilePath().toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
                    QFile file(past_name);
                    if (!file.remove()) {
                        if (wasCanceled_first)
                            func_loop();
                        if (wasCanceled || (cant_del_ind == 2))
                            return 1;

                        if (cant_del_ind == 0) {
                            emit cant_del(past_name % "\n\n" % file.errorString());
                            func_loop();
                        }
                        if (cant_del_ind == 2)
                            return 1;
                        continue;
                    }
                }
            }

        }
    }
    return 0;
}

void CopyProcess::change_cant_del_ind(int val)
{
    cant_del_ind = val;
    emit signal_loop();
}

void CopyProcess::func_loop()
{
    QEventLoop pause;
    connect(this, SIGNAL(signal_loop()), &pause, SLOT(quit()));
    pause.exec();
}

void CopyProcess::cancel_clicked_first()
{
    wasCanceled_first = true;
}

void CopyProcess::cancel_unclicked_first()
{
    wasCanceled_first = false;
    emit signal_loop();
}

bool CopyProcess::get_dir_info(const QString &dir)
{
    if (QDir().exists(dir)) {
        if (QFileInfo(dir_to).absoluteFilePath().append("/").contains(QFileInfo(dir).absoluteFilePath().append("/"))) {
            emit error_operation("Нельзя копировать каталог в его же подкаталог!");
            return false;
        }
        QDirIterator it(dir, QDir::Files | QDir::Hidden | QDir::System, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            all_size += it.fileInfo().size();
            ++all_count;
        }
    }
    return true;
}

int CopyProcess::copy_file(const QString &past_name, const QString &new_name)
{
    emit update_name_copy(past_name);

    QFile file(past_name);
    QFile dest(new_name);

    if (!file.open(QIODevice::ReadOnly)) {
        if (!QDir(past_name.split("/").first()).exists()) {
            emit error_operation("Операция прервана!\nУстройство извлечено!");
            return 2;
        }

        if (cant_copy_ind == 0) {
            emit cant_copy("Не могу открыть: " % file.fileName() % "\n\n" % file.errorString());
            func_loop();
        }

        if (cant_copy_ind == 2)
            return 1;

        --all_count;
        all_size -= file.size();
        if (all_size < 1)
            all_size = 1;
        if (all_count < 1)
            all_count = 1;

        return 0;
    }
    if (!dest.open(QIODevice::WriteOnly)) {
        file.close();
        if (!QDir(new_name.split("/").first()).exists()) {
            emit error_operation("Операция прервана!\nУстройство извлечено!");
            return 2;
        }

        if (cant_copy_ind == 0) {
            emit cant_copy("Не могу записать: " % dest.fileName() % "\n\n" % dest.errorString());
            func_loop();
        }

        --all_count;
        all_size -= file.size();
        if (all_size < 1)
            all_size = 1;
        if (all_count < 1)
            all_count = 1;

        if (cant_copy_ind == 2)
            return 1;
        return 0;
    }

    QByteArray buf = 0;
    qint64 progres = 0;
    while (!(buf = file.read(1048576)).isEmpty())
    {
        dest.write(buf);
        complited_size += buf.size();
        emit set_comp_size(complited_size);

        if (wasCanceled_first)
            func_loop();

        if (wasCanceled) {
            dest.close();
            QFile::remove(new_name);
            return 1;
        }
    }

    if (!file.exists() || !dest.exists()) {
        emit error_operation("Операция прервана!\nУстройство извлечено!");
        return 2;
    }
    ++complited_count;
    emit set_comp_count(complited_count);
    file.close();
    dest.close();
    DWORD dw_attr = GetFileAttributesA(past_name.toLocal8Bit().data());
    SetFileAttributesA(new_name.toLocal8Bit().data(), dw_attr);

    long long int av_s = st_inf.bytesAvailable();
    if (av_s < 1048576) {
        emit error_operation("Недостаточно свободного места!");
        return 1;
    }

    return 0;
}
