#include "UpdateDialog.h"
#include "ui_UpdateDialog.h"

UpdateDialog::UpdateDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::UpdateDialog)
{
    m_ui->setupUi(this);

    m_returnResult = 0;

    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(okWasPressed()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelWasPressed()));

}

UpdateDialog::~UpdateDialog()
{
    delete m_ui;
}

int UpdateDialog::returnResult()
{
    return m_returnResult;
}

void UpdateDialog::okWasPressed()
{
    m_returnResult = m_ui->decisionBox->currentIndex();
    m_ui->decisionBox->setCurrentIndex(0); //reset to first choice in drop-down box
}

void UpdateDialog::cancelWasPressed()
{
    m_returnResult = 2; //this is the same thing as the last choice in the combo box (remain on current item)
    m_ui->decisionBox->setCurrentIndex(0); //reset to first choice in drop-down box
}

void UpdateDialog::closeEvent(QCloseEvent *e)
{
    m_returnResult = 2;  //closeing the dialog is the same as canceling it
    m_ui->decisionBox->setCurrentIndex(0); //reset to first choice in drop-down box
}
