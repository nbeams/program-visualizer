#ifndef VISUALIZERITEM_H
#define VISUALIZERITEM_H

#include <QTreeWidgetItem>

class VisualizerBrowserItem : public QTreeWidgetItem
{

public:
    VisualizerBrowserItem(QTreeWidget *parent = 0, int type = 0, QString nameText = 0);
    VisualizerBrowserItem(VisualizerItem *parent = 0, int type = 0, QString nameText = 0);

    QString nameText();
    QString expandedText();

private:

    QString m_expandedText;
    QString m_nameText;
    VisualizerBrowserItem *m_parentItem;

};

#endif // VISUALIZERITEM_H
