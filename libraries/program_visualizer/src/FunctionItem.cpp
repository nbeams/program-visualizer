#include "FunctionItem.h"

FunctionItem::FunctionItem(QObject *programVisualizer, QTreeWidget *treeWidget, QString name) :
        QObject(programVisualizer)
{
    m_name = name;
    m_graphicsScene = new VisualizerGraphicsScene(programVisualizer);
    m_treeWidget = treeWidget; //each function item has own pointer to the ui's tree widget in order to make
                                //top-level flowchart and browser items inside this class.
    m_flowchartList = new QList<FlowchartItem*>();
}

FunctionItem::~FunctionItem()
{
}

void FunctionItem::addFlowchartItem(FlowchartItem *item)
{
    m_flowchartList->append(item);
    m_graphicsScene->addItem(item);
}

void FunctionItem::clear()
{
    m_flowchartList->clear();
    m_graphicsScene->clear();
}

void FunctionItem::setDefinitionText(QString definition)
{
    m_definitionText = definition;
}

void FunctionItem::createFlowchart()
{
    searchForKeywords(m_definitionText, 0);

    int level = 0; int highestLevel = 0;
    float width = 0; float widestRectF = 0;
    int numOfChildren = 0; int totalChildren = 0; int previousItemsChildren = 0;
    float currentX = 0; float currentY = 0;
    int i = 0; int j = 0;
    int currentLevel = 0; int nextItemAtLevel = 0;


    for(i = 0; i < m_flowchartList->length(); i++)
    {
        level = m_flowchartList->at(i)->level();
        if(level > highestLevel)
            highestLevel = level;
    }

    //Temporary code just to display the items
    for(i = 0; i < m_flowchartList->length(); i++)
    {
        currentY = currentY + 20;
        m_flowchartList->at(i)->setLocation(100*m_flowchartList->at(i)->level(), currentY);
    }

    /*This part is not working correctly yet*/
  /*  for(currentLevel = 0; currentLevel <= highestLevel; currentLevel++)
    {
        for(i = 0; i < m_flowchartList->length(); i++)
        {
            if(m_flowchartList->at(i)->level() == currentLevel)
            {
                    width = m_flowchartList->at(i)->rectF()->width();
                    if(width > widestRectF) //currently broken
                        widestRectF = width;
            }
        }

        currentX = currentX + widestRectF + 100;  //+100 only needed while width part isn't working

        for(i = 0; i < m_flowchartList->length(); i++)
        {
            numOfChildren = m_flowchartList->at(nextItemAtLevel)->numberOfChildren();

            while(j <= numOfChildren || m_flowchartList->at(nextItemAtLevel)->level() > currentLevel+1)
            {
               if(nextItemAtLevel >= m_flowchartList->length()-2)
                    break;


               if(m_flowchartList->at(nextItemAtLevel)->level() <= currentLevel)
                    break;
               else if(m_flowchartList->at(nextItemAtLevel)->level() == currentLevel + 1)
               {
                    j++;
                    totalChildren++;
               }
               else
                    totalChildren++;

               nextItemAtLevel++;

            }
            currentY = currentY + previousItemsChildren*15;
            previousItemsChildren = totalChildren;
            totalChildren = 0;
            j = 0;

            if(m_flowchartList->at(i)->level() == currentLevel)
            {
                m_flowchartList->at(i)->setLocation(currentX, currentY);
            }
        }

        currentY = 0;
        for(i = 0; i < m_flowchartList->length(); i++)
        {
            if(m_flowchartList->at(i)->level() == currentLevel + 1)
            {
                nextItemAtLevel = i;
                break;
            }
        }
        previousItemsChildren = 0;
    }  */


}

void FunctionItem::searchForKeywords(QString text, FlowchartItem *parentItem)
{
    QString currentName;
    QStringList captured;
    bool newItemFound = true; bool notCommentOrQuote = false;
    bool whileFound = false; bool forFound = false; bool switchFound = false;

    int itemPos = 0; int matchedLength = 0;
    int newPos = 0; int ifPos = 0; int forPos = 0; int whilePos = 0; int switchPos = 0; int functionCallPos = 0;
    int beginPos = 0; int endPos = 0;

    /*Define RegExps for if statements*/
    QRegExp ifStatement("\\bif\\s*\\(\\s*\\W?\\w+([^\\n\\r]*)?\\s*\\)");
    
    /*Define RegExps for loops*/
    QRegExp whileLoop("\\bwhile\\s*\\(\\s*\\W?\\w+([^\\n\\r]*)?\\s*\\)");
    /*
     *      Translation of these QRegExps:
     *      \\bwhile\\s*\\( -> Finds the word "while" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      \\s*\\W?\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            and one or more word characters (variable name).
     *      ([^\\n\\r]*)?\\s*\\) -> Zero or one occurances of a set of characters other than newlines or carriage returns,
     *            followed by possible whitespace and a closed parenthesis.
     */
    QRegExp forLoop("\\bfor\\s*\\(([^\\n\\r]*;){2}[^\\n\\r]*\\)");
    /*
     *      Translation of these QRegExps:
     *      \\bfor\\s*\\( -> Finds the word "for" if it is not in the middle of a word, followed by possible whitespace
     *           and an open parenthesis.
     *      ([^\\n\\r]*;){2}  ->  Two instances of a combination of characters other than newlines and carriage returns,
     *           each followed by a semicolon (the starting and ending condidtions of the for loop variable).
     *      [^\\n\\r]*\\)  ->  One last instance of a comtination of characters other than newlines and carriage returns
     *           (the change in the foor loop variable), followed by a closed parenthesis.
     */

    /*Define RegExps for switch statements*/
    QRegExp switchBlock("\\bswitch\\s*\\([^\\n\\r]\\)");
    /*
     *      Translation of these QRegExps:
     *      \\bswitch\\s*\\( -> Finds the word "switch" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      [^\\n\\r]\\) -> Any combination of characters other than newlines or carraige returns, followed by a closed
     *           parenthesis.
     */

    while(newItemFound)
    {
        newItemFound = false; //set to false so the loop will exit if no new item is found after this
        whileFound = false;
        forFound = false;
        switchFound = false;

        /*Search for QRegExps*/
        whilePos = whileLoop.indexIn(text, itemPos);
        forPos = forLoop.indexIn(text, itemPos);
        switchPos = switchBlock.indexIn(text, itemPos);

        /*Find first while loop item*/
        while(whilePos >= 0) //a while loop has been found
        {
            /*Run checkItem until you are sure you have found the last comment to start before the while item*/
            while(beginPos <= whilePos)
            {
                newPos = whileLoop.indexIn(text, whilePos);
                notCommentOrQuote = checkItem(text, beginPos, newPos, &endPos);
                if(beginPos != endPos) //a comment or quote was found
                   beginPos = endPos; //move to end of the commment or quote
                else
                   break;
            }

            /*This item is not inside a comment or quotations*/
            if(notCommentOrQuote)
            {
                whilePos = newPos;
                whileFound = true;
                newItemFound = true;
                break;
            }
            else
            {
               if(newPos >= 0) //a while loop was found, but it is inside a comment or quote
                   whilePos = endPos; //move position to end of comment and search again
               else //no while loop was found when starting the search from the current whilePos
                   whilePos = newPos; //whilePos is now -1, so the loop will exit
            }
        }
        beginPos = 0; endPos = 0; //reset these variables for the next item search

        /*Find first for loop item*/
        while(forPos >= 0)
        {
            while(beginPos <= forPos)
            {
                newPos = forLoop.indexIn(text, forPos);
                notCommentOrQuote = checkItem(text, beginPos, newPos, &endPos);
                if(beginPos != endPos)
                   beginPos = endPos;
                else
                   break;
            }

            if(notCommentOrQuote)
            {
                forPos = newPos;
                forFound = true;
                newItemFound = true;
                break;
            }
            else
            {
                if(newPos >= 0)
                   forPos = endPos;
               else
                   forPos = newPos;
            }
        }
        beginPos = 0; endPos = 0;

        /*Find first switch item*/
        while(switchPos >= 0)
        {
            while(beginPos <= switchPos)
            {
                newPos = switchBlock.indexIn(text, switchPos);
                notCommentOrQuote = checkItem(text, beginPos, newPos, &endPos);
                if(beginPos != endPos)
                    beginPos = endPos;
                else
                   break;
            }

            if(notCommentOrQuote)
            {
                switchPos = newPos;
                switchFound = true;
                newItemFound = true;
                break;
            }
            else
            {
               if(newPos >= 0)
                    switchPos = endPos;
               else
                    switchPos = newPos;
            }
        }
        beginPos = 0; endPos = 0;

        /*Set itemPos back to end of file so we can see if there are any items prior to this*/
        itemPos = text.length();

        if(whileFound && whilePos <= itemPos)
        {
            itemPos = whilePos;
            matchedLength = whileLoop.matchedLength();
            captured = whileLoop.capturedTexts();
        }

        if(forFound && forPos <= itemPos)
        {
            itemPos = forPos;
            matchedLength = forLoop.matchedLength();
            captured = forLoop.capturedTexts();
        }

        if(switchFound && switchPos <= itemPos)
        {
            itemPos = switchPos;
            matchedLength = switchBlock.matchedLength();
            captured = switchBlock.capturedTexts();
        }
        
        if(newItemFound)
        {
            currentName = captured.at(0);
            FlowchartItem *item;

            if(parentItem)
            {
                item = new FlowchartItem(m_treeWidget, parentItem, 0, currentName);
                /*Increase the number of children for the parent*/
                item->parentItem()->setNumberOfChildren(item->parentItem()->numberOfChildren() + 1);
                /*Set the item's level to be one great than its parent*/
                item->setLevel(item->parentItem()->level() + 1);
            }
            else
               item = new FlowchartItem(m_treeWidget, 0, currentName);

            itemPos = itemPos + matchedLength;
            /*Find the code associated with this item*/
            item->setItemText(findItemText(text, &itemPos));
            addFlowchartItem(item);
            /*Check whether this item has any child items*/
            searchForKeywords(item->itemText(), item);
        }
    }
}

QString FunctionItem::findItemText(QString text, int *itemPos)
{
    QString itemText;
    int oneLinePos = 0; int multiLinePos = 0; int currentPos = 0;
    int bracePos = 0; int closedBracePos = 0;
    int numOpenBraces = 1;

    /*Define RegExps for braces*/
    QRegExp brace("\\{");
    QRegExp closedBrace("\\}");

    /*Define RegExps for finding text associated with visualization items*/
    QRegExp beginOneLineItem("((\\s*\\r*\\n*)*(/\\*[^*/]*\\*/)*(//[^\\n\\r]*\\n)?)*[^\\n;]*;");
    QRegExp beginMultiLineItem("((\\s*\\r*\\n*)*(/\\*[^*/]*\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
    /*
     *      Translation of these QRegExps:
     *      (\\s*\\r*\\n*)* -> Any combination of whitespace, newlines, and carriage returns.
     *///   (/\\*[^*/]*\\*/)* -> Any number of "slash-star" comments.
    /*      (//[^\\n\\r]*\\n)?  -> An in-line comment is possible, as long as it is followed by a new line.
     *      [^\\n;]*;   OR   \\{ -> Either a line of characters other than newlines or semicolons followed by a
     *              a single semicolon (one line of code for one-line loops, ifs, etc.) or and open curly brace for a
     *              block of code.
     */

     /*FIXME*/
     /*Don't know what to do with these - putting them here for now*/
    QRegExp explicitCase("case\\s+\\w*\\s*:");
    QRegExp defaultCase("default\\s*: ");

    /*Search for these RegExps, starting at the current item's position*/
    oneLinePos = beginOneLineItem.indexIn(text, *itemPos);
    multiLinePos = beginMultiLineItem.indexIn(text, *itemPos);

    if(oneLinePos == *itemPos) //this is a one-line item, with a match starting right where the name ended
    {
        currentPos = *itemPos + beginOneLineItem.matchedLength();
        itemText.append(text.mid(*itemPos, currentPos-*itemPos));
    }

    else if(multiLinePos == *itemPos) //This item has a block of text, and we must count the curly braces to find all of it
    {
        *itemPos = *itemPos + beginMultiLineItem.matchedLength();
        currentPos = *itemPos;

        while(numOpenBraces > 0)
        {
            /*Search for curly braces in the text*/
            bracePos = brace.indexIn(text, *itemPos);
            closedBracePos = closedBrace.indexIn(text, *itemPos);

            if(bracePos >= 0 && bracePos < closedBracePos) //open curly brace has been found, and it is prior to a closing brace
            {
                if(checkItem(text, *itemPos, bracePos, &currentPos)) //the brace is not inside a comment or quotation marks
                {
                    currentPos = bracePos + 1; //move position past brace
                    numOpenBraces++; //increment the number of open braces
                }

                itemText.append(text.mid(*itemPos, currentPos-*itemPos)); //add this section of code to the item's text
            }

            else if(closedBracePos >= 0) //closing curly brace has been found
            {
                if(checkItem(text, *itemPos, closedBracePos, &currentPos))
                {
                    currentPos = closedBracePos + 1; //move position past brace
                    numOpenBraces--;  //decrease the number of open curly braces - one has been closed
                }

                itemText.append(text.mid(*itemPos, currentPos-*itemPos)); //add this section of code to the item's text
                if(numOpenBraces == 0) //we have reached the end of this block of text
                        itemText.chop(1); //chop off last curly brace - we don't need it
            }
            *itemPos = currentPos; //update the position being used as starting point for RegExp search
        }
    }
    return itemText;
}

bool FunctionItem::checkItem(QString text, int beginPos, int testPos, int *endPos)
{
    int starPos = 0; int linePos = 0; int quotePos = 0;
    bool checked = true;

    /*Define RegExps for comments and quotes*/
    QRegExp starComment("/\\*(\\w*\\W*)*\\*/");
    QRegExp lineComment("//[^\\n\\r]*");
    QRegExp quote("\"[^\\n\\r]*\"");

    starPos = starComment.indexIn(text, beginPos);
    linePos = lineComment.indexIn(text, beginPos);
    quotePos = quote.indexIn(text, beginPos);

    if(testPos < 0) //invalid item location has been passed
    {
        checked = false;
    }
    else if(starPos >= 0 && starPos < testPos) //starred comment has been found, and it starts before the item being checked
    {
        *endPos = starPos + starComment.matchedLength(); //find end of comment
        if(*endPos > testPos) //item is inside the comment
            checked = false; //this item is in a comment and should be skipped
    }
    else if(linePos >= 0 && linePos < testPos) //line comment has been found prior to item being checked
    {
        *endPos = linePos + lineComment.matchedLength();
        if(*endPos > testPos)
            checked = false;
    }
    else if(quotePos >= 0 && quotePos < testPos) //item may be inside quotes, like a string definition or print statment
    {
        *endPos = quotePos + quote.matchedLength();
        if(*endPos > testPos)
            checked = false;
    }
    else
    {
        checked = true;
         /*
          *   The item found is not inside a comment or quotation marks, and should be counted (for curly braces) or
          *   have its own flowchart item.
          */
    }

    return checked;

}

VisualizerGraphicsScene* FunctionItem::graphicsScene()
{
    return m_graphicsScene;
}

QString FunctionItem::name()
{
    return m_name;
}

QString FunctionItem::definitionText()
{
    return m_definitionText;
}

QList<FlowchartItem*>* FunctionItem::flowchartList()
{
    return m_flowchartList;
}
