#include "copy_files.h"
#include "copy_process.h"
#include <QCoreApplication>
#include <QThread>
#include <QPushButton>


Copy_files::Copy_files()
{
    msgBox.setFont(main_font);
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

void Copy_files::update_value_progress(int val)
{
    qpd->setValue(val);
}

void Copy_files::Work(QString dir_to, QStringList selected_dirs, QStringList selected_files, bool remove_after)
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
        q_copy.setFont(main_font);
        q_copy.setIcon(QMessageBox::Question);

        //TODO lineenit

        if (remove_after) {
            q_copy.setWindowTitle("Перемещение");
            q_copy.setText("Переместить файлы(" % QString::number(selected_dirs.length() + selected_files.length()) %  "шт.) в\n'" % dir_to);
        }
        else {
            q_copy.setWindowTitle("Копирование");
            q_copy.setText("Копировать файлы("  % QString::number(selected_dirs.length() + selected_files.length()) %  "шт.) в\n'" % dir_to);
        }
        q_copy.addButton("Да", QMessageBox::YesRole);
        q_copy.addButton("Отмена", QMessageBox::NoRole);
        if (q_copy.exec() == 1) {
            emit end_operation();
            this->deleteLater();
            return;
        }
        qpd = new QProgressDialog();
        qpd->setAutoClose(true);
        qpd->setMinimum(0);
        qpd->setMaximum(100);
        qpd->setMinimumWidth(400);

        th = new QThread(this);
        cd = new CopyProcess(dir_to, selected_dirs, selected_files, remove_after);

        connect(qpd, SIGNAL(canceled()), this, SLOT(cancel_clicked()));

        connect(cd, SIGNAL(update_value_copy(int)), this, SLOT(update_value_progress(int)));
        connect(cd, SIGNAL(update_name_copy(QString)), this, SLOT(update_name_progress(QString)));
        connect(this, SIGNAL(cancel_copy_clicked()), cd, SLOT(copy_Cancel_clicked()), Qt::DirectConnection);
        connect(cd, SIGNAL(yet_exists(QString)), this, SLOT(set_yet_exists_ind(QString)));
        connect(this, SIGNAL(change_yet_exists_ind(int)), cd, SLOT(change_yet_exists_ind(int)), Qt::DirectConnection);

        connect(cd, SIGNAL(error_operation(QString)), this, SLOT(v_error(QString)));

        connect(this, SIGNAL(cancel_clicked_first()), cd, SLOT(cancel_clicked_first()), Qt::DirectConnection);
        connect(this, SIGNAL(cancel_unclicked_first()), cd, SLOT(cancel_unclicked_first()), Qt::DirectConnection);

        connect(th, SIGNAL(started()), cd, SLOT(Copy()));
        connect(th, SIGNAL(finished()), this, SLOT(end_copy()));

        cd->moveToThread(th);
        th->start();
    }

}

void Copy_files::update_name_progress(QString val)
{
    qpd->setLabelText(val);
}

void Copy_files::cancel_clicked()
{
    emit cancel_clicked_first();
    QMessageBox v_q;
    v_q.setFont(main_font);
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
        v_q.setFont(main_font);
        v_q.setIcon(QMessageBox::Question);
        v_q.setWindowTitle("Ошибка!");
        v_q.setText("Прервать операцию?");
        QAbstractButton *ok = v_q.addButton(tr("ОК"), QMessageBox::ActionRole);
        v_q.addButton(tr("Отмена"), QMessageBox::ActionRole);
        v_q.exec();
        if (v_q.clickedButton() == ok) {
            emit change_yet_exists_ind(ind);
            qpd->disconnect();
            emit cancel_copy_clicked();
            qpd->close();
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
    v_err.setFont(main_font);
    v_err.setIcon(QMessageBox::Critical);
    v_err.setWindowTitle("Ошибка !");
    v_err.setText(str_error);
    v_err.setTextInteractionFlags(Qt::TextSelectableByMouse);
    v_err.exec();
}

void Copy_files::end_copy()
{
    emit end_operation();
    qpd->disconnect();
    qpd->close();
    qpd->deleteLater();
    delete th;
    this->deleteLater();
}

