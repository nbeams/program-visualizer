#include "ProgramVisualizer.h"
#include "ui_ProgramVisualizer.h"
#include <Qsci/qsciscintilla.h>

ProgramVisualizer::ProgramVisualizer() : m_ui(new Ui::ProgramVisualizer), m_currentSourceFile(0),
        m_currentFunction(0), m_currentFlowchartItem(), m_action(QIcon(":/shortcuts/visualizer/eye.png"),"Visualize Program", this)

{
    
   Q_INIT_RESOURCE(program_visualizer);
   
   /*Set up Ui and GraphicsScene (to which program map objects will be added)*/
    m_ui->setupUi(this);
    m_ui->changeFunction->setVisible(false);
    m_ui->treeWidget->setMouseTracking(true);
    m_updateDecision = 1;
    m_currentFlowchartItem = 0;

    m_currentItemText = new QTextDocument(this);
    m_currentItemText->setModified(false);

    m_updateSourceWarning = new UpdateDialog();

    /*Connections*/
    connect(&m_action, SIGNAL(triggered()), this, SLOT(run()));
    connect(m_ui->Refresh, SIGNAL(clicked()), this, SLOT(refresh()));
    connect(m_ui->expandAll, SIGNAL(clicked()), m_ui->treeWidget, SLOT(expandAll()));
    connect(m_ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(setCurrentFlowchartItem()));
    connect(m_ui->functionSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentFunction(int)));
    connect(m_ui->changeFunction, SIGNAL(clicked()), this, SLOT(changeFunction()));
    connect(m_ui->jumpTo, SIGNAL(clicked()), this, SLOT(jumpToPosition()));
    connect(m_ui->textEdit, SIGNAL(textChanged()), this, SLOT(enableUpdate()));
    connect(m_ui->updateSourceFile, SIGNAL(clicked()), this, SLOT(updateSourceFile()));
   // connect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(checkCursorPosition()));
    connect(m_ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(setItemTextFont(QFont)));
    connect(m_ui->fontSize, SIGNAL(valueChanged(int)), this, SLOT(setItemTextPointSize(int)));
    connect(m_ui->applyFont, SIGNAL(clicked()), this, SLOT(setBrowserFont()));
    connect(m_ui->treeWidget, SIGNAL(itemEntered(QTreeWidgetItem*, int)), this, SLOT(setHighlightedBrowserItem(QTreeWidgetItem*, int)));
}

ProgramVisualizer::~ProgramVisualizer()
{
    Q_CLEANUP_RESOURCE(program_visualizer);
    delete m_ui;
}

void ProgramVisualizer::setCurrentFile(SourceFile *sourceFile)
{
    m_currentSourceFile = sourceFile;
}

void ProgramVisualizer::run()
{
    reset();

    if(!m_currentSourceFile)
           QMessageBox::warning(this, "Visualization Error", "A valid file must be open.\n", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);

    else
    {
        int errorCode = 0;

        errorCode = searchForFunctions(m_currentSourceFile->text());
        switch(errorCode)
        {
            case 0:
                setCurrentFunction(0);
                int i;
                for(i = 0; i < m_functionList.length(); i++)
                {
                    m_ui->functionSelector->addItem(m_functionList.at(i)->prototype());
                }

                connect(m_ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(browserItemChanged(QTreeWidgetItem*, int)));
                /*Show the finished Program Visualization window*/
                show();
                break;

            case 1:
                QMessageBox::warning(this, "Visualization Error", "No functions were found to visualize!\n", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
                break;
            case 2:
                QMessageBox::warning(this, "Visualization Error", "An error occurred when breaking the program into functions.\nPlease check for unmatched braces in your function definitions.\n", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
                break;
            case 3:
                QMessageBox::warning(this, "Visualization Error", "An error occurred when searching the functions in the program.\nPlease check for unmatched braces in your function definitions.\n", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
                break;
            case 4:
                QMessageBox::warning(this, "Visualization Error", "An error occurred when searching the functions in the program.\nPlease check for unmatched parentheses in your function definitions.\n", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        }
    }
}

void ProgramVisualizer::refresh()
{
    reset();
   // if(m_updateDecision == 2)
   //     return;
   // else
        run();
}

void ProgramVisualizer::reset()
{
    m_updateDecision = 1;

    if(m_currentFlowchartItem)
    {
        if(m_ui->updateSourceFile->isEnabled())  //item text has been changed
            setUpdateDecision();  //make sure the user doesn't want to lose changes to item text
    }
    if(m_updateDecision == 2)
        return;

    disconnect(m_ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(browserItemChanged(QTreeWidgetItem*, int)));
    disconnect(m_currentItemText, SIGNAL(contentsChange(int, int, int)), this, SLOT(setOffset(int, int, int)));
    disconnect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(checkCursorPosition()));
    m_offsetList.clear();

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
    m_ui->textEdit->clear();

}

int ProgramVisualizer::searchForFunctions(QString sourceFileText)
{
    int line = 0; int index = 0;
    int pos = 0; int endPos = 0; int defPos = 0; int dummyPos = 0; bool ok; int errorCode;
    QString currentPrototype;
    QString currentType; QString currentName;

    /*define regular expression for finding the main function*/
   // QRegExp mainFunction("int\\s+main\\s*\\(\\s*\\)(\\s*\\r*\\n*)*\\{");
   QRegExp mainFunction("int\\s+main\\s*\\(\\s*\\)((\\s*\\r*\\n*)*(/\\*(.(?!\\*/))*.\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
    /*
     *      Translation of QRegExp because it's really confusing-looking:
     *      int\\s+main\\s* -> "int" plus at least one space, followed by "main" and possible whitespace.
     *      \\(\\s*\\) -> parentheses and possible whitespace in between them.
     *      (\\s*\\r*\\n*)* -> any possible combination of whitespace and line returns.
     *///   (/\\*(.(?!\\*/))*.\\*/)* -> any number of "slash-star" comments.
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
    FunctionItem *newFunction;
    defPos = mainFunction.indexIn(sourceFileText, 0);
    if(newFunction->findNext(sourceFileText, mainFunction, &defPos)) //Make sure it's not commented out
    {
        newFunction = new FunctionItem(this, m_ui->treeWidget, "int main()");
        newFunction->setName("int main()");
        defPos = defPos + mainFunction.matchedLength(); //move to opening curly brace
        newFunction->setPosInSource(defPos);  //make a record of where this function is located

        /*Find text of the main function and set this text in its function item*/
        newFunction->setDefinitionText(findFunctionDefinition(sourceFileText, newFunction, defPos));
        if(newFunction->definitionText().toInt(&ok, 10) == 2)
            return 2; //error code for problem counting matching curly braces
        else
        {
            errorCode = newFunction->createFlowchart(); //create visualization for the main function
            if(errorCode == 3)
                return 3; //error code for problem counting braces within a specific function
            else if(errorCode == 4)
                return 4; //error code for problem counting parentheses within a specific function
            else
                m_functionList.append(newFunction); //add to list
        }
    }

    /*Find prototypes*/
    line = 0; index = 0;
    pos = 0; defPos = 0; //reset position variables
    while(pos < functionPrototype.lastIndexIn(sourceFileText) && pos >= 0)
    {
        pos = functionPrototype.indexIn(sourceFileText, endPos); //find location of prototypes

        if(newFunction->findNext(sourceFileText, functionPrototype, &pos)) //Make sure the prototype isn't commented out
        {
            endPos = pos + functionPrototype.matchedLength(); //move cursor to end of current prototype

            /*Set current prototype*/
            currentPrototype = sourceFileText.mid(pos, endPos-1-pos);
            /*Create new function item for this prototype*/
            newFunction = new FunctionItem(this, m_ui->treeWidget, currentPrototype);

            /*Get type and name of function out of the current prototype*/
            dummyPos = findFunctionType.indexIn(currentPrototype, 0);
            currentType = currentPrototype.mid(dummyPos, findFunctionType.matchedLength());
            newFunction->setType(currentType);

            dummyPos = findFunctionName.indexIn(currentPrototype, 0);
            currentName = currentPrototype.mid(dummyPos, findFunctionName.matchedLength()-1);
            newFunction->setName(currentName);

            /*Create RegExp for finding this function's definition*/
            QRegExp currentDefinition(currentType+"\\s*\\*?\\&?\\s+\\*?\\&?\\s*"+currentName+"\\s*\\([^;{]*\\)[^;]((\\s*\\r*\\n*)*(/\\*(.(?!\\*/))*.\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
            /*Search for this RegExp*/
            defPos = currentDefinition.indexIn(sourceFileText, endPos);
            if(defPos > 0)
            {
                defPos = defPos + currentDefinition.matchedLength(); //move to opening curly brace
                newFunction->setPosInSource(defPos); //make a record of where this function is located

                /*Find definition text for this function and set this text in the function item*/
                newFunction->setDefinitionText(findFunctionDefinition(sourceFileText, newFunction, defPos));
                if(newFunction->definitionText().toInt(&ok, 10) == 2)
                    return 2; //error code for problem counting matching curly braces
                else
                {
                    errorCode = newFunction->createFlowchart(); //create visualization for this function
                    if(errorCode == 3)
                        return 3; //error code for problem counting curly braces in an item
                    else if(errorCode == 4)
                        return 4; //error code for problem counting parentheses in an item
                    else
                    {
                        m_functionList.append(newFunction); //add to the funciton list
                        defPos = 0; //reset defPos
                    }
                }
            }
        }
    }

    if(m_functionList.isEmpty()) //an error occurred - couldn't find any functions
        return 1;
    else
        return 0;

}

QString ProgramVisualizer::findFunctionDefinition( QString sourceFileText, FunctionItem *item, int defPos)
{
    int numOpenBraces = 1;
    int bracePos = 0; int closedBracePos = 0;
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

          if(defPos != currentPos)
            defPos = currentPos; //update the definition pos being used as starting point for RegExp search
          else
          {
              definitionText = "2"; //error code 2 - we seem to have un-matched curly braces
              break;
          }
    }
    return definitionText;

}

void ProgramVisualizer::setCurrentFlowchartItem()
{
      int i; FlowchartItem *nextItem;

      if(m_ui->treeWidget->selectedItems().isEmpty())
          return;
     
      for(i = 0; i < m_flowchartList->length(); i++)
      {
        if(m_flowchartList->at(i)->browserItem()->isSelected()) //find item selected in browser window
        {
            if(m_flowchartList->at(i) == m_currentFlowchartItem)  //we already have the correct current item; no need to continue
                return;
        }
      }

      if(m_currentFlowchartItem)
      {
          if(m_ui->updateSourceFile->isEnabled())  //item text has been changed
              setUpdateDecision();  //make sure the user doesn't want to lose changes to item text
          else
              m_updateDecision = 1;

          if(m_updateDecision == 2) //we're staying with the same item, a cancel was chosen
          {
              m_ui->treeWidget->clearSelection();
              m_currentFlowchartItem->browserItem()->setSelected(true);
              return;
          }
      }

      if(m_updateDecision == 1)  //we're moving to a new item
      {
          if(m_currentFlowchartItem)
              disconnect(m_currentItemText, SIGNAL(contentsChange(int, int, int)), this, SLOT(setOffset(int, int, int)));

          disconnect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(checkCursorPosition()));

          if(m_currentFlowchartItem && m_offsetList.length() >= 1)
              m_currentFlowchartItem->resetItemTextDocument(); //undo changes

          m_currentItemText->setModified(false);

          for(i = 0; i < m_flowchartList->length(); i++) //find currently selected item from the item list
          {
              if(m_flowchartList->at(i)->browserItem()->isSelected())
                  m_currentFlowchartItem = m_flowchartList->at(i);
          }

          for(i = 0; i < m_flowchartList->length(); i++)
          {
              if(m_flowchartList->at(i) == m_currentFlowchartItem)
              {
                  /*Expand the necessary items in the program browser window*/
                  nextItem = m_currentFlowchartItem;
                  m_currentFlowchartItem->browserItem()->setExpanded(true);
                  while(nextItem->hasParentItem()) //we need to expand all items up to this point
                  {
                      nextItem = nextItem->parentItem();
                      nextItem->browserItem()->setExpanded(true);
                  }

              }
          }

          /*Check to see if the current flowchart item is for a function call, and if it is, enable the
            change function push button*/
          if(m_currentFlowchartItem->isFunctionCall())
          {
              for(i = 0; i < m_functionList.length(); i++)
              {
                  if(m_functionList.at(i)->name() == m_currentFlowchartItem->functionName()) //this is a user-defined function call
                  {
                      if(!m_ui->changeFunction->isVisible())
                          m_ui->changeFunction->setVisible(true);
                      break;
                  }
                  else  //this is not a user-defined function
                  {
                      if(m_ui->changeFunction->isVisible())
                          m_ui->changeFunction->setVisible(false);
                  }
              }

          }
          else
              m_ui->changeFunction->setVisible(false);

          /*Get the text for the newly selected item*/
          m_currentItemText = m_currentFlowchartItem->itemTextDocument();
          m_currentItemText->setModified(false);
          m_ui->textEdit->setDocument(m_currentItemText);

          connect(m_currentItemText, SIGNAL(contentsChange(int, int, int)), this, SLOT(setOffset(int, int, int)));
          connect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(checkCursorPosition()));
          m_offsetList.clear();

          /*Update font and size of the newly added document*/
          setItemTextFont(m_ui->fontComboBox->currentFont());
          setItemTextPointSize(m_ui->fontSize->value());
      }
      else if(m_updateDecision == 0)
      {
          /*Not working right now*/
       //   updateSourceFile();
      }
}

void ProgramVisualizer::setCurrentFunction(int index)
{
    int i;

    if(m_currentFunction == m_functionList.at(index))
        return; //we don't need to set the current function - we already have the correct one

    if(m_currentFlowchartItem)
    {
        if(m_ui->updateSourceFile->isEnabled())  //item text has been changed
            setUpdateDecision(); //make sure the user doesn't want to lose changes to item text
        else
            m_updateDecision = 1;
    }
    if(m_updateDecision == 2) //changing to a new function was canceled by the user
    {
        for(i = 0; i < m_functionList.length(); i++)
        {
            if(m_functionList.at(i) == m_currentFunction)
                m_ui->functionSelector->setCurrentIndex(i); //set the drop down menu back to the current function
        }
    }
  //  else if(m_updateDecision == 0)
 //        return;

    else
    {
        if(m_currentFlowchartItem)
        {
            disconnect(m_currentItemText, SIGNAL(contentsChange(int, int, int)), this, SLOT(setOffset(int, int, int)));
            disconnect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(checkCursorPosition()));
        }

        if(m_currentFlowchartItem && m_offsetList.length() >= 1)
            m_currentFlowchartItem->resetItemTextDocument(); //undo changes

          m_currentItemText->setModified(false);
          m_offsetList.clear();

        //clear text browser (current flowchart item is no longer selected anyway)
        m_ui->textEdit->clear();
        //remove the "Display This Function" button if it's there
        if(m_ui->changeFunction->isVisible())
            m_ui->changeFunction->setVisible(false);

        if(index != -1)  //valid new function has been selected
        {
            disconnect(m_ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(browserItemChanged(QTreeWidgetItem*, int)));

             /*Set current function item and its list of flowchart items*/
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
            connect(m_ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(browserItemChanged(QTreeWidgetItem*, int)));
        }
    }
}

void ProgramVisualizer::changeFunction()
{
    int i; int newIndex = -1;

    if(m_currentFlowchartItem)
    {
        if(m_ui->updateSourceFile->isEnabled())  //item text has been changed
            setUpdateDecision();  //make sure the user doesn't want to lose changes to item text
        else
            m_updateDecision = 1;
    }
    if(m_updateDecision == 2)
        return;

    else
    {
        if(m_currentFlowchartItem)
        {
            disconnect(m_currentItemText, SIGNAL(contentsChange(int, int, int)), this, SLOT(setOffset(int, int, int)));
            disconnect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(checkCursorPosition()));
        }

       if(m_currentFlowchartItem && m_offsetList.length() >= 1)
            m_currentFlowchartItem->resetItemTextDocument(); //undo changes

        m_currentItemText->setModified(false);
        m_offsetList.clear();

        for(i = 0; i < m_functionList.length(); i++)
        {
            if(m_functionList.at(i)->name() == m_currentFlowchartItem->functionName())
                newIndex = i;
        }

        if(newIndex >= 0) //we have a valid function number to jump to
            m_ui->functionSelector->setCurrentIndex(newIndex);
    }
}

void ProgramVisualizer::jumpToPosition()
{
    int line = 0; int index = 0;
    int pos = m_currentFunction->posInSource();

    if(m_currentFlowchartItem != 0)
    {
        pos = pos + m_currentFlowchartItem->posInText();

        FlowchartItem *nextItem;
        nextItem = m_currentFlowchartItem;

        while(nextItem->hasParentItem()) //we need to add the location of the parent item too
        {
            nextItem = nextItem->parentItem();
            pos = pos + nextItem->beginItemTextPos();
        }
    }

    m_currentSourceFile->lineIndexFromPosition(pos, &line, &index);
    m_currentSourceFile->setCursorPosition(line, index); //move to this location in the source file
    m_currentSourceFile->ensureCursorVisible();
}

void ProgramVisualizer::setUpdateDecision()
{
    /*Run the warning dialog*/
    m_updateSourceWarning->exec();

    switch(m_updateSourceWarning->returnResult())
    {
   /*     case 0:   //the user wants to update the source file
            m_updateDecision = 0;
            break; */

        case 0:   //the user wants to move to a new item without saving the current item text's changes
            m_updateDecision = 1;
            break;

        case 1:   //the user wants to remain where he/she is
            m_updateDecision = 2;
            break;

        default:   //something went wrong with the dialog
            m_updateDecision = 2;
    }

}

void ProgramVisualizer::enableUpdate()
{
    if(m_currentItemText->isModified()&& !m_currentFlowchartItem->isFunctionCall()
                &&  m_currentFlowchartItem->itemType() != "break")
        m_ui->updateSourceFile->setEnabled(true);
    else
        m_ui->updateSourceFile->setEnabled(false);
}

void ProgramVisualizer::updateSourceFile()
{
    QString newItemText;
    FlowchartItem *nextItem;
    int currentFunctionIndex = 0; int currentFlowchartIndex = 0;

    int beginPos = 0; int endPos = 0;
    int beginLine = 0; int beginIndex = 0;
    int endLine = 0; int endIndex = 0; int i;

    /*Find base position of item*/
    int pos = m_currentFunction->posInSource();
    nextItem = m_currentFlowchartItem;

    while(nextItem->hasParentItem()) //we need to add the location of the parent item too
    {
        nextItem = nextItem->parentItem();
        pos = pos + nextItem->beginItemTextPos();
    }
    beginPos = pos; endPos = pos;

    /*Set line and index values for the beginning and end of the item's text in the main document*/
    beginPos = beginPos + m_currentFlowchartItem->beginItemTextPos();
    m_currentSourceFile->lineIndexFromPosition(beginPos, &beginLine, &beginIndex);

    endPos = endPos + m_currentFlowchartItem->endItemTextPos();
    m_currentSourceFile->lineIndexFromPosition(endPos, &endLine, &endIndex);

    /*Delete old item text*/
    m_currentSourceFile->setSelection(beginLine, beginIndex, endLine, endIndex);
       m_currentSourceFile->removeSelectedText();

    /*Add new item text*/
    newItemText = m_ui->textEdit->toPlainText();
    m_currentSourceFile->setCursorPosition(beginLine, beginIndex); //move to this location in the source file
    m_currentSourceFile->insert(newItemText);

    m_currentItemText->setModified(false);
    m_ui->updateSourceFile->setEnabled(false); //we've just updated the source file, so we don't need the button again yet

    /*Save the current function index and flowchart item*/
    currentFunctionIndex = m_ui->functionSelector->currentIndex();

    for(i = 0; i < m_flowchartList->length(); i++) //find currently selected item from the item list
    {
        if(m_flowchartList->at(i) == m_currentFlowchartItem)
            currentFlowchartIndex = i;
    }

    /*Refresh the window to reflect changes to source code*/
    refresh();

    /*Reset the current function and flowchart item to what they were before refreshing*/
    setCurrentFunction(currentFunctionIndex);
    m_flowchartList->at(currentFlowchartIndex)->browserItem()->setSelected(true);
}

void ProgramVisualizer::checkCursorPosition()
{
    if(m_currentFlowchartItem && !(m_ui->textEdit->textCursor().atStart()))
    {
        int pos = 0; int beginPos; int endPos; int basePos; int i; int j;
        int numOfChildren;
        int currentIndex; int endIndex; int currentLevel; int highlightIndex;

        pos = m_ui->textEdit->textCursor().position();
        beginPos = 0; basePos = 0;
        endPos = m_currentFlowchartItem->itemText().length();

        currentIndex = m_flowchartList->indexOf(m_currentFlowchartItem); //index of currently selected item
        endIndex = m_flowchartList->length() - 1;
        highlightIndex = currentIndex;
        i = currentIndex + 1;

        currentLevel = m_currentFlowchartItem->level();
        numOfChildren = m_currentFlowchartItem->numberOfChildren();

        if(numOfChildren != 0)
        {
            while(i < m_flowchartList->length())
            {
                if(m_flowchartList->at(i)->level() == currentLevel) //we've left the item's children
                {
                    endIndex = i;
                    break;
                }
                i++;
            }

            for(i = currentIndex; i <= endIndex; i++)  //add up the absolute position of items within the currently selected flowchart item
            {
                if(i != currentIndex)
                    beginPos = basePos + m_flowchartList->at(i)->posInText();

                if(m_flowchartList->at(i)->itemType() == "doWhile")
                    endPos = basePos + m_flowchartList->at(i)->endDoWhilePos();
                else
                    endPos = basePos + m_flowchartList->at(i)->endItemTextPos();

                if(m_currentFlowchartItem->itemText().mid(endPos, 1) == "}")
                    endPos++;

                /*Allow for changes in the text edit that haven't been updated in the source file*/
                for(j = 0; j < m_offsetList.length(); j++)
                {
                    if(beginPos > m_offsetList.at(j).x())  //the x value is the position where the change occurred
                         beginPos = beginPos + m_offsetList.at(j).y(); //the y value is the number of characters added at that position
                    if(endPos > m_offsetList.at(j).x())
                         endPos = endPos + m_offsetList.at(j).y();
                }

                if(pos >= beginPos && pos <= endPos)  //we are inside this item's text
                {
                    highlightIndex = i;
                    if(i != currentIndex)
                    {
                        if(m_flowchartList->at(i)->level() == currentLevel + 1 || basePos != 0) //this means that it has a parent and the cursor's not located in it
                            basePos = basePos + m_flowchartList->at(i)->beginItemTextPos();
                    }
                }
                if(highlightIndex == i && m_flowchartList->at(i)->numberOfChildren() == 0)  //we can't narrow it down any more
                    break;
            }
        }
        for(i = 0; i < m_flowchartList->length(); i++)
            m_flowchartList->at(i)->setHighlighted(false); //clear old highlighting, if there is any

        /*Set new highlighted item*/
        if(!(m_flowchartList->at(highlightIndex)->level() > currentLevel + 1 && basePos == 0)) //again, it's kind of confusing, but check to make sure we didn't get a child without its parent
            m_flowchartList->at(highlightIndex)->setHighlighted(true);

        checkHighlighted(); //update browser, graphics, and text for new highlighted item
    }
}

void ProgramVisualizer::checkHighlighted()
{
    int i; int j; FlowchartItem *nextItem;
    int currentIndex = -1; int highlightIndex = -1;
    int currentLevel; int numOfChildren = 0;
    int basePos = 0; int beginPos = 0; int endPos = 0;
    bool wasPreviouslyAlt = false;

    if(m_ui->updateSourceFile->isEnabled())
        wasPreviouslyAlt = true;  //the text document had already been altered prior to highlighting a new item

    /*Highlight background*/
    for(i = 0; i < m_flowchartList->length(); i++)
    {
        if(m_flowchartList->at(i) == m_currentFlowchartItem)
            currentIndex = i;
        if(m_flowchartList->at(i)->isHighlighted())
        {
            highlightIndex = i;
            m_flowchartList->at(i)->browserItem()->setBackgroundColor(0, Qt::yellow);
        }
        else
            m_flowchartList->at(i)->browserItem()->setBackgroundColor(0, Qt::white);
    }

    /*Expand items as necessary to see the highlighted item*/
    for(i = 0; i < m_flowchartList->length(); i++)
    {
        if(m_flowchartList->at(i)->isHighlighted())
        {
            /*Expand the necessary items in the program browser window*/
            nextItem = m_flowchartList->at(i);
            m_flowchartList->at(i)->browserItem()->setExpanded(true);
            while(nextItem->hasParentItem()) //we need to expand all items up to this point
            {
                nextItem = nextItem->parentItem();
                nextItem->browserItem()->setExpanded(true);
            }

        }
    }

    /*Highlight item text if it is visible*/
    if(currentIndex >= 0 && highlightIndex >= 0 && highlightIndex != currentIndex)
    {

        QTextCursor *selection = new QTextCursor(m_currentItemText);
        QTextCharFormat *selectedTextForm = new QTextCharFormat();

        /*Reset any old bolding from previous highlights*/
        selection->select(QTextCursor::Document);
        selectedTextForm->setFontWeight(50); //regular weight
        selection->setCharFormat(*selectedTextForm);

        if(!wasPreviouslyAlt)
        {
            m_ui->updateSourceFile->setEnabled(false);  //return to disabled state; all we did was bold some text
            m_currentItemText->setModified(false);
        }

        i = currentIndex + 1;
        currentLevel = m_currentFlowchartItem->level();
        numOfChildren = m_currentFlowchartItem->numberOfChildren();
        
        if(highlightIndex < currentIndex)
            return;
        
        if(m_flowchartList->at(highlightIndex)->level() <= currentLevel)
            return;

        if(numOfChildren != 0)
        {
            FlowchartItem *nextItem = m_flowchartList->at(highlightIndex);

            while(nextItem->hasParentItem() && nextItem->parentItem() != m_currentFlowchartItem) //we need to add the location of the parent item too
            {
                nextItem = nextItem->parentItem();
                basePos = basePos + nextItem->beginItemTextPos();
            }
            beginPos = basePos + m_flowchartList->at(highlightIndex)->posInText();

            if(m_flowchartList->at(highlightIndex)->itemType() == "doWhile")
                endPos = basePos + m_flowchartList->at(highlightIndex)->endDoWhilePos();
            else
                endPos = basePos + m_flowchartList->at(highlightIndex)->endItemTextPos();

            if(m_currentFlowchartItem->itemText().mid(endPos, 1) == "}")
                    endPos++;

            /*Allow for changes in the text edit that haven't been updated in the source file*/
            for(j = 0; j < m_offsetList.length(); j++)
            {
                if(beginPos > m_offsetList.at(j).x())  //the x value is the position where the change occurred
                    beginPos = beginPos + m_offsetList.at(j).y();  //the y value is the number of characters added at that position
                if(endPos > m_offsetList.at(j).x())
                    endPos = endPos + m_offsetList.at(j).y();
            }

            /*Bold text for new highlighted item*/
            if(beginPos <= m_currentItemText->toPlainText().length())
                selection->setPosition(beginPos, QTextCursor::MoveAnchor);
            if(endPos <= m_currentItemText->toPlainText().length())
            {
                if(selection->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, endPos - beginPos))
                {
                    selectedTextForm->setFontWeight(75); //bold weight
                    selection->setCharFormat(*selectedTextForm);
                }
            }

        }
    }
    if(!wasPreviouslyAlt)
    {
        m_ui->updateSourceFile->setEnabled(false);  //return to disabled state; all we did was bold some text
        m_currentItemText->setModified(false);
    }

}

void ProgramVisualizer::setHighlightedBrowserItem(QTreeWidgetItem *item, int column)
{
    int i;

    for(i = 0; i < m_flowchartList->length(); i++)
    {
        if(m_flowchartList->at(i)->browserItem() == item)
            m_flowchartList->at(i)->setHighlighted(true);
        else
            m_flowchartList->at(i)->setHighlighted(false);
    }

    checkHighlighted();
}

void ProgramVisualizer::browserItemChanged(QTreeWidgetItem *item, int column)
{
    int i;
    if(item->backgroundColor(0) == Qt::blue)
    {
        for(i = 0; i < m_flowchartList->length(); i++)
        {
            if(m_flowchartList->at(i)->browserItem() == item)
                m_flowchartList->at(i)->setHighlighted(true);
            else
                m_flowchartList->at(i)->setHighlighted(false);
        }
    }

    checkHighlighted();
}

void ProgramVisualizer::setOffset(int pos, int removed, int added)
{
    QPoint newOffset;

    if(added - removed != 0)
    {   
        newOffset.setX(pos);
        newOffset.setY(added-removed);
        m_offsetList.append(newOffset);

        m_ui->updateSourceFile->setEnabled(true);
        m_currentItemText->setModified(true);
    }

}

void ProgramVisualizer::setItemTextFont(QFont font)
{
    m_ui->textEdit->setFont(font);
    setItemTextPointSize(m_ui->fontSize->value());

    if(m_ui->applyFont->isChecked())
        setBrowserFont();
}

void ProgramVisualizer::setItemTextPointSize(int size)
{
    QFont editedFont;
    editedFont = m_ui->textEdit->font();
    editedFont.setPointSize(size);
    m_ui->textEdit->setFont(editedFont);

    if(m_ui->applyFont->isChecked())
        setBrowserFont();

}

void ProgramVisualizer::setBrowserFont()
{
    if(m_ui->applyFont->isChecked())
    {
        QFont currentFont; int i;
        currentFont = m_ui->textEdit->font();
        for(i = 0; i < m_flowchartList->length(); i++)
            m_flowchartList->at(i)->browserItem()->setFont(0, currentFont);
    }
}


void ProgramVisualizer::closeEvent(QCloseEvent *event)
{
    if(m_currentFlowchartItem)
    {
        if(m_ui->updateSourceFile->isEnabled())  //item text has been changed
            setUpdateDecision();  //make sure the user doesn't want to lose changes to item text
    }
    if(m_updateDecision == 2)
        event->ignore();

    else
    {
        m_currentFlowchartItem = 0;
        reset();
    }
}
