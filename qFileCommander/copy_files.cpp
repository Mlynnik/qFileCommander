#include "copy_files.h"
#include "copy_process.h"
#include "helperfunctions.h"
#include <QCoreApplication>
#include <QThread>
#include <QPushButton>


Copy_files::Copy_files(const AppSettings *appSettings)
{
    dialog_font = appSettings->dialog_font;
    msgBox.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    msgBox.setFont(*dialog_font);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Внимание !");
    msgBox.addButton("Переименовать", QMessageBox::YesRole);
    msgBox.addButton("Переименовать все", QMessageBox::YesRole);
    msgBox.addButton("Пропустить", QMessageBox::YesRole);
    msgBox.addButton("Пропустить все", QMessageBox::YesRole);
    msgBox.addButton("Заменить", QMessageBox::NoRole);
    msgBox.addButton("Заменить все", QMessageBox::NoRole);
    msgBox.addButton("Отмена", QMessageBox::NoRole);
}


void Copy_files::update_value_progress()
{
    if (b_minimize->isVisible()) {
        if (all_size == 0) {
            lab_size->setText("0/0 Б");
            lab_files->setText("Файлов: " % reformat_size(comp_count) % " /" % reformat_size(all_count));
            pb->setValue(((comp_count*100)/all_count));
        } else {
            pb->setValue(((comp_size*100)/all_size));
            lab_size->setText(reformat_size_2(comp_size) % " /" % reformat_size_2(all_size));
            lab_files->setText("Файлов: " % reformat_size(comp_count) % " /" % reformat_size(all_count));
        }
    } else if (all_size == 0) {
        pb->setValue(((comp_count*100)/all_count));
    } else {
        pb->setValue(((comp_size*100)/all_size));
    }

}

void Copy_files::Work(QString dir_to, const QStringList& selected_dirs, const QStringList& selected_files, bool remove_after)
{
    if (selected_dirs.length() + selected_files.length() > 0) {
        if (!dir_to.endsWith("/"))
            dir_to.append("/");

        //TODO
        /*if (dir_fold == "1") {
        QString dir_to = QFileDialog::getExistingDirectory(0, "Выбирете папку назначения");
        if (dir_to == "")
            return;
        lp_old.first() = dir_to + "/";
        }*/


        QMessageBox q_copy;
        q_copy.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
        q_copy.setFont(*dialog_font);
        q_copy.setIcon(QMessageBox::Question);

        //TODO lineenit

        if (remove_after) {
            q_copy.setWindowTitle("Перемещение");
            q_copy.setText("Переместить файлы(" % QString::number(selected_dirs.length() + selected_files.length()) %  "шт.) в\n'" % dir_to % "'");
        }
        else {
            q_copy.setWindowTitle("Копирование");
            q_copy.setText("Копировать файлы("  % QString::number(selected_dirs.length() + selected_files.length()) %  "шт.) в\n'" % dir_to % "'");
        }
        q_copy.addButton("Да", QMessageBox::YesRole);
        q_copy.addButton("Отмена", QMessageBox::NoRole);
        if (q_copy.exec() == 1) {
            emit end_operation();
            this->deleteLater();
            return;
        }

        w_progress = new QDialog();
        w_progress->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
        if (remove_after)
            w_progress->setWindowTitle("Перемещение");
        else
            w_progress->setWindowTitle("Копирование");
        w_progress->setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
        w_progress->setFixedHeight(170);
        w_progress->setFixedWidth(500);
        pb = new QProgressBar(w_progress);
        pb->setGeometry(10, 40, 480, 30);
        lab_name = new QLabel(w_progress);
        lab_name->setAlignment(Qt::AlignRight);
        lab_name->setGeometry(10, 10, 400, 20);

        lab_files = new QLabel(w_progress);
        lab_files->setGeometry(10, 90, 200, 30);

        lab_size = new QLabel(w_progress);
        lab_size->setGeometry(300, 90, 180, 30);
        lab_size->setAlignment(Qt::AlignRight);

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
        cd = new CopyProcess(dir_to, selected_dirs, selected_files, remove_after);

        connect(w_progress, SIGNAL(finished(int)), this, SLOT(hard_cancel_clicked()));

        connect(cd, &CopyProcess::set_all_size, this, [this](long long int sz) {all_size = sz;});
        connect(cd, &CopyProcess::set_all_count, this, [this](long long int cnt) {all_count = cnt; update_value_progress();});
        connect(cd, &CopyProcess::set_comp_size, this, [this](long long int sz) {comp_size = sz; update_value_progress();});
        connect(cd, &CopyProcess::set_comp_count, this, [this](long long int cnt) {comp_count = cnt;});

        //connect(cd, SIGNAL(update_value_copy(int)), this, SLOT(update_value_progress(int)));
        connect(cd, SIGNAL(update_name_copy(QString)), this, SLOT(update_name_progress(QString)));
        connect(this, SIGNAL(cancel_copy_clicked()), cd, SLOT(copy_Cancel_clicked()), Qt::DirectConnection);
        connect(cd, SIGNAL(yet_exists(QString)), this, SLOT(set_yet_exists_ind(QString)));
        connect(this, SIGNAL(change_yet_exists_ind(int)), cd, SLOT(change_yet_exists_ind(int)), Qt::DirectConnection);

        connect(cd, SIGNAL(error_operation(QString)), this, SLOT(v_error(QString)));

        connect(this, SIGNAL(cancel_clicked_first()), cd, SLOT(cancel_clicked_first()), Qt::DirectConnection);
        connect(this, SIGNAL(cancel_unclicked_first()), cd, SLOT(cancel_unclicked_first()), Qt::DirectConnection);

        connect(cd, SIGNAL(cant_copy(QString)), this, SLOT(cant_copy(QString)));
        connect(this, SIGNAL(change_cant_copy_ind(int)), cd, SLOT(change_cant_copy_ind(int)), Qt::DirectConnection);
        connect(cd, SIGNAL(cant_del(QString)), this, SLOT(cant_del(QString)));
        connect(this, SIGNAL(change_cant_del_ind(int)), cd, SLOT(change_cant_del_ind(int)), Qt::DirectConnection);

        connect(th, SIGNAL(started()), cd, SLOT(Copy()));
        connect(th, SIGNAL(finished()), this, SLOT(end_copy()));

        cd->moveToThread(th);
        th->start();
    }

}

void Copy_files::update_name_progress(QString val)
{
    lab_name->setText(val);
}

void Copy_files::cancel_clicked()
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
        emit cancel_copy_clicked();
    } else {
        emit cancel_unclicked_first();
    }
}

void Copy_files::set_yet_exists_ind(QString val)
{
    msgBox.setText(val);
    int ind = msgBox.exec();
    if (ind == 6) {
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
            emit change_yet_exists_ind(ind);
            w_progress->disconnect();
            emit cancel_copy_clicked();
            w_progress->close();
            //this->destroyed();
            //this->close();
        } else {
            set_yet_exists_ind(val);
        }
    } else {
        emit change_yet_exists_ind(ind);
    }
}

void Copy_files::v_error(QString str_error) {
    QMessageBox v_err;
    v_err.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_err.setFont(*dialog_font);
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
}


void Copy_files::cant_copy(QString str_error) {
    QMessageBox v_err;
    v_err.setWindowIcon(QIcon(":/resources/icons/appIcon.png"));
    v_err.setFont(*dialog_font);
    v_err.setIcon(QMessageBox::Warning);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.addButton("Пропустить", QMessageBox::YesRole);
    v_err.addButton("Пропустить все", QMessageBox::YesRole);
    v_err.addButton("Отмена", QMessageBox::NoRole);
    emit change_cant_copy_ind(v_err.exec());
}

void Copy_files::cant_del(QString str_error)
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

void Copy_files::hard_cancel_clicked()
{
    emit cancel_clicked_first();
    v_error("Прервано пользователем!");
    emit cancel_copy_clicked();
}

void Copy_files::pause_operation()
{
    if (b_pause->text() == "Пауза") {
        b_pause->setText("Старт");
        emit cancel_clicked_first();
    } else {
        b_pause->setText("Пауза");
        emit cancel_unclicked_first();
    }
}

void Copy_files::minimize_clicked()
{
    b_minimize->hide();
    b_cancel->hide();
    lab_files->hide();
    lab_size->hide();
    w_progress->setFixedHeight(60);
    w_progress->setFixedWidth(300);
    lab_name->setGeometry(5, 5, 200, 25);
    pb->setGeometry(10, 30, 200, 30);
    b_pause->setGeometry(210, 30, 90, 30);
    w_progress->showMinimized();
}

void Copy_files::end_copy()
{
    emit end_operation();

    w_progress->disconnect();
    pb->deleteLater();
    lab_name->deleteLater();
    lab_size->deleteLater();
    lab_files->deleteLater();
    b_pause->deleteLater();
    b_minimize->deleteLater();
    b_cancel->deleteLater();
    w_progress->close();
    w_progress->deleteLater();

    delete th;
    this->deleteLater();
}

