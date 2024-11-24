#ifndef Rename_Widget_H
#define Rename_Widget_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>
#include <QProgressDialog>


class Rename_TreeWidget : public QTreeWidget {
    Q_OBJECT
public:
    explicit Rename_TreeWidget(QWidget *parent = nullptr);
signals:
    void update_cnt();
public slots:
    void dropEvent(QDropEvent* event) override;
};


class Rename_Mass : public QObject {
    Q_OBJECT
public:
    Rename_Mass(const QString &_dir, const QStringList &_old_names, const QStringList &_new_names, const QList<bool> &_flags_dir);

signals:
    ///фатальная ошибка
    void error_operation(QString str_error);
    ///устанавливает compiled кол-во файлов
    void set_comp_count(int);

private:
    QString dir;
    QStringList old_names;
    QStringList new_names;
    QList<bool> flags_dir;
    bool is_stop = false;

public slots:
    void Work();
    void stop();
};


class Rename_Widget : public QMainWindow
{
    Q_OBJECT

public:
    Rename_Widget(const QFont& _main_font, float _w = 1.0, float _h = 1.0, QWidget *parent = nullptr);
    void Fill(const QString& _dir, QStringList _selected_dirs, QStringList _selected_files);
    QFont main_font;
    ~Rename_Widget();

signals:
    void end_operation();

private:
    float w = 1.0;
    float h = 1.0;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit_name;
    QPushButton *pushButton_name;
    QLabel *label_name;
    QLineEdit *lineEdit_type;
    QLabel *label_type;
    QPushButton *pushButton_type;
    QLabel *label_cnt;
    QPushButton *pushButton_name_cnt;
    QLabel *label_begin;
    QLineEdit *lineEdit_find;
    QPushButton *pushButton_type_cnt;
    QLabel *label_find_replace;
    QLabel *label_step;
    QLabel *label_replace;
    QLabel *label_find;
    QLineEdit *lineEdit_replace;
    QLineEdit *lineEdit_begin;
    QLineEdit *lineEdit_step;
    QLabel *label_path;
    Rename_TreeWidget *treeWidget;
    QPushButton *pushButton_work;
    QProgressDialog *qpd;

    QList<char> ban_symb { '<', '>', ':', '"', '/', '\\', '|', '?', '*'};

    QString dir;
    QString find_now = "";
    QString replace_now = "";

private slots:
    void on_pushButton_name_clicked();
    void on_pushButton_name_cnt_clicked();

    void on_pushButton_type_clicked();
    void on_pushButton_type_cnt_clicked();

    void on_pushButton_work_clicked();

    void lineEdit_name_text_changed();
    void lineEdit_type_text_changed();

    void lineEdit_begin_changed();
    void lineEdit_step_changed();

    void lineEdit_find_text_changed();
    void lineEdit_replace_text_changed();

    void end_operation_rename();

public slots:
    void v_error(QString str_error);

};
#endif // Rename_Widget_H
