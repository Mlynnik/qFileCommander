#include "delete_files.h"
#include "helperfunctions.h"
#include <QDir>
#include <QEventLoop>
#include <QThread>
#include <QPushButton>
#include <shlobj.h>

Delete_Files::Delete_Files(const AppSettings *appSettings) : dialog_font(appSettings->dialog_font) {}

void Delete_Files::Work(const QStringList &selected_dirs, const QStringList &selected_files, bool is_final)
{
    if (selected_dirs.length() + selected_files.length() > 0) {
        all_count = selected_dirs.length() + selected_files.length();
        QMessageBox q_del;
        q_del.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
        q_del.setFont(*dialog_font);
        q_del.setIcon(QMessageBox::Question);
        q_del.setWindowTitle("Удаление");
        if (is_final)
            q_del.setText("Безвозвратно удалить файлы/каталоги ("  % QString::number(selected_dirs.length() + selected_files.length()) %  "шт.)?");
        else
            q_del.setText("Удалить файлы/каталоги ("  % QString::number(selected_dirs.length() + selected_files.length()) %  "шт.)?");
        q_del.addButton("Да", QMessageBox::YesRole);
        q_del.addButton("Отмена", QMessageBox::NoRole);
        if (q_del.exec() == 1) {
            emit end_operation();
            this->deleteLater();
            return;
        }

        w_progress = new QDialog();
        w_progress->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
        w_progress->setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
        w_progress->setWindowTitle("Удаление");
        w_progress->setFixedHeight(170);
        w_progress->setFixedWidth(500);
        pb = new QProgressBar(w_progress);
        pb->setGeometry(10, 40, 480, 30);
        lab_name = new QLabel(w_progress);
        lab_name->setAlignment(Qt::AlignRight);
        lab_name->setGeometry(10, 10, 400, 20);

        lab_files = new QLabel(w_progress);
        lab_files->setGeometry(10, 90, 200, 30);


        b_pause = new QPushButton(w_progress);
        b_pause->setText("Пауза");
        b_pause->setGeometry(20, 130, 110, 30);
        connect(b_pause, SIGNAL(clicked()), this, SLOT(pause_operation()));
        b_minimize = new QPushButton(w_progress);
        b_minimize->setText("В фоне");
        b_minimize->setGeometry(190, 130, 110, 30);
        connect(b_minimize, SIGNAL(clicked()), this, SLOT(minimize_clicked()));
        b_cancel = new QPushButton(w_progress);
        b_cancel->setText("Отмена");
        b_cancel->setGeometry(360, 130, 110, 30);
        connect(b_cancel, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
        w_progress->setFont(*dialog_font);
        w_progress->show();

        th = new QThread(this);
        dp = new Delete_Process(selected_dirs, selected_files, is_final);

        connect(w_progress, SIGNAL(finished(int)), this, SLOT(hard_cancel_clicked()));

        connect(dp, &Delete_Process::set_comp_count, this, [this](long long int cnt) {comp_count = cnt; update_value_progress();});

        connect(dp, SIGNAL(update_name_del(QString)), this, SLOT(update_name_progress(QString)));
        connect(this, SIGNAL(cancel_del_clicked()), dp, SLOT(del_Cancel_clicked()), Qt::DirectConnection);

        connect(dp, SIGNAL(error_operation(QString)), this, SLOT(v_error(QString)));

        connect(this, SIGNAL(cancel_clicked_first()), dp, SLOT(cancel_clicked_first()), Qt::DirectConnection);
        connect(this, SIGNAL(cancel_unclicked_first()), dp, SLOT(cancel_unclicked_first()), Qt::DirectConnection);

        connect(dp, SIGNAL(cant_del(QString)), this, SLOT(cant_del(QString)));
        connect(this, SIGNAL(change_cant_del_ind(int)), dp, SLOT(change_cant_del_ind(int)), Qt::DirectConnection);

        connect(th, SIGNAL(started()), dp, SLOT(Work()));
        connect(th, SIGNAL(finished()), this, SLOT(end_del()));

        dp->moveToThread(th);
        th->start();
    }
}

void Delete_Files::update_value_progress()
{
    if (b_minimize->isVisible()) {
        lab_files->setText("Файлов: " % HelperFunctions::reformat_size(comp_count) % " /" % HelperFunctions::reformat_size(all_count));
    }
    pb->setValue(((comp_count*100)/all_count));
}

void Delete_Files::update_name_progress(QString val)
{
    lab_name->setText(val);
}

void Delete_Files::v_error(QString str_error)
{
    QMessageBox v_err;
    v_err.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_err.setFont(*dialog_font);
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
}

void Delete_Files::cant_del(QString str_error)
{
    QMessageBox v_err;
    v_err.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_err.setFont(*dialog_font);
    v_err.setIcon(QMessageBox::Warning);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText("Не удалось удалить: " + str_error);
    v_err.addButton("Пропустить", QMessageBox::YesRole);
    v_err.addButton("Пропустить все", QMessageBox::YesRole);
    v_err.addButton("Отмена", QMessageBox::NoRole);
    emit change_cant_del_ind(v_err.exec());
}

void Delete_Files::cancel_clicked()
{
    emit cancel_clicked_first();
    QMessageBox v_q;
    v_q.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_q.setFont(*dialog_font);
    v_q.setIcon(QMessageBox::Question);
    v_q.setWindowTitle("Ошибка!");
    v_q.setText("Прервать операцию?");
    QAbstractButton *ok = v_q.addButton(tr("ОК"), QMessageBox::ActionRole);
    v_q.addButton(tr("Отмена"), QMessageBox::ActionRole);
    v_q.exec();

    if (v_q.clickedButton() == ok) {
        emit cancel_del_clicked();
    } else {
        emit cancel_unclicked_first();
    }

}

void Delete_Files::hard_cancel_clicked()
{
    emit cancel_clicked_first();
    v_error("Прервано пользователем!");
    emit cancel_del_clicked();
}

void Delete_Files::pause_operation()
{
    if (b_pause->text() == "Пауза") {
        b_pause->setText("Старт");
        emit cancel_clicked_first();
    } else {
        b_pause->setText("Пауза");
        emit cancel_unclicked_first();
    }
}

void Delete_Files::minimize_clicked()
{
    b_minimize->hide();
    b_cancel->hide();
    lab_files->hide();
    w_progress->setFixedHeight(60);
    w_progress->setFixedWidth(300);
    lab_name->setGeometry(5, 5, 200, 25);
    pb->setGeometry(10, 30, 200, 30);
    b_pause->setGeometry(210, 30, 90, 30);
    w_progress->showMinimized();
}

void Delete_Files::end_del()
{
    emit end_operation();

    w_progress->disconnect();
    pb->deleteLater();
    lab_name->deleteLater();
    lab_files->deleteLater();
    b_pause->deleteLater();
    b_minimize->deleteLater();
    b_cancel->deleteLater();
    w_progress->close();
    w_progress->deleteLater();

    delete th;
    this->deleteLater();
}









Delete_Process::Delete_Process(const QStringList &selected_dirs, const QStringList &selected_files, bool is_final)
    : selected_dirs(selected_dirs), selected_files(selected_files), is_final(is_final)
{
    wasCanceled = false;
    if (selected_dirs.size() > 0)
        disk = selected_dirs[0].left(selected_dirs[0].lastIndexOf("/")) % "/";
    else
        disk = selected_files[0].left(selected_files[0].lastIndexOf("/")) % "/";
}

void Delete_Process::Work()
{
    for (int i = 0; i < selected_dirs.size(); ++i) {
        if (is_final || !QFile::moveToTrash(selected_dirs[i])) {
            SetFileAttributesA(selected_dirs[i].toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
            if (!removeDir(selected_dirs[i])) {
                if (wasCanceled_first)
                    func_loop();
                if (wasCanceled || (cant_del_ind == 2))
                    goto lab_end;

                if (!QDir(disk).exists()) {
                    emit error_operation("Операция прервана!\nУстройство извлечено!");
                    goto lab_end;
                }
                if (cant_del_ind == 0) {
                    emit cant_del(selected_dirs[i]);
                    func_loop();
                }
                if (cant_del_ind == 2)
                    goto lab_end;
            }
        }

        if (wasCanceled_first)
            func_loop();

        if (wasCanceled || (cant_del_ind == 2))
            goto lab_end;

        emit update_name_del(selected_dirs[i]);
        emit set_comp_count(++complited_count);
    }

    for (int i = 0; i < selected_files.size(); ++i) {
        if (is_final || !QFile::moveToTrash(selected_files[i])) {
            SetFileAttributesA(selected_files[i].toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
            QFile file(selected_files[i]);
            if (!file.remove()) {
                if (wasCanceled_first)
                    func_loop();
                if (wasCanceled || (cant_del_ind == 2))
                    goto lab_end;

                if (!QDir(disk).exists()) {
                    emit error_operation("Операция прервана!\nУстройство извлечено!");
                    goto lab_end;
                }
                if (cant_del_ind == 0) {
                    emit cant_del(selected_files[i]);
                    func_loop();
                }
                if (cant_del_ind == 2)
                    goto lab_end;
            }
        }

        if (wasCanceled_first)
            func_loop();

        if (wasCanceled || (cant_del_ind == 2))
            goto lab_end;

        emit update_name_del(selected_files[i]);
        emit set_comp_count(++complited_count);
    }

lab_end:
    this->deleteLater();
    this->thread()->exit();
}

bool Delete_Process::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {

            emit update_name_del(info.absoluteFilePath());
            SetFileAttributesA(info.absoluteFilePath().toLocal8Bit().data(), FILE_ATTRIBUTE_NORMAL);
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());

                if (!result) {
                    if (wasCanceled_first)
                        func_loop();
                    if (wasCanceled || (cant_del_ind == 2))
                        return false;

                    if (!QDir(disk).exists()) {
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

                    if (!QDir(disk).exists()) {
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
                //result = QFile::remove(info.absoluteFilePath());
            }

            if (wasCanceled_first)
                func_loop();
            if (wasCanceled || (cant_del_ind == 2))
                return false;
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void Delete_Process::del_Cancel_clicked()
{
    wasCanceled = true;
    wasCanceled_first = false;
    emit signal_loop();
}

void Delete_Process::func_loop()
{
    QEventLoop pause;
    connect(this, SIGNAL(signal_loop()), &pause, SLOT(quit()));
    pause.exec();
}

void Delete_Process::cancel_clicked_first()
{
    wasCanceled_first = true;
}

void Delete_Process::cancel_unclicked_first()
{
    wasCanceled_first = false;
    emit signal_loop();
}

void Delete_Process::change_cant_del_ind(int val)
{
    cant_del_ind = val;
    emit signal_loop();
}


