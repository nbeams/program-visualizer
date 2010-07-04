/********************************************************************************
** Form generated from reading ui file 'UpdateDialog.ui'
**
** Created: Sat Jul 3 21:45:48 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_UPDATEDIALOG_H
#define UI_UPDATEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_UpdateDialog
{
public:
    QDialogButtonBox *buttonBox;
    QComboBox *decisionBox;
    QLabel *message;
    QLabel *label;

    void setupUi(QDialog *UpdateDialog)
    {
        if (UpdateDialog->objectName().isEmpty())
            UpdateDialog->setObjectName(QString::fromUtf8("UpdateDialog"));
        UpdateDialog->resize(606, 133);
        buttonBox = new QDialogButtonBox(UpdateDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(340, 90, 171, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        decisionBox = new QComboBox(UpdateDialog);
        decisionBox->setObjectName(QString::fromUtf8("decisionBox"));
        decisionBox->setGeometry(QRect(60, 100, 231, 21));
        message = new QLabel(UpdateDialog);
        message->setObjectName(QString::fromUtf8("message"));
        message->setGeometry(QRect(50, 10, 531, 41));
        label = new QLabel(UpdateDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(50, 80, 261, 16));

        retranslateUi(UpdateDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UpdateDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UpdateDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UpdateDialog);
    } // setupUi

    void retranslateUi(QDialog *UpdateDialog)
    {
        UpdateDialog->setWindowTitle(QApplication::translate("UpdateDialog", "Confirm Selection Change", 0, QApplication::UnicodeUTF8));
        decisionBox->clear();
        decisionBox->insertItems(0, QStringList()
         << QApplication::translate("UpdateDialog", "Leave item without updating source file", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("UpdateDialog", "Do not leave current item", 0, QApplication::UnicodeUTF8)
        );
        message->setText(QApplication::translate("UpdateDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Are you sure you want to leave the current item?</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Changes made to this item's text will not be saved unless you update your source file first.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("UpdateDialog", "What do you want the Program Visualizer to do?", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(UpdateDialog);
    } // retranslateUi

};

namespace Ui {
    class UpdateDialog: public Ui_UpdateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATEDIALOG_H
