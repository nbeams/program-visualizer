/********************************************************************************
** Form generated from reading ui file 'VisualizerRect.ui'
**
** Created: Sun Feb 21 06:17:58 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_VISUALIZERRECT_H
#define UI_VISUALIZERRECT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VisualizerRect
{
public:

    void setupUi(QWidget *VisualizerRect)
    {
        if (VisualizerRect->objectName().isEmpty())
            VisualizerRect->setObjectName(QString::fromUtf8("VisualizerRect"));
        VisualizerRect->resize(400, 300);

        retranslateUi(VisualizerRect);

        QMetaObject::connectSlotsByName(VisualizerRect);
    } // setupUi

    void retranslateUi(QWidget *VisualizerRect)
    {
        VisualizerRect->setWindowTitle(QApplication::translate("VisualizerRect", "Form", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(VisualizerRect);
    } // retranslateUi

};

namespace Ui {
    class VisualizerRect: public Ui_VisualizerRect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VISUALIZERRECT_H
