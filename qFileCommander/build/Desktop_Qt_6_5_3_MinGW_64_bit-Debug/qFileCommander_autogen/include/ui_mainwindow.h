/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_l;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_r;
    QLineEdit *path_l;
    QLineEdit *path_r;
    QLabel *inf_dir_l;
    QLabel *inf_dir_r;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_f3;
    QPushButton *pushButton_f4;
    QPushButton *pushButton_f5;
    QPushButton *pushButton_f6;
    QPushButton *pushButton_f7;
    QPushButton *pushButton_f8;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *horizontalLayout_free_size_l;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *horizontalLayout_free_size_r;
    QToolButton *toolButton;
    QPushButton *pushButton_open_in_exp;
    QPushButton *pushButton_find;
    QPushButton *pushButton_notepad;
    QPushButton *pushButton_create_file;
    QPushButton *pushButton_admin;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1418, 798);
        QFont font;
        font.setStyleStrategy(QFont::PreferDefault);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(10, 0, 281, 31));
        horizontalLayout_l = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_l->setObjectName("horizontalLayout_l");
        horizontalLayout_l->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(720, 0, 281, 21));
        horizontalLayout_r = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_r->setObjectName("horizontalLayout_r");
        horizontalLayout_r->setContentsMargins(0, 0, 0, 0);
        path_l = new QLineEdit(centralwidget);
        path_l->setObjectName("path_l");
        path_l->setGeometry(QRect(5, 50, 600, 20));
        path_l->setStyleSheet(QString::fromUtf8(""));
        path_r = new QLineEdit(centralwidget);
        path_r->setObjectName("path_r");
        path_r->setGeometry(QRect(715, 50, 600, 20));
        inf_dir_l = new QLabel(centralwidget);
        inf_dir_l->setObjectName("inf_dir_l");
        inf_dir_l->setGeometry(QRect(5, 680, 600, 30));
        inf_dir_r = new QLabel(centralwidget);
        inf_dir_r->setObjectName("inf_dir_r");
        inf_dir_r->setGeometry(QRect(715, 680, 600, 30));
        horizontalLayoutWidget_3 = new QWidget(centralwidget);
        horizontalLayoutWidget_3->setObjectName("horizontalLayoutWidget_3");
        horizontalLayoutWidget_3->setGeometry(QRect(9, 710, 1411, 31));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_f3 = new QPushButton(horizontalLayoutWidget_3);
        pushButton_f3->setObjectName("pushButton_f3");

        horizontalLayout->addWidget(pushButton_f3);

        pushButton_f4 = new QPushButton(horizontalLayoutWidget_3);
        pushButton_f4->setObjectName("pushButton_f4");

        horizontalLayout->addWidget(pushButton_f4);

        pushButton_f5 = new QPushButton(horizontalLayoutWidget_3);
        pushButton_f5->setObjectName("pushButton_f5");

        horizontalLayout->addWidget(pushButton_f5);

        pushButton_f6 = new QPushButton(horizontalLayoutWidget_3);
        pushButton_f6->setObjectName("pushButton_f6");

        horizontalLayout->addWidget(pushButton_f6);

        pushButton_f7 = new QPushButton(horizontalLayoutWidget_3);
        pushButton_f7->setObjectName("pushButton_f7");

        horizontalLayout->addWidget(pushButton_f7);

        pushButton_f8 = new QPushButton(horizontalLayoutWidget_3);
        pushButton_f8->setObjectName("pushButton_f8");

        horizontalLayout->addWidget(pushButton_f8);

        line = new QFrame(centralwidget);
        line->setObjectName("line");
        line->setGeometry(QRect(5, 20, 1410, 3));
        line->setFrameShadow(QFrame::Raised);
        line->setLineWidth(5);
        line->setFrameShape(QFrame::HLine);
        line_2 = new QFrame(centralwidget);
        line_2->setObjectName("line_2");
        line_2->setGeometry(QRect(5, 50, 1410, 3));
        line_2->setFrameShadow(QFrame::Raised);
        line_2->setLineWidth(5);
        line_2->setFrameShape(QFrame::HLine);
        line_3 = new QFrame(centralwidget);
        line_3->setObjectName("line_3");
        line_3->setGeometry(QRect(5, 710, 1410, 3));
        line_3->setFrameShadow(QFrame::Raised);
        line_3->setLineWidth(5);
        line_3->setFrameShape(QFrame::HLine);
        horizontalLayoutWidget_4 = new QWidget(centralwidget);
        horizontalLayoutWidget_4->setObjectName("horizontalLayoutWidget_4");
        horizontalLayoutWidget_4->setGeometry(QRect(10, 30, 691, 21));
        horizontalLayout_free_size_l = new QHBoxLayout(horizontalLayoutWidget_4);
        horizontalLayout_free_size_l->setObjectName("horizontalLayout_free_size_l");
        horizontalLayout_free_size_l->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout_free_size_l->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutWidget_5 = new QWidget(centralwidget);
        horizontalLayoutWidget_5->setObjectName("horizontalLayoutWidget_5");
        horizontalLayoutWidget_5->setGeometry(QRect(720, 30, 691, 21));
        horizontalLayout_free_size_r = new QHBoxLayout(horizontalLayoutWidget_5);
        horizontalLayout_free_size_r->setObjectName("horizontalLayout_free_size_r");
        horizontalLayout_free_size_r->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout_free_size_r->setContentsMargins(0, 0, 0, 0);
        toolButton = new QToolButton(centralwidget);
        toolButton->setObjectName("toolButton");
        toolButton->setGeometry(QRect(1390, 0, 24, 26));
        pushButton_open_in_exp = new QPushButton(centralwidget);
        pushButton_open_in_exp->setObjectName("pushButton_open_in_exp");
        pushButton_open_in_exp->setGeometry(QRect(1310, 0, 31, 21));
        pushButton_find = new QPushButton(centralwidget);
        pushButton_find->setObjectName("pushButton_find");
        pushButton_find->setGeometry(QRect(1270, 0, 31, 21));
        pushButton_notepad = new QPushButton(centralwidget);
        pushButton_notepad->setObjectName("pushButton_notepad");
        pushButton_notepad->setGeometry(QRect(1220, 0, 31, 21));
        pushButton_create_file = new QPushButton(centralwidget);
        pushButton_create_file->setObjectName("pushButton_create_file");
        pushButton_create_file->setGeometry(QRect(1170, 0, 31, 21));
        pushButton_admin = new QPushButton(centralwidget);
        pushButton_admin->setObjectName("pushButton_admin");
        pushButton_admin->setGeometry(QRect(1350, 0, 31, 21));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        inf_dir_l->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        inf_dir_r->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButton_f3->setText(QCoreApplication::translate("MainWindow", "F3 \320\237\321\200\320\276\321\201\320\274\320\276\321\202\321\200", nullptr));
        pushButton_f4->setText(QCoreApplication::translate("MainWindow", "F4 \320\237\320\265\321\200\320\265\320\270\320\274\320\265\320\275\320\276\320\262\320\260\321\202\321\214", nullptr));
        pushButton_f5->setText(QCoreApplication::translate("MainWindow", "F5 \320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", nullptr));
        pushButton_f6->setText(QCoreApplication::translate("MainWindow", "F6 \320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214", nullptr));
        pushButton_f7->setText(QCoreApplication::translate("MainWindow", "F7 \320\232\320\260\321\202\320\260\320\273\320\276\320\263", nullptr));
        pushButton_f8->setText(QCoreApplication::translate("MainWindow", "F8 \320\243\320\264\320\260\320\273\320\270\321\202\321\214", nullptr));
        toolButton->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_open_in_exp->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700;\">\320\237\321\200\320\276\320\262\320\276\320\264\320\275\320\270\320\272</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_open_in_exp->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_find->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700;\">\320\237\320\276\320\270\321\201\320\272 \321\204\320\260\320\271\320\273\320\276\320\262...</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_find->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_notepad->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700;\">\320\221\320\273\320\276\320\272\320\275\320\276\321\202</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_notepad->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_create_file->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700;\">\320\241\320\276\320\267\320\264\320\260\321\202\321\214 \321\204\320\260\320\271\320\273</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_create_file->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_admin->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700;\">\320\237\320\265\321\200\320\265\320\267\320\260\320\277\321\203\321\201\321\202\320\270\321\202\321\214 \320\276\321\202 \320\270\320\274\320\265\320\275\320\270 \320\260\320\264\320\274\320\270\320\275\320\270\321\201\321\202\321\200\320\260\321\202\320\276\321\200\320\260</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_admin->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
