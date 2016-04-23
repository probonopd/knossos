#ifndef NODELISTDECORATOR_H
#define NODELISTDECORATOR_H

#include "coordinate.h"
#include "skeleton/node.h"
#include "skeleton/tree.h"

#include <QObject>

class NodeListDecorator : public QObject
{
    template<typename, std::size_t> friend class Coord;
    Q_OBJECT
public:
    explicit NodeListDecorator(QObject *parent = 0);

signals:

public slots:

    std::uint64_t node_id(nodeListElement *self);
    QList<segmentListElement *> *segments(nodeListElement *self);
    bool is_branch_node(nodeListElement *self);
    QString comment(nodeListElement *self);
    int time(nodeListElement *self);
    float radius(nodeListElement *self);
    treeListElement *parent_tree(nodeListElement *self);
    Coordinate coordinate(nodeListElement *self);
    int mag(nodeListElement *self);
    int viewport(nodeListElement *self);
    bool selected(nodeListElement *self);
    QString static_Node_help();
};

#endif // NODELISTDECORATOR_H
