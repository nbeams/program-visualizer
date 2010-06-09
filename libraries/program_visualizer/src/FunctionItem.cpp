#include "FunctionItem.h"

FunctionItem::FunctionItem(QObject *programVisualizer, QTreeWidget *treeWidget, QString prototype) :
        QObject(programVisualizer)
{
    m_prototype = prototype;
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

void FunctionItem::setName(QString name)
{
    m_name = name;
}

void FunctionItem::setType(QString type)
{
    m_type = type;
}

int FunctionItem::createFlowchart()
{
    int errorCode;
    errorCode = searchForKeywords(m_definitionText, 0);

    if(errorCode == 3)
        return 3;

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

    return 0;

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

int FunctionItem::searchForKeywords(QString text, FlowchartItem *parentItem)
{
    QString currentName; QString currentType;
    QString currentItemText;
    bool newItemFound = true; bool ok;
    bool ifFound = false; bool elseIfFound = false; bool elseFound = false; bool ternCondFound = false;
    bool whileFound = false; bool forFound = false; bool doWhileFound = false;
    bool switchFound = false;
    bool functionCallFound = false;
    FlowchartItem *newCase;

    int itemPos = 0; int matchedLength = 0;
    int ifPos = 0; int elseIfPos = 0; int elsePos = 0; int ternCondPos = 0;
    int forPos = 0; int whilePos = 0; int doWhilePos = 0;
    int switchPos = 0;
    int functionCallPos = 0;

    /*Define RegExps for if statements*/
    QRegExp ifStatement("\\bif\\s*\\(\\s*\\W?\\s*\\w+([^\\n\\r]*)?\\s*\\)");
    /*
     *      Translation of this QRegExp:
     *      \\bif\\s*\\( -> Find the word "if" if it is next to a word boundary, followed by possible whitespace and
     *            an open parenthesis.
     *      \\s*\\W?\\s*\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            more possible whitespace, and one or more word characters (variable name).
     *      ([^\\n\\r]*)?\\s*\\) -> Zero or one occurances of a set of characters other than newlines or carriage returns,
     *            followed by possible whitespace and a closed parenthesis.
     */
    QRegExp elseIfStatement("\\belse\\s+if\\s*\\(\\s*\\W?\\s*\\w+([^\\n\\r]*)?\\s*\\)");
    /*
     *      Translation of this QRegExp:
     *      \\belse\\s+if\\s*\\( -> Find the word "else" if it is next to a word boundary, followed by whitespace
     *            and the word "if," followed by possible whitespace and an open parenthesis.
     *      \\s*\\W?\\s*\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            more possible whitespace, and one or more word characters (variable name).
     *      ([^\\n\\r]*)?\\s*\\) -> Zero or one occurances of a set of characters other than newlines or carriage returns,
     *            followed by possible whitespace and a closed parenthesis.
     */
    QRegExp elseStatement("\\belse(?!\\s+if\\s*\\(\\s*\\W?\\s*\\w+([^\\n\\r]*)?\\s*\\))");
    /*
     *      Translation of this QRegExp:
     *      \\belse(?! etc.)
     *          This finds the "else" if it as a) on a word boundary and b) not followed by the rest of the QRegExp
     *          for "else if" statements.
     */
    QRegExp ternConditional("\\b(\\w+([^\\n\\r;]*)+\\s*\\=\\s*)?[^\\s\\n\\r;]*\\s*\\?\\s*[^\\n\\r;]*\\s*\\:\\s*[^\\n\\r;]*\\s*(?=;)");
    /*
     *      Translation of this QRegExp:
     *      \\b(\\w+([^\\n\\r;]*)+\\s*\\=\\s*)? -> Finds at least one word character, followed by any combination of
     *            characters other than newlines, carriage returns, and semicolons (variable name), plus possible whitespace
     *            surrounding an equals sign.  This whole part of the expression can have zero or one occurrances.
     *      [^\\s\\n\\r;]*\\s*\\? -> Characters other than whitespace, newlines, carriage returns, and semicolons, 
     *            followed by a question mark.
     *      \\s*[^\\n\\r;]*\\s*\\:  -> Find another group of characters, this time followed by a colon.
     *      \\s*[^\\n\\r;]*\\s* -> One more group of characters.
     *      (?=;) -> This whole expression must be followed by a semicolon.
     */
    
    /*Define RegExps for loops*/
    QRegExp whileLoop("\\bwhile\\s*\\(\\s*\\W?\\s*\\w+([^\\n\\r]*)?\\s*\\)(?!\\s*;)");
    /*
     *      Translation of this QRegExp:
     *      \\bwhile\\s*\\( -> Finds the word "while" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      \\s*\\W?\\s*\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            more possible whitespace, and one or more word characters (variable name).
     *      ([^\\n\\r]*)?\\s*\\) -> Zero or one occurances of a set of characters other than newlines or carriage returns,
     *            followed by possible whitespace and a closed parenthesis.
     *      (?!\\s*;) ->this whole expression will only match if it is NOT followed by a semicolon (which would mean that
     *            it's actually an independent while loop, not the end of a do-while loop).
     */
    QRegExp forLoop("\\bfor\\s*\\(([^\\n\\r]*;){2}[^\\n\\r]*\\)");
    /*
     *      Translation of this QRegExp:
     *      \\bfor\\s*\\( -> Finds the word "for" if it is not in the middle of a word, followed by possible whitespace
     *           and an open parenthesis.
     *      ([^\\n\\r]*;){2}  ->  Two instances of a combination of characters other than newlines and carriage returns,
     *           each followed by a semicolon (the starting and ending condidtions of the for loop variable).
     *      [^\\n\\r]*\\)  ->  One last instance of a comtination of characters other than newlines and carriage returns
     *           (the change in the foor loop variable), followed by a closed parenthesis.
     */
    QRegExp doWhileBegin("\\bdo(?!\\w+)(\\s*\\n*)*(/\\*[^*/]*\\*/)*(//[^\\n\\r]*\\n)?");
    /*
     *      Translation of this QRegExp:
     *      \\bdo(?!\\w+) -> finds the word "do" if it is not in the middle of a word.
     *      (\\s*\\n*)*  -> Possible combination of whitespace and/or newlines.
     */    //(/\\*[^*/]*\\*/)* -> Could have zero or more slash-star comments.
    /*      (//[^\\n\\r]*\\n)? -> Also could have an inline comment.
     */
    QRegExp doWhileName("while\\s*\\(\\s*\\W?\\w+([^\\n\\r]*)?\\s*\\)(?=\\s*;)");
     /*
     *      Translation of this QRegExp:
     *      while\\s*\\( -> Finds the word "while" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      \\s*\\W?\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            and one or more word characters (variable name).
     *      ([^\\n\\r]*)?\\s*\\) -> Zero or one occurances of a set of characters other than newlines or carriage returns,
     *            followed by possible whitespace and a closed parenthesis.
     *      (?=\\s*;) ->this whole expression will only match if it IS followed by a semicolon (which would mean that
     *            it's actually the end of a do-while loop, not an independent while loop).
     *
     */

    /*Define RegExp for switch statements*/
    QRegExp switchBlock("\\bswitch\\s*\\([^\\n\\r]\\)");
    /*
     *      Translation of this QRegExp:
     *      \\bswitch\\s*\\( -> Finds the word "switch" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      [^\\n\\r]\\) -> Any combination of characters other than newlines or carraige returns, followed by a closed
     *           parenthesis.
     */

    /*Define RegExp for function calls*/
    QRegExp functionCall("\\w+\\s*\\([^;{\\n]*\\)");
    /*
     *      Translation of this QRegExp:
     *      \\w+\\s*\\( -> One or more word characters, possible whitespace, and an open parenthesis.
     *      [^;{\\n]*\\) ->any characters other than semicolons, curly braces, and newlines, possible
     *          whitespace, and a closed parenthesis.
     */

    while(newItemFound)
    {
            if(parentItem && parentItem->itemType() == "switch")
                newCase = findCase(text, parentItem, &itemPos, &newItemFound);

            else
            {
                newItemFound = false; //set to false so the loop will exit if no new item is found after this
                ifFound = false; elseIfFound = false; elseFound = false; ternCondFound = false;
                whileFound = false; forFound = false; doWhileFound = false;
                switchFound = false;

                /*Search for QRegExps*/
                ifPos = ifStatement.indexIn(text, itemPos);
                elseIfPos = elseIfStatement.indexIn(text, itemPos);
                elsePos = elseStatement.indexIn(text, itemPos);
                ternCondPos = ternConditional.indexIn(text, itemPos);
                whilePos = whileLoop.indexIn(text, itemPos);
                forPos = forLoop.indexIn(text, itemPos);
                doWhilePos = doWhileBegin.indexIn(text, itemPos);
                switchPos = switchBlock.indexIn(text, itemPos);
                functionCallPos = functionCall.indexIn(text, itemPos);

                /*Find first if statement that is not in quotes or a comment*/
                ifFound = findNext(text, ifStatement, &ifPos);
                if(ifFound)
                    newItemFound = true;

                /*Find first else if statement that is not in quotes or a comment*/
                elseIfFound = findNext(text, elseIfStatement, &elseIfPos);
                if(elseIfFound)
                    newItemFound = true;

                /*Find first else statement that is not in quotes or a comment*/
                elseFound = findNext(text, elseStatement, &elsePos);
                if(elseFound)
                    newItemFound = true;

                /*Find first ternary conditional statement that is not in quotes or a comment*/
                ternCondFound = findNext(text, ternConditional, &ternCondPos);
                if(ternCondFound)
                    newItemFound = true;

                /*Find first while loop item*/
                whileFound = findNext(text, whileLoop, &whilePos);
                if(whileFound)
                    newItemFound = true;

                /*Find first for loop item*/
                forFound = findNext(text, forLoop, &forPos);
                if(forFound)
                    newItemFound = true;

                /*Find first do while loop item*/
                doWhileFound = findNext(text, doWhileBegin, &doWhilePos);
                if(doWhileFound)
                    newItemFound = true;

                /*Find first switch item*/
                switchFound = findNext(text, switchBlock, &switchPos);
                if(switchFound)
                    newItemFound = true;

                /*Find first function call item*/
                functionCallFound = findNext(text, functionCall, &functionCallPos);
                if(functionCallFound)
                    newItemFound = true;

                /*Set itemPos back to end of file so we can see if there are any items prior to this*/
                itemPos = text.length();

                if(ifFound && ifPos <= itemPos)
                {
                    itemPos = ifPos;
                    matchedLength = ifStatement.matchedLength();
                    currentType = "if";
                }

                if(elseIfFound && elseIfPos <= itemPos)
                {
                    itemPos = elseIfPos;
                    matchedLength = elseIfStatement.matchedLength();
                    currentType = "else if";
                }

                if(elseFound && elsePos <= itemPos)
                {
                    itemPos = elsePos;
                    matchedLength = elseStatement.matchedLength();
                    currentType = "else";
                }

                  if(ternCondFound && ternCondPos <= itemPos)
                {
                    itemPos = ternCondPos;
                    matchedLength = ternConditional.matchedLength();
                    currentType = "tertiary conditional";
                }

                if(whileFound && whilePos <= itemPos)
                {
                    itemPos = whilePos;
                    matchedLength = whileLoop.matchedLength();
                    currentType = "while";
                }

                if(forFound && forPos <= itemPos)
                {
                    itemPos = forPos;
                    matchedLength = forLoop.matchedLength();
                    currentType = "for";
                }

                if(doWhileFound && doWhilePos <= itemPos)
                {
                    itemPos = doWhilePos;
                    matchedLength = doWhileBegin.matchedLength();
                    currentType = "doWhile";
                }

                if(switchFound && switchPos <= itemPos)
                {
                    itemPos = switchPos;
                    matchedLength = switchBlock.matchedLength();
                    currentType = "switch";
                }

                if(functionCallFound && functionCallPos < itemPos)
                {
                    itemPos = functionCallPos;
                    matchedLength = functionCall.matchedLength();
                    currentType = "function call";
                }
            }

        if(newItemFound)
        {
            FlowchartItem *item;

            if(parentItem && parentItem->itemType() == "switch")
                item = newCase; //get item returned by the findCase function
            else
            {
                currentName = text.mid(itemPos, matchedLength);
                itemPos = itemPos + matchedLength;
                /*Find the code associated with this item*/
                if(currentType != "tertiary conditional"&& currentType != "function call")
                    currentItemText = findItemText(text, &itemPos);

                if(currentType == "doWhile")
                {
                    currentName = "do...";
                    itemPos = doWhileName.indexIn(text, itemPos);
                    matchedLength = doWhileName.matchedLength() + 1;
                    currentName.append(text.mid(itemPos, matchedLength));
                    itemPos = itemPos + matchedLength;
                }

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

                if(currentType == "function call")
                {
                    int endOfFunctionCall = itemPos-matchedLength;
                    handleFunctionCall(item->nameText(), item, &endOfFunctionCall);
                    if(endOfFunctionCall >= 0)
                        itemPos = endOfFunctionCall;
                }
                else
                {
                    item->setItemType(currentType);
                    item->setItemText(currentItemText);
                }
            }

            if(item->itemText().toInt(&ok, 10) == 3)
                return 3;
            else
            {
                addFlowchartItem(item);
                /*Check whether this item has any child items*/
                searchForKeywords(item->itemText(), item);
            }
        }
    }
    return 0;
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

            if(*itemPos != currentPos)
                *itemPos = currentPos; //update the position being used as starting point for RegExp search
            else //there seems to be a problem finding the matching curly brace for this item
            {
                itemText = "3";
                break;
            }
        }
    }
    return itemText;
}

FlowchartItem* FunctionItem::findCase(QString text, FlowchartItem *parentItem, int *itemPos, bool *newItemFound)
{
    *newItemFound = false;
    bool caseFound = false; bool defaultFound = false; bool breakFound = false;
    int matchedLength = 0;
    int casePos = 0; int defaultPos = 0; int bracePos = 0; int breakPos = 0;
    FlowchartItem *newCase;
    QString currentType; QString nameText; QString currentItemText;

    QRegExp explicitCase("case\\s+\\w+\\s*:");
    QRegExp defaultCase("default\\s*:");
    QRegExp withBraces("((\\s*\\r*\\n*)*(/\\*[^*/]*\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
    QRegExp breakFromCase("\\bbreak\\s*;");

    casePos = explicitCase.indexIn(text, *itemPos);
    defaultPos = defaultCase.indexIn(text, *itemPos);

    /*Find first case that is not in a comment or quote*/
    caseFound = findNext(text, explicitCase, &casePos);
    if(caseFound)
        *newItemFound = true;

    /*Find default case*/
    defaultFound = findNext(text, defaultCase, &defaultPos);
    if(defaultFound)
        *newItemFound = true;

    *itemPos = text.length();

    if(caseFound && casePos <= *itemPos)
    {
        *itemPos = casePos;
        currentType = "case";
        matchedLength = explicitCase.matchedLength();
    }

    if(defaultFound && defaultPos <= *itemPos)
    {
        *itemPos = defaultPos;
        currentType = "default";
        matchedLength = defaultCase.matchedLength();
    }

    if(*newItemFound)
    {
        nameText = text.mid(*itemPos,  matchedLength);
        *itemPos = *itemPos + matchedLength;
    }

    caseFound = false; defaultFound = false;

    /*Find code associated with this case*/

    /*Set initial positions for searching*/
    bracePos = withBraces.indexIn(text, *itemPos);
    breakPos = breakFromCase.indexIn(text, *itemPos);
    casePos = explicitCase.indexIn(text, *itemPos);
    defaultPos = defaultCase.indexIn(text, *itemPos);

    if(bracePos == *itemPos) //This case's code is enclosed in curly braces
        currentItemText = findItemText(text, itemPos); //So, we can use our regular text-finding code
    else //This case's code does not have curly braces; we must search for breaks or the next case
    {
        breakFound = findNext(text, breakFromCase, &breakPos);
        caseFound = findNext(text, explicitCase, &casePos);
        defaultFound = findNext(text, defaultCase, &defaultPos);

        if(breakFound) //There is a break before the next case
        {
            if((!caseFound || (caseFound && breakPos < casePos)) && (!defaultFound || (defaultFound && breakPos < defaultPos)))
            {
                matchedLength = breakFromCase.matchedLength();
                breakPos = breakPos + matchedLength; //Move to end of break
                currentItemText = text.mid(*itemPos, breakPos-*itemPos); //Get all text through the break
            }
        }
        if(caseFound)
        {
            if((!breakFound || (breakFound && casePos < breakPos)) && (!defaultFound || (defaultFound && casePos < defaultPos)))
            {
                //This case does not have a break
                currentItemText = text.mid(*itemPos, casePos-*itemPos); //Get all text until next case
            }
        }
        if(defaultFound)
        {
            if((!breakFound || (breakFound && defaultPos < breakPos)) && (!caseFound || (caseFound && defaultPos < casePos)))
            {
                //This case does not have a break and is followed by the default case
                currentItemText = text.mid(*itemPos, defaultPos-*itemPos); //Get all text until the default
            }
        }
        if(!breakFound && !caseFound && !defaultFound) //Looks like this is the end of the switch
            currentItemText = text.mid(*itemPos, text.length()-*itemPos-1); //Get rest of switch's text
    }

    /*Create and configure a FlowchartItem based on what has been found*/
    newCase = new FlowchartItem(m_treeWidget, parentItem, 0, nameText);
    newCase->setItemType(currentType);
    newCase->setItemText(currentItemText);

    /*Increase the number of children for the parent*/
    parentItem->setNumberOfChildren(parentItem->numberOfChildren() + 1);
    /*Set the item's level to be one great than its parent*/
    newCase->setLevel(parentItem->level() + 1);

    return newCase;

}

void FunctionItem::handleFunctionCall(QString nameText, FlowchartItem *item, int *endOfFunctionCall)
{
    int namePos; int openPos = 0; int closedPos = 0; int numOpenParen = 1; int matchedLength = 0; int adjust = 0;
    bool openFound = false; bool closedFound = false;
    QString newItemText; QString name;

    QRegExp findFunctionName("\\w+\\s*\\(");
    QRegExp openParen("\\(");
    QRegExp closedParen("\\)");

    /*Find the name of the function*/
    namePos = findFunctionName.indexIn(nameText, 0);
    if(namePos >= 0)
    {
        name = nameText.mid(namePos, findFunctionName.matchedLength()-1);
        item->setFunctionName(name);
    }
    matchedLength = findFunctionName.matchedLength();
    namePos = namePos + matchedLength;

    openPos = openParen.indexIn(nameText, namePos+1);
    closedPos = closedParen.indexIn(nameText, namePos);

    /*Find the end of this particular funciton call by counting open and closed parentheses*/
    while(numOpenParen > 0 && closedPos > 0 && closedPos < nameText.length()-1)
    {
        openFound = findNext(nameText, openParen, &openPos);
        closedFound = findNext(nameText, closedParen, &closedPos);

        if(openFound && openPos < closedPos)
        {
            openPos++;
            numOpenParen++;
        }
        else if(closedFound && (!openFound || (openFound && closedPos < openPos)))
        {
            closedPos++;
            numOpenParen--;
        }
    }

   if(closedPos != -1 && closedPos < nameText.length()-1)
    {
        adjust = 1;
        /*We only need the part of the line of code that deals with this function call*/
        item->setNameText(nameText.mid(namePos-matchedLength, closedPos-(namePos-matchedLength)));
    }

    newItemText = "Function call to: "+name+"\nwith parameters: "+nameText.mid(namePos, closedPos-namePos-adjust);

    if(closedPos != -1 && closedPos < nameText.length()-1) //the function call ends before the line of code does
        *endOfFunctionCall = *endOfFunctionCall + closedPos; //this will tell searchForKeywords to go back to the
                                                             //middle of this line of code and search for more function calls
    else
        *endOfFunctionCall = -1; //this will do nothing back in searchForKeywords

    item->setItemText(newItemText);
    item->setIsFunctionCall(true);
}

bool FunctionItem::findNext(QString text, QRegExp regExp, int *searchPos)
{
    int beginPos = 0; int endPos = 0; int newPos = 0;
    bool itemFound = false; bool notCommentOrQuote = false;

    while(*searchPos >= 0) //a matching expression has been found
    {
        /*Run checkItem until you are sure you have found the last comment to start before the item*/
        while(beginPos <= *searchPos)
        {
            newPos = regExp.indexIn(text, *searchPos);
            notCommentOrQuote = checkItem(text, beginPos, newPos, &endPos);
            if(beginPos != endPos) //a comment or quote was found
                beginPos = endPos; //move to end of the commment or quote
            else
                break;
        }

        /*This item is not inside a comment or quotations*/
        if(notCommentOrQuote)
        {
            *searchPos = newPos;
            itemFound = true;
            break;
        }
        else
        {
            if(newPos >= 0) //a match was found, but it is inside a comment or quote
                *searchPos = endPos; //move position to end of comment and search again
            else //no match was found when starting the search from the current whilePos
                *searchPos = newPos; //foundPos is now -1, so the loop will exit
        }
    }

    return itemFound;
}

bool FunctionItem::checkItem(QString text, int beginPos, int testPos, int *endPos)
{
    int starPos = 0; int linePos = 0; int quotePos = 0;
    bool checked = true;

    /*Define RegExps for comments and quotes*/
    QRegExp starComment("/\\*(.(?!\\*/))*.\\*/");
    QRegExp lineComment("//[^\\n\\r]*\\n");
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

QString FunctionItem::prototype()
{
    return m_prototype;
}

QString FunctionItem::name()
{
    return m_name;
}

QString FunctionItem::type()
{
    return m_type;
}

QString FunctionItem::definitionText()
{
    return m_definitionText;
}

QList<FlowchartItem*>* FunctionItem::flowchartList()
{
    return m_flowchartList;
}
