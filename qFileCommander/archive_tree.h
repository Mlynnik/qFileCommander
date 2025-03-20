#ifndef ARCHIVE_TREE_H
#define ARCHIVE_TREE_H

#include "appsettings.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QFileIconProvider>
#include <algorithm>

class ANode {
public:
    //имя
    QString fname;
    //дата
    QString fdate;
    //размер (исходный)
    qint64 fsize;
    //размер (после сжатия)
    qint64 fcompr;
    //родительская директория
    ANode* parent = nullptr;
    //содержимое папки (если файл - пусто)
    QVector<ANode*> dnodes = QVector<ANode*>();
    QVector<ANode*> fnodes = QVector<ANode*>();

    ANode(QString _fname, QString _fdate, qint64 _fsize, qint64 _fcompr) : fname(_fname), fdate(_fdate), fsize(_fsize), fcompr(_fcompr) {}

    inline void addDir(ANode* node) {
        node->parent = this;
        dnodes.push_back(node);
    }

    inline void addFile(ANode* node) {
        node->parent = this;
        fnodes.push_back(node);
    }

    ANode* getDir(QString fname) {
        int mx = dnodes.size();
        for (int i = 0; i < mx; ++i) {
            if (dnodes[i]->fname == fname)
                return dnodes[i];
        }
        return nullptr;
    }

    inline static bool PComp(const ANode * const & a, const ANode * const & b)
    {
        return (QString::localeAwareCompare(a->fname, b->fname) < 0);
    }

    void sort_name() {
        std::sort(dnodes.begin(), dnodes.end(), PComp);
        std::sort(fnodes.begin(), fnodes.end(), PComp);

        auto it = dnodes.begin();
        auto end = dnodes.end();
        for (; it != end; ++it)
            (*it)->sort_name();
    }

    ~ANode() {
        auto it = dnodes.begin();
        auto end = dnodes.end();
        for (; it != end; ++it)
            delete *it;

        it = fnodes.begin();
        end = fnodes.end();
        for (; it != end; ++it)
            delete *it;
    }
};


class Archive_tree : public QWidget
{
Q_OBJECT
    QString fname;
    ANode* root;
    QTreeWidget* tree;
    ANode* cur_node;
    QVBoxLayout *horizontalLayout = nullptr;
    QLineEdit *line_path;
    QPushButton *btn_add;
    QPushButton *btn_remove;
    QPushButton *btn_decompr;
    QPushButton *btn_info;
    QString cur_path;
    QIcon dir_ic = QIcon(style()->standardIcon(QStyle::SP_DirIcon));
    QFileIconProvider IC_PR;
    QString str_info;
    const AppSettings *appSettings;

public:
    Archive_tree(QString _fname, const AppSettings *_appSettings);

    void download_arc();

    void set_Active_Dir(ANode* n);

    ~Archive_tree() {
        delete root;
    }

    static void cp866_cp1251( std::string &s ) {
        for( char *c=(char*)s.c_str(); *c!=0; ++c ) {
            if(*c < -80)
                *c += 64;
            else if(*c > -33 && *c < -16)
                *c += 16;
            else if (*c == -1)
                *c = -96;
        }
    }

public slots:
    void remove_file();
    void add_file();
    void decompress();
    void info_show();

private slots:
    void treeWidget_itemActivated(QTreeWidgetItem *item, int column);
    void keyPressEvent(QKeyEvent *event);
};

#endif // ARCHIVE_TREE_H
