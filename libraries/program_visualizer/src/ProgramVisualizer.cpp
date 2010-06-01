#include "ProgramVisualizer.h"
#include "ui_ProgramVisualizer.h"
#include <Qsci/qsciscintilla.h>

ProgramVisualizer::ProgramVisualizer() : m_ui(new Ui::ProgramVisualizer), m_currentSourceFile(0),
        m_currentFunction(0), m_currentFlowchartItem()

{
   /*Set up Ui and GraphicsScene (to which program map objects will be added)*/
    m_ui->setupUi(this);

    connect(m_ui->Refresh, SIGNAL(clicked()), this, SLOT(refresh()));
    connect(m_ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(setCurrentFlowchartItem()));
    connect(m_ui->functionSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentFunction(int)));
}

ProgramVisualizer::~ProgramVisualizer()
{
    delete m_ui;
}

void ProgramVisualizer::setCurrentFile(SourceFile *sourceFile)
{
    m_currentSourceFile = sourceFile;
}

void ProgramVisualizer::run()
{
    if(!m_currentSourceFile)
           QMessageBox::warning(this, "Visualization Error", "A valid file must be open.\n", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);

    else
    {
        searchForFunctions(m_currentSourceFile->text());
        setCurrentFunction(0);
        int i;
        for(i = 0; i < m_functionList.length(); i++)
        {
            m_ui->functionSelector->addItem(m_functionList.at(i)->name());
        }
        /*Show the finished Program Visualization window*/
        show();
    }
}

void ProgramVisualizer::refresh()
{
    reset();
    run();
}

void ProgramVisualizer::reset()
{
    int i; int numFunctions;
    /*Clear all lists, the graphics scene, the browser window, and the text browser*/
    numFunctions = m_functionList.length();
    for(i = numFunctions-1; i >= 0; i--)
    {
        m_ui->functionSelector->removeItem(i);
        m_functionList.at(i)->clear();
    }
    m_functionList.clear();
    m_ui->treeWidget->clear();
    m_ui->textBrowser->clear();

}

void ProgramVisualizer::searchForFunctions(QString sourceFileText)
{
    int line = 0; int index = 0;
    int pos = 0; int endPos = 0; int defPos = 0; int dummyPos = 0;
    QStringList captured;
    QString currentPrototype;
    QString currentType; QString currentName;

    /*define regular expression for finding the main function*/
   // QRegExp mainFunction("int\\s+main\\s*\\(\\s*\\)(\\s*\\r*\\n*)*\\{");
   QRegExp mainFunction("int\\s+main\\s*\\(\\s*\\)((\\s*\\r*\\n*)*(/\\*[^*/]*\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
    /*
     *      Translation of QRegExp because it's really confusing looking:
     *      int\\s+main\\s* -> "int" plus at least one space, followed by "main" and possible whitespace.
     *      \\(\\s*\\) -> parentheses and possible whitespace in between them.
     *      (\\s*\\r*\\n*)* -> any possible combination of whitespace and line returns.
     *///   (/\\*[^*/]*\\*/)* -> any number of "slash-star" comments.
    /*      (//[^\\n\\r]*\\n)?  -> an in-line comment is possible, as long as it is followed by a new line.
     *      \\{ -> open curly brace beginning definition of main function.
     */

    /*Define regular expression for finding function prototypes*/
    QRegExp functionPrototype("(void|int|char|float|double|(struct\\s+\\w+))\\s*\\*?\\&?\\s+\\*?\\&?\\s*\\w+\\s*\\([^;{]*\\)\\s*;");
    /*
     *  Translation of this QRegExp:
     *  (void|int|char|float|double|(struct\\s+\\w+)) -> Prototype must start with one of these
     *               words or with struct + whitespace + letters or digits (function type).
     *  \\s*\\*?\\&?\\s+\\*?\\&?\\s*\\w+ -> Function type could be followed by zero or one of * and & (for pointers).
     *               Also must have whitespace and at least one or more letters or digits (function name).
     *  \\s*\\([^;{]*\\)\\s*; -> Function name might be followed by whitespace and must be followed by
     *               open parenthesis. Optional combination of characters other than semicolons and { (these
     *               are any parameters that might be present), followed by a closed parenthesis, possible
     *               whitespace, and the ending semicolon.
     */

     /*Define regular expressions used for later creating a RegExp that finds specific function definitions*/
    QRegExp findFunctionType("(void|int|char|float|double|(struct\\s+\\w+))");
    QRegExp findFunctionName("\\w+\\s*\\(");

    /*Find main() and get its text*/
    defPos = mainFunction.indexIn(sourceFileText, 0);
    FunctionItem *newFunction = new FunctionItem(this, m_ui->treeWidget, "int main()");
    if(defPos >= 0) //main function has been found!
    {
        defPos = defPos + mainFunction.matchedLength(); //move to opening curly brace
        /*Find text of the main function and set this text in its function item*/
        newFunction->setDefinitionText(findFunctionDefinition(sourceFileText, newFunction, defPos));
        newFunction->createFlowchart(); //create visualization for the main function
        m_functionList.append(newFunction); //add to list
    }

    /*Find prototypes*/
    line = 0; index = 0;
    pos = 0; defPos = 0; //reset position variables
    while(pos < functionPrototype.lastIndexIn(sourceFileText) && pos >= 0)
    {
        pos = functionPrototype.indexIn(sourceFileText, endPos); //find location of prototypes
        endPos = pos + functionPrototype.matchedLength(); //move cursor to end of current prototype
        captured = functionPrototype.capturedTexts(); //create list of texts that match all or part of reg exp

        /*Set current prototype to first entry in captured text list (which is the entire match of the reg exp)*/
        currentPrototype = captured.at(0);
        currentPrototype.chop(1); //remove semicolon from end; we don't need it
        /*Create new function item for this prototype*/
        FunctionItem *newFunction = new FunctionItem(this, m_ui->treeWidget, currentPrototype);

        /*Get type and name of function out of the current prototype*/
        dummyPos = findFunctionType.indexIn(currentPrototype, 0);
        captured = findFunctionType.capturedTexts();
        currentType = captured.at(0);

        dummyPos = findFunctionName.indexIn(currentPrototype, 0);
        captured = findFunctionName.capturedTexts();
        currentName = captured.at(0);
        currentName.chop(1); //chop ( off the end of the name

        /*Create RegExp for finding this function's definition*/
        QRegExp currentDefinition(currentType+"\\s*\\*?\\&?\\s+\\*?\\&?\\s*"+currentName+"\\s*\\([^;{]*\\)[^;]((\\s*\\r*\\n*)*(/\\*[^*/]*\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
        /*Search for this RegExp*/
        defPos = currentDefinition.indexIn(sourceFileText, endPos);
        if(defPos > 0)
        {
            defPos = defPos + currentDefinition.matchedLength(); //move to opening curly brace
            /*Find definition text for this function and set this text in the function item*/
            newFunction->setDefinitionText(findFunctionDefinition(sourceFileText, newFunction, defPos));
            newFunction->createFlowchart(); //create visualization for this function
            m_functionList.append(newFunction); //add to the funciton list
            defPos = 0; //reset defPos
        }
    }

}

QString ProgramVisualizer::findFunctionDefinition( QString sourceFileText, FunctionItem *item, int defPos)
{
    int numOpenBraces = 1;
    int bracePos = 0; int closedBracePos = 0; //int starPos = 0; int linePos = 0; int quotePos = 0;
    int currentPos = 0;
    QString definitionText;

    /*Define RegExps for comments, quotes, and braces*/
    QRegExp brace("\\{");
    QRegExp closedBrace("\\}");
    
    while(numOpenBraces > 0)
    {
        /*Search for next occurences of the RegExps in the text*/
        bracePos = brace.indexIn(sourceFileText, defPos);
        closedBracePos = closedBrace.indexIn(sourceFileText, defPos);

        if(bracePos >= 0 && bracePos < closedBracePos) //open curly brace has been found, and it is prior to a closing brace
            {
                if(item->checkItem(sourceFileText, defPos, bracePos, &currentPos)) //the brace is not inside a comment or quotation marks
                {
                    currentPos = bracePos + 1; //move position past brace
                    numOpenBraces++; //increment the number of open braces
                }

                definitionText.append(sourceFileText.mid(defPos, currentPos-defPos)); //add this section of code to the definition
            }

            else if(closedBracePos >= 0) //closing curly brace has been found
            {
                if(item->checkItem(sourceFileText, defPos, closedBracePos, &currentPos))
                {
                    currentPos = closedBracePos + 1; //move position past brace
                    numOpenBraces--;  //decrease the number of open curly braces - one has been closed
                }

                definitionText.append(sourceFileText.mid(defPos, currentPos-defPos)); //add this section of code to the definition
                if(numOpenBraces == 0) //we have reached the end of this block of text
                        definitionText.chop(1); //chop off last curly brace - we don't need it
            }
        defPos = currentPos; //update the definition pos being used as starting point for RegExp search

    }
    return definitionText;

}

void ProgramVisualizer::setCurrentFlowchartItem()
{
      int i;
      if(m_ui->treeWidget->selectedItems().isEmpty())
          return;
      else
      {

          for(i = 0; i < m_flowchartList->length(); i++) //find currently selected item from the item list
          {
              if(m_flowchartList->at(i)->browserItem()->isSelected())
                  m_currentFlowchartItem = m_flowchartList->at(i);
          }
          for(i = 0; i < m_flowchartList->length(); i++)
          {
              if(m_flowchartList->at(i) == m_currentFlowchartItem)
              {
                  m_currentFlowchartItem->browserItem()->setExpanded(true);
              }
              else
              {
                  /*Reset anything that was previously selected*/
                  m_ui->treeWidget->expandAll();

                  if(!m_flowchartList->at(i)->isVisible())
                      m_flowchartList->at(i)->show();
              }
          }

          m_ui->textBrowser->setText(m_currentFlowchartItem->itemText());
      }
}

void ProgramVisualizer::setCurrentFunction(int index)
{
    int i;

    //clear text browser (current flowchart item is no longer selected anyway)
    m_ui->textBrowser->clear();

    if(index != -1)  //valid function has been selected
    {
        /*set current function item and its list of flowchart items*/
        m_currentFunction = m_functionList.at(index);
        m_currentScene = m_currentFunction->graphicsScene();
        m_flowchartList = m_currentFunction->flowchartList();

        /*Set correct graphics scene*/
        m_ui->viewer->setScene(m_currentFunction->graphicsScene());

        /*Clear out browser items and replace with new ones*/
        m_ui->treeWidget->clear();
        for(i = 0; i < m_flowchartList->length(); i++)
        {
            m_flowchartList->at(i)->createBrowserItem();
        }
    }
}

void ProgramVisualizer::closeEvent(QCloseEvent *event)
{
    reset();
}
