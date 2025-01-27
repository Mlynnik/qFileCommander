#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QWidget>
#include <QMimeDatabase>
#include <QTreeWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QFileIconProvider>

class Find_Process : public QObject
{
    Q_OBJECT
public:
    Find_Process(QString file_name, QString path, QString f_cont);

signals:
    void add_item(QString path, QFileInfo fi);
    void end_of_search(int, int);
    ///выйти из петли (снятие с паузы)
    void signal_loop();

public slots:
    void Work();
    void stop_find();

private:
    QString file_name;
    QString path;
    QString f_cont;

    QMimeDatabase db;
    bool stop = false;
    QString last_f_n;
    QString last_d_n;
    QString last_f_cont;
};

class FindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindWidget(float _w, float _h, const QFont *_main_font, const QFont *_panel_font, const QFont *_dialog_font, QWidget *parent = nullptr);

signals:
    void stop_find();

    void open_find_fid_signal(QString f_name);

private:
    float w;
    float h;
    const QFont *main_font;
    const QFont *panel_font;
    const QFont *dialog_font;
    QMenu * menu;
    QAction * menu_open;
    QAction * menu_open_in_dir;
    QAction * menu_open_in_dir_exp;

    QMimeDatabase db;
    QFileIconProvider ic_pr;
    QTreeWidget *tree_find = new QTreeWidget();
    QLabel *lab_find = new QLabel();
    QComboBox *comb_file = new QComboBox();
    QPushButton *find_file = new QPushButton();
    QLabel *find_cont = new QLabel();
    QComboBox *comb_cont = new QComboBox();
    QPushButton *cancel_comb = new QPushButton();
    QLabel *lab_dir_find = new QLabel();
    QComboBox *comb_dir = new QComboBox();
    QPushButton *select_dir_b = new QPushButton();
    QLabel *find_res_0 = new QLabel();
    QLabel *find_res_1 = new QLabel();
    QGridLayout *gr_lay = new QGridLayout(this);
    bool stop = false;
    QString last_f_n;
    QString last_d_n;
    QString last_f_cont;
    bool is_close = false;

public slots:
    void set_dir(QString dir);
    void add_item(QString path, QFileInfo fi);
    void end_of_search(int n_f, int n_d);
    void cancel_clicked();
    void close_wid();

private slots:
    void find_f_0();
    void dir_select_comb();
    void open_find_f(QTreeWidgetItem *item, int);
    void open_find_fid(QString f_name);
    void open_find_fid_exp(QString f_name);
    void tree_find_customContextMenuRequested(const QPoint &pos);

    void closeEvent(QCloseEvent *event);

    void v_error(QString str_error);
};

#endif // FINDWIDGET_H
