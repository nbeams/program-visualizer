#include "ExtraGUIToolBar.h"
#include "ui_ExtraGUIToolBar.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>

ExtraGUIToolBar::ExtraGUIToolBar(QWidget *parent) : QWidget(parent), m_ui(new Ui::ExtraGUIToolBar), m_currentSourceFile(0), m_action("Extra GUI", this)
{
    Q_INIT_RESOURCE(extra_gui);
    m_ui->setupUi(this);

    m_funcDialog = new FunctionDialog();
    m_help = new HelpWindow();

    /*Connections*/
    connect(m_ui->conditionalIf, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_ui->ifElse, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_ui->conditionalSwitch, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_ui->loop, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_ui->function, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_ui->help, SIGNAL(clicked()), this, SLOT(helpRequested()));
    connect(this, SIGNAL(newFunction()), this, SLOT(newFunctionFromToolBar()));
    connect(&m_action, SIGNAL(triggered()), this, SLOT(toggleWindow()));

}

ExtraGUIToolBar::~ExtraGUIToolBar()
{
    delete m_ui;
    Q_CLEANUP_RESOURCE(extra_gui);
}

void ExtraGUIToolBar::toggleWindow()
{
    if(isVisible()) close();
    else show();
}

void ExtraGUIToolBar::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int ExtraGUIToolBar::loopIndex()
{
    return m_ui->loopComboBox->currentIndex();
}

int ExtraGUIToolBar::modeIndex()
{
    return 0;
    //return m_ui->modeComboBox->currentIndex();
}

void ExtraGUIToolBar::setInsertText(QString objectName)
{
    int i;
    /*Clear m_insertText*/
    m_insertText.clear();

    QString m_indent;
    /*Set string of space for indentations*/
    for(i = 1; i <= m_currentSourceFile->tabWidth(); i++)
        m_indent.append(" ");

    /*Set text to insert based on which button or label has been selected*/
    if(objectName == "conditionalIf" || objectName == "ifLabel")
    {
        m_insertText.append(m_indent+"if(boolean_expression)\n");
        m_insertText.append(m_indent+"{ \n"+m_indent+m_indent+"//What will happen if this is true? \n"+m_indent+"}\n");
    }
    else if(objectName == "ifElse" || objectName == "ifElseLabel")
    {
        m_insertText.append(m_indent+"if(boolean_expression)\n");
        m_insertText.append(m_indent+"{\n"+m_indent+m_indent+"//What will happen if this is true? \n"+m_indent+"}\n");
        m_insertText.append(m_indent+"else\n");
        m_insertText.append(m_indent+"{\n"+m_indent+m_indent+"//What will happen if this is false?\n"+m_indent+"}\n");
    }
    else if(objectName == "conditionalSwitch" || objectName == "switchLabel")
    {
        m_insertText.append(m_indent+"switch(variable)\n"+m_indent+"{\n");
        m_insertText.append(m_indent+m_indent+"case a_value:\n");
        m_insertText.append(m_indent+m_indent+"{\n"+m_indent+m_indent+m_indent+"//What happens if variable == a_value?\n");
        m_insertText.append(m_indent+m_indent+m_indent+"break;\n"+m_indent+m_indent+"}    \n");
        m_insertText.append(m_indent+m_indent+"//other cases go here\n"+m_indent+"}\n");
    }
    else if(objectName == "loop" || objectName == "loopLabel")
    {
        switch(loopIndex())
        {
            case 0:
            {
                m_insertText.append(m_indent+"for(variable = starting_value; variable <, <=, >=, or > ending_value; change_in_variable)\n");
                m_insertText.append(m_indent+"{\n"+m_indent+m_indent+"//What needs to be looped?\n"+m_indent+"}\n");
                break;
            }
            case 1:
            {
                m_insertText.append(m_indent+"while(boolean_expression)\n");
                m_insertText.append(m_indent+"{\n"+m_indent+m_indent+"//What code will be looped while this is true?\n"+m_indent+"}\n");
                break;
            }
        }
    }
    else if(objectName == "functionLabel")
        m_insertText.append("function");

}

void ExtraGUIToolBar::mousePressEvent(QMouseEvent *event)
{
    /*Ignore if there is no current source file...no use in having drag and drop if there's nothing to drop to!*/
    if(!m_currentSourceFile)
        return;

    else
    {
        /*Ignore if there is no child at this spot*/
        if(childAt(event->pos()) == 0)
            event->ignore();
        /*Ignore if click is on the line or on the mode label*/
        else if(childAt(event->pos()) == m_ui->line)
            event->ignore();
        /*Ignore right clicks*/
        else if(event->button() == Qt::RightButton)
            event->ignore();

        /*Begin drag if child is draggable*/
        else
        {
            m_childAtPosition = qobject_cast<QLabel *>(childAt(event->pos()));
            m_dragStartPosition = event->pos();

            setInsertText(m_childAtPosition->objectName());
            m_dragText = new QMimeData;
            m_dragText->setText(m_insertText);

            m_drag = new QDrag(this);
            m_drag->setPixmap(*(m_childAtPosition->pixmap()));
            m_drag->setMimeData(m_dragText);
            m_drag->start();
        }
    }
}

void ExtraGUIToolBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    /*Ignore double clicks*/
    event->ignore();

}

void ExtraGUIToolBar::buttonClicked()
{
     if(!m_currentSourceFile)
            return;
     else
     {
         /*Find which button has been clicked and emit the correct signal*/
         QObject *senderObject = sender();
         QPushButton *senderButton = qobject_cast<QPushButton *>(senderObject);

         if(senderObject->objectName() == "function")
             emit newFunction();

         else //Regular text insertion button
         {
             setInsertText(senderButton->objectName());
             m_currentSourceFile->insert(m_insertText);
         }
     }
}

void ExtraGUIToolBar::helpRequested()
{
    m_help->show();
}

void ExtraGUIToolBar::toolBarDropEvent(QDropEvent *event)
{
   m_dropData = event->mimeData();
   int pos_y;
   int topLineShown = 0;
   int currentTextHeight = 0;

   if(!m_currentSourceFile)
       return;
   else
   {
       /*Check which line is currently at the top of the screen*/
       topLineShown = m_currentSourceFile->firstVisibleLine();

       /*Get Y position of drop event and convert to a line number.
     (This is to avoid problems when the drop is desired at a place currently without
        a line of text.) */
       pos_y = event->pos().y();
       currentTextHeight = m_currentSourceFile->textHeight(0); //what pixel size is the font currently being used?
       m_insertLine = topLineShown + (pos_y - 15)/(currentTextHeight-1);  //ugly code to convert to line number

       if(m_dropData->text() == "function")
           emit newFunction();

       else
           m_currentSourceFile->insertAt(m_dropData->text(), m_insertLine, 0);
   }
}

void ExtraGUIToolBar::newFunctionFromToolBar()
{
    int line = 0;
    int index = 0;
    int newLine = 0;
    int newIndex = 0;
    int mainLine = 0;
    int functionLine = 0;
    bool textFound = true;

    QString m_indent; int i;
    /*Set string of space for indentations and send it to the dialog for use with m_bodyText if needed*/
    for(i = 1; i <= m_currentSourceFile->tabWidth(); i++)
        m_indent.append(" ");

    m_funcDialog->setIndent(m_indent);

    /*Execute the New Function Dialog*/
    m_funcDialog->exec();

    if(!m_currentSourceFile)
        return;
    else
    {
        if(m_funcDialog->okWasPressed())
        {
            /*Begin code to find insertion point for prototype*/
            /*Find location of "Function prototypes below" text*/
            if(m_currentSourceFile->findFirst("Function prototypes below", true, false, true, true, true, 0, 0, false))
                m_currentSourceFile->getCursorPosition(&line, &index);

            /*Decide where to add this text and the prototype if the text cannot already be found*/
            else
            {
                /*Find location of main function if it exists*/
                if(m_currentSourceFile->findFirst("main", true, false, true, true, true, 0, 0, false))
                    m_currentSourceFile->getCursorPosition(&mainLine, &index);
                else
                    mainLine = m_currentSourceFile->lines();

                /*Find location of first preprocessor command and set cursor to this position*/
                if(m_currentSourceFile->findFirst("#", true, false, true, true, true, 0, 0, false))
                {
                    m_currentSourceFile->getCursorPosition(&line, &index);

                    while(textFound == true)
                    {
                        m_currentSourceFile->findNext();
                        /*Get location of cursor after next preprocessor command is found*/
                        m_currentSourceFile->getCursorPosition(&newLine, &newIndex);

                        /*Ignore anything found beyond the beginning of the main function*/
                        if(newLine >= mainLine)
                            textFound = false;
                        else
                        {
                            /*If next # is not farther down or to the right in the document, ignore it*/
                            if((newLine > line)||(newIndex > index))
                                line = newLine;
                            else
                                textFound = false;
                        }
                    }
                    line = line + 2;
                }
                m_currentSourceFile->insertAt("/*Function prototypes below*/\n\n", line, 0);
            }

            /*Insert function prototype*/
            line++;
            m_currentSourceFile->setCursorPosition(line, 0);
            m_currentSourceFile->insert(";\n");
            m_currentSourceFile->insert(*(m_funcDialog->prototype()));

             /*Insert function definition skeleton*/
            /*Find bottom of file to insert new function*/
           functionLine = line; //start from wherever the prototype was placed so it won't accidentally precede its prototype
           while(!(m_currentSourceFile->text(functionLine).isEmpty()))
            {
                functionLine++;
            }
           /*Insert new function*/
            m_currentSourceFile->insertAt("\n"+*(m_funcDialog->prototype())+"\n"+m_funcDialog->bodyText(), functionLine, 0);

        }
    }
}


void ExtraGUIToolBar::setCurrentFile(SourceFile *sourceFile)
{
    if(m_currentSourceFile)
        disconnect(m_currentSourceFile, SIGNAL(handleDrop(QDropEvent*)), this, SLOT(toolBarDropEvent(QDropEvent*)));
    
    m_currentSourceFile = sourceFile;
    
    if(m_currentSourceFile)
        connect(m_currentSourceFile, SIGNAL(handleDrop(QDropEvent*)), this, SLOT(toolBarDropEvent(QDropEvent*)));
}
