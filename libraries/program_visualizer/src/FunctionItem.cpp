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

void FunctionItem::setPosInSource(int pos)
{
    m_posInSource = pos;
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
    /*Create top-level item that represents entire function*/
    FlowchartItem *topItem = new FlowchartItem(m_treeWidget, 0, m_prototype);
    topItem->setItemType("top item");
    topItem->setItemText(m_definitionText);
    topItem->setBeginItemTextPos(0);
    topItem->setEndItemTextPos(m_definitionText.length());
    addFlowchartItem(topItem);

    /*Search the function's text for more items*/
    int errorCode;
    errorCode = searchForKeywords(m_definitionText, topItem);

    if(errorCode == 3)
        return 3;

    float currentX = 0; float currentY = 0; int i;

    m_flowchartList->at(0)->setPreviousItem(0);
    for(i = 1; i < m_flowchartList->length(); i++)
    {
        m_flowchartList->at(i)->setPreviousItem(m_flowchartList->at(i-1));
    }

    for(i = 0; i < m_flowchartList->length(); i++)
    {
        currentY = currentY + 55;
        m_flowchartList->at(i)->setLocation(100*m_flowchartList->at(i)->level(), currentY);
    }

    return 0;

}

int FunctionItem::searchForKeywords(QString text, FlowchartItem *parentItem)
{
    QString currentName; QString currentType;
    QString currentItemText;
    bool newItemFound = true; bool ok;
    bool ifFound = false; bool elseIfFound = false; bool elseFound = false; bool ternCondFound = false;
    bool whileFound = false; bool forFound = false; bool doWhileFound = false;
    bool switchFound = false; bool breakFound = false; bool returnFound = false;
    bool functionCallFound = false;
    bool additionalCodeFound = false;
    FlowchartItem *newCase;

    int itemPos = 0; int matchedLength = 0; int posInText = 0; int beginItemTextPos = 0; int endItemTextPos = 0;
    int ifPos = 0; int elseIfPos = 0; int elsePos = 0; int ternCondPos = 0;
    int forPos = 0; int whilePos = 0; int doWhilePos = 0;
    int switchPos = 0; int breakPos = 0; int returnPos = 0;
    int functionCallPos = 0;
    int additionalCodePos = 0;
    int additionalCodeBeginPos = 0; int additionalCodeEndPos = -1;  //this different intitialization is important later

    /*Define RegExps for if statements*/
    QRegExp ifStatement("\\bif\\s*\\(\\s*\\W?\\s*\\w+([^{};]*)?\\s*\\)(?!\\s*;)");
    /*
     *      Translation of this QRegExp:
     *      \\bif\\s*\\( -> Find the word "if" if it is next to a word boundary, followed by possible whitespace and
     *            an open parenthesis.
     *      \\s*\\W?\\s*\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            more possible whitespace, and one or more word characters (variable name).
     *      ([^{};]*)?\\s*\\) -> Zero or one occurances of a set of characters other than curly braces or semicolons,
     *            followed by possible whitespace and a closed parenthesis.
     */
    QRegExp elseIfStatement("\\belse\\s+if\\s*\\(\\s*\\W?\\s*\\w+([^{};]*)?\\s*\\)(?!\\s*;)");
    /*
     *      Translation of this QRegExp:
     *      \\belse\\s+if\\s*\\( -> Find the word "else" if it is next to a word boundary, followed by whitespace
     *            and the word "if," followed by possible whitespace and an open parenthesis.
     *      \\s*\\W?\\s*\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            more possible whitespace, and one or more word characters (variable name).
     *      ([^{};]*)?\\s*\\) -> Zero or one occurances of a set of characters other than curly braces and semicolons,
     *            followed by possible whitespace and a closed parenthesis.
     */
    QRegExp elseStatement("\\belse(?!\\s+if\\s*\\(\\s*\\W?\\s*\\w+([^{};]*)?\\s*\\))");
    /*
     *      Translation of this QRegExp:
     *      \\belse(?! etc.)
     *          This finds the "else" if it as a) on a word boundary and b) not followed by the rest of the QRegExp
     *          for "else if" statements.
     */
    QRegExp ternConditional("\\b(\\w+([^;]*)+\\s*\\=\\s*)?[^\\s;]*\\s*\\?\\s*[^;]*\\s*\\:\\s*[^;]*\\s*(?=;)");
    /*
     *      Translation of this QRegExp:
     *      \\b(\\w+([^\\n\\r;]*)+\\s*\\=\\s*)? -> Finds at least one word character, followed by any combination of
     *            characters other than newlines, carriage returns, and semicolons (variable name), plus possible whitespace
     *            surrounding an equals sign.  This whole part of the expression can have zero or one occurrances.
     *      [^\\s;]*\\s*\\? -> Characters other than whitespace and semicolons,
     *            followed by a question mark.
     *      \\s*[^;]*\\s*\\:  -> Find another group of characters, this time followed by a colon.
     *      \\s*[^;]*\\s* -> One more group of characters.
     *      (?=;) -> This whole expression must be followed by a semicolon.
     */
    
    /*Define RegExps for loops*/
    QRegExp whileLoop("\\bwhile\\s*\\(\\s*\\W?\\s*\\w+([^{};]*)?\\s*\\)(?!\\s*;)");
    /*
     *      Translation of this QRegExp:
     *      \\bwhile\\s*\\( -> Finds the word "while" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      \\s*\\W?\\s*\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            more possible whitespace, and one or more word characters (variable name).
     *      ([^{};]*)?\\s*\\) -> Zero or one occurances of a set of characters other than curly braces and semicolons,
     *            followed by possible whitespace and a closed parenthesis.
     *      (?!\\s*;) ->this whole expression will only match if it is NOT followed by a semicolon (which would mean that
     *            it's actually an independent while loop, not the end of a do-while loop).
     */
    QRegExp forLoop("\\bfor\\s*\\(([^{};]*;){2}[^{};]*\\)(?!\\s*;)");
    /*
     *      Translation of this QRegExp:
     *      \\bfor\\s*\\( -> Finds the word "for" if it is not in the middle of a word, followed by possible whitespace
     *           and an open parenthesis.
     *      ([^{};]*;){2}  ->  Two instances of a combination of characters other than braces and semicolons,
     *           each followed by a semicolon (the starting and ending condidtions of the for loop variable).
     *      [^{};]*\\)  ->  One last instance of a comtination of characters other than curly braces and semicolons
     *           (the change in the foor loop variable), followed by a closed parenthesis.
     */
    QRegExp doWhileBegin("\\bdo(?!\\w+)(\\s*\\n*)*(/\\*(.(?!\\*/))*.\\*/)*(//[^\\n\\r]*\\n)?");
    /*
     *      Translation of this QRegExp:
     *      \\bdo(?!\\w+) -> finds the word "do" if it is not in the middle of a word.
     *      (\\s*\\n*)*  -> Possible combination of whitespace and/or newlines.
     */    //(/\\*(.(?!\\*/))*.\\*/)* -> Could have zero or more slash-star comments.
    /*      (//[^\\n\\r]*\\n)? -> Also could have an inline comment.
     */
    QRegExp doWhileName("while\\s*\\(\\s*\\W?\\w+([^{};]*)?\\s*\\)(?=\\s*;)");
     /*
     *      Translation of this QRegExp:
     *      while\\s*\\( -> Finds the word "while" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      \\s*\\W?\\w+ -> Possible whitespace, zero or one non-word characters (maybe a ! preceding a boolean variable),
     *            and one or more word characters (variable name).
     *      ([^{};]*)?\\s*\\) -> Zero or one occurances of a set of characters other than curly braces and semicolons,
     *            followed by possible whitespace and a closed parenthesis.
     *      (?=\\s*;) ->this whole expression will only match if it IS followed by a semicolon (which would mean that
     *            it's actually the end of a do-while loop, not an independent while loop).
     *
     */

    /*Define RegExp for switch statements*/
    QRegExp switchBlock("\\bswitch\\s*\\([^{};]*\\)(?!\\s*;)");
    /*
     *      Translation of this QRegExp:
     *      \\bswitch\\s*\\( -> Finds the word "switch" if it is not in the middle of a word, followed by possible
     *           whitespace and an open parenthesis.
     *      [^{};]*\\) -> Any combination of characters other than curly braces and semicolons, followed by a closed
     *           parenthesis.
     */

    QRegExp breakStatement("\\bbreak\\s*;");
    /*
     *      Translation of this QRegExp:
     *      \\bbreak -> Finds the word "break" if it is not in the middle of a word.
     *      \\s*; ->Possible whitespace, followed by a semicolon.
     */

    QRegExp returnStatement("\\breturn\\s*\\(?[^{};]*\\)?\\s*;");
    /*
     *      Translation of this QRegExp:
     *      \\breturn\\s* -> Finds the keyword "return" if it is not in the middle of a word, followed  by possible
     *          whitespace.
     *      \\(?[^{};]*\\)? -> There may be zero or more occurances of characters other than {} and ; enclosed in
     *          zero or one set of parentheses.
     *      \\s*; -> Possible whitespace and a semicolon finish the regular expression.
     */

    /*Define RegExp for function calls*/
    QRegExp functionCall("\\w+\\s*\\([^;{}]*\\)");
    /*
     *      Translation of this QRegExp:
     *      \\w+\\s*\\( -> One or more word characters, possible whitespace, and an open parenthesis.
     *      [^;{}]*\\) ->any characters other than semicolons and curly braces, possible
     *          whitespace, and a closed parenthesis.
     */

    QRegExp semicolon(";");
    /*
     *      Translation of this QRegExp:
     *      Just kidding!  I should think it doesn't need a translation.
     */

    while(newItemFound)
    {
            if(parentItem && parentItem->itemType() == "switch")
                newCase = findCase(text, parentItem, &itemPos, &newItemFound);

            else
            {
                /*Resets*/
                newItemFound = false; //set to false so the loop will exit if no new item is found after this
                ifFound = false; elseIfFound = false; elseFound = false; ternCondFound = false;
                whileFound = false; forFound = false; doWhileFound = false;
                switchFound = false; breakFound = false; returnFound = false;
                functionCallFound = false; additionalCodeFound = false;
                additionalCodeEndPos = -1;

                /*Search for QRegExps*/
                ifPos = ifStatement.indexIn(text, itemPos);
                elseIfPos = elseIfStatement.indexIn(text, itemPos);
                elsePos = elseStatement.indexIn(text, itemPos);
                ternCondPos = ternConditional.indexIn(text, itemPos);
                whilePos = whileLoop.indexIn(text, itemPos);
                forPos = forLoop.indexIn(text, itemPos);
                doWhilePos = doWhileBegin.indexIn(text, itemPos);
                switchPos = switchBlock.indexIn(text, itemPos);
                breakPos = breakStatement.indexIn(text, itemPos);
                returnPos = returnStatement.indexIn(text, itemPos);
                functionCallPos = functionCall.indexIn(text, itemPos);
                additionalCodePos = semicolon.indexIn(text, itemPos);

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

                 /*Find first break item*/
                breakFound = findNext(text, breakStatement, &breakPos);
                if(breakFound)
                    newItemFound = true;

                 /*Find first return item*/
                returnFound = findNext(text, returnStatement, &returnPos);
                if(returnFound)
                    newItemFound = true;

                /*Find first function call item*/
                functionCallFound = findNext(text, functionCall, &functionCallPos);
                if(functionCallFound)
                    newItemFound = true;

                additionalCodeFound = findNext(text, semicolon, &additionalCodePos);

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
                    matchedLength = ternConditional.matchedLength() + 1;
                    currentType = "ternary conditional";
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

                if(breakFound && breakPos <= itemPos)
                {
                    itemPos = breakPos;
                    matchedLength = breakStatement.matchedLength();
                    currentType = "break";
                }

                if(returnFound && returnPos <= itemPos)
                {
                    itemPos = returnPos;
                    matchedLength = returnStatement.matchedLength();
                    currentType = "return";
                }

                if(functionCallFound && functionCallPos < itemPos) //note <, not <= on this one - important for not mis-classifying earlier items!
                {
                    itemPos = functionCallPos;
                    matchedLength = functionCall.matchedLength();
                    currentType = "function call";
                }

                if(additionalCodeFound)
                {
                    while(additionalCodePos >=0 && additionalCodePos < itemPos) //find last valid code statement before the next control sequence item
                    {
                        additionalCodePos++;
                        additionalCodeEndPos = additionalCodePos;
                        additionalCodeFound = findNext(text, semicolon, &additionalCodePos);
                    }
                    if(additionalCodeEndPos >= 0) //some additional code has been found and the position was updated from -1
                    {
                        newItemFound = true;
                        currentType = "additional code";
                    }
                }

            }

        if(newItemFound)
        {
            FlowchartItem *item;

            if(currentType == "additional code")
            {
             /*   if(parentItem && parentItem == m_flowchartList->last())
                    posInText = parentItem->beginItemTextPos();
                else
                    posInText = m_flowchartList->last()->endItemTextPos() + 1; */
                posInText = additionalCodeBeginPos;

                currentItemText = text.mid(posInText, additionalCodeEndPos - posInText);
                if(currentItemText.length() > 1)
                    currentName = "...";
                else
                    currentName = currentItemText;

                if(parentItem)
                {
                    /*Create new flowchart item*/
                    item = new FlowchartItem(m_treeWidget, parentItem, 0, currentName);
                    /*Increase the number of children for the parent*/
                    item->parentItem()->setNumberOfChildren(item->parentItem()->numberOfChildren() + 1);
                    /*Set the item's level to be one great than its parent*/
                    item->setLevel(item->parentItem()->level() + 1);
                }
                else
                    item = new FlowchartItem(m_treeWidget, 0, currentName);

                item->setItemType(currentType);
                item->setItemText(currentItemText);
                item->setPosInText(posInText);
                item->setBeginItemTextPos(posInText);
                item->setEndItemTextPos(additionalCodeEndPos);

                itemPos = additionalCodeEndPos; //move search point ahead

            }

            else if(parentItem && parentItem->itemType() == "switch")
                item = newCase; //get item returned by the findCase function

            else //all other items
            {
                currentName = text.mid(itemPos, matchedLength);
                posInText = itemPos;
                itemPos = itemPos + matchedLength;

                /*Find the code associated with this item*/
                if(currentType != "ternary conditional"&& currentType != "function call" && currentType != "break" && currentType != "return")
                    currentItemText = findItemText(text, &itemPos, &beginItemTextPos, &endItemTextPos);
                else
                {
                    beginItemTextPos = posInText;
                    endItemTextPos = beginItemTextPos + matchedLength;
                    currentItemText.clear();
                }

                if(currentType == "doWhile")
                {
                    currentName = "do...";
                    itemPos = doWhileName.indexIn(text, itemPos);
                    matchedLength = doWhileName.matchedLength() + 1;
                    currentName.append(text.mid(itemPos, matchedLength-1));
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
                    if(parentItem && parentItem->isFunctionCall()) //nested function calls
                    {
                        item->setPosInText(0);
                        item->setBeginItemTextPos(0);
                    }
                    else
                    {
                        item->setPosInText(endOfFunctionCall);
                        item->setBeginItemTextPos(endOfFunctionCall);
                    }
                    handleFunctionCall(item->nameText(), item, &endOfFunctionCall);
                    if(endOfFunctionCall >= 0)
                        itemPos = endOfFunctionCall;
                    else
                        itemPos++;

                    item->setEndItemTextPos(itemPos);
                }
                else
                {
                    item->setItemType(currentType);
                    item->setItemText(currentItemText);
                    item->setPosInText(posInText);
                    item->setBeginItemTextPos(beginItemTextPos);
                    item->setEndItemTextPos(endItemTextPos);
                }
            }

            if(item->itemText().toInt(&ok, 10) == 3)
                return 3;
            else
            {
                if(item->nameText().contains("\n"))
                    item->setNameText(item->nameText().mid(0, item->nameText().indexOf("\n")) + " ...");

                addFlowchartItem(item);

                /*Check whether this item has any child items*/
                if(item->itemType() != "additional code")
                {
                    additionalCodeBeginPos = itemPos;
                    searchForKeywords(item->itemText(), item);
                }
            }
        } 
    }
    return 0;
}

QString FunctionItem::findItemText(QString text, int *itemPos, int *beginItemTextPos, int *endItemTextPos)
{
    QString itemText;
    int oneLinePos = 0; int multiLinePos = 0; int currentPos = 0;
    int bracePos = 0; int closedBracePos = 0;
    int numOpenBraces = 1;

    /*Define RegExps for braces*/
    QRegExp brace("\\{");
    QRegExp closedBrace("\\}");

    /*Define RegExps for finding text associated with visualization items*/
    QRegExp beginOneLineItem("((\\s*\\r*\\n*)*(/\\*(.(?!\\*/))*.\\*/)*(//[^\\n\\r]*\\n)?)*[^\\n;]*;");
    QRegExp beginMultiLineItem("((\\s*\\r*\\n*)*(/\\*(.(?!\\*/))*.\\*/)*(//[^\\n\\r]*\\n)?)*\\{");
    /*
     *      Translation of these QRegExps:
     *      (\\s*\\r*\\n*)* -> Any combination of whitespace, newlines, and carriage returns.
     *///   (/\\*(.(?!\\*/))*.\\*/)* -> Any number of "slash-star" comments.
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
        *beginItemTextPos = *itemPos;
        currentPos = *itemPos + beginOneLineItem.matchedLength();
        itemText.append(text.mid(*itemPos, currentPos-*itemPos));
        *itemPos = currentPos;
        *endItemTextPos = currentPos;
    }

    else if(multiLinePos == *itemPos) //This item has a block of text, and we must count the curly braces to find all of it
    {
        *itemPos = *itemPos + beginMultiLineItem.matchedLength();
        *beginItemTextPos = *itemPos;
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
        *endItemTextPos = *itemPos - 1;
    }

    return itemText;
}

FlowchartItem* FunctionItem::findCase(QString text, FlowchartItem *parentItem, int *itemPos, bool *newItemFound)
{
    *newItemFound = false;
    bool caseFound = false; bool defaultFound = false;
    int matchedLength = 0; int posInText = 0; int beginItemTextPos = 0; int endItemTextPos = 0;
    int casePos = 0; int defaultPos = 0; int bracePos = 0;
    FlowchartItem *newCase;
    QString currentType; QString nameText; QString currentItemText;

    QRegExp explicitCase("case\\s+\\w+\\s*:");
    QRegExp defaultCase("default\\s*:");
    QRegExp withBraces("((\\s*\\r*\\n*)*(/\\*(.(?!\\*/))*.\\*/)*(//[^\\n\\r]*\\n)?)*\\{");

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
        posInText = *itemPos;
        nameText = text.mid(*itemPos,  matchedLength);
        *itemPos = *itemPos + matchedLength;
        beginItemTextPos = *itemPos;
    }

    caseFound = false; defaultFound = false;

    /*Find code associated with this case*/

    /*Set initial positions for searching*/
    bracePos = withBraces.indexIn(text, *itemPos);
    casePos = explicitCase.indexIn(text, *itemPos);
    defaultPos = defaultCase.indexIn(text, *itemPos);

    if(bracePos == *itemPos) //This case's code is enclosed in curly braces
        currentItemText = findItemText(text, itemPos, &beginItemTextPos, &endItemTextPos); //So, we can use our regular text-finding code
    else //This case's code does not have curly braces; we must search for the next case
    {
        caseFound = findNext(text, explicitCase, &casePos);
        defaultFound = findNext(text, defaultCase, &defaultPos);

        if(caseFound)
        {
            if(!defaultFound || (defaultFound && casePos < defaultPos))
            {
                currentItemText = text.mid(*itemPos, casePos-*itemPos); //Get all text until next case
                endItemTextPos = *itemPos + (casePos-*itemPos);
            }
        }
        if(defaultFound)
        {
            if(!caseFound || (caseFound && defaultPos < casePos))
            {
                //This case is followed by the default case
                currentItemText = text.mid(*itemPos, defaultPos-*itemPos); //Get all text until the default
                endItemTextPos = *itemPos + (defaultPos-*itemPos);
            }
        }
        if(!caseFound && !defaultFound) //Looks like this is the end of the switch
        {
            currentItemText = text.mid(*itemPos, text.length()-*itemPos-1); //Get rest of switch's text
            endItemTextPos = text.length()-1;
        }
    }

    /*Create and configure a FlowchartItem based on what has been found*/
    newCase = new FlowchartItem(m_treeWidget, parentItem, 0, nameText);
    newCase->setItemType(currentType);
    newCase->setItemText(currentItemText);
    newCase->setPosInText(posInText);
    newCase->setBeginItemTextPos(beginItemTextPos);
    newCase->setEndItemTextPos(endItemTextPos);

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

int FunctionItem::posInSource()
{
    return m_posInSource;
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
