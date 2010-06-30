#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QtGui/QDialog>


namespace Ui {
    class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

    public:
        UpdateDialog(QWidget *parent = 0);
        ~UpdateDialog();

        int returnResult();

    private:
        Ui::UpdateDialog *m_ui;

        int m_returnResult;

        void closeEvent(QCloseEvent *e);

    private slots:
        void okWasPressed();
        void cancelWasPressed();
};

#endif // UPDATEDIALOG_H
