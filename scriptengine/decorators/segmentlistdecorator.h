#ifndef SEGMENTLISTDECORATOR_H
#define SEGMENTLISTDECORATOR_H

#include <QObject>

class segmentListElement;
class nodeListElement;
class SegmentListDecorator : public QObject
{
    Q_OBJECT
public:
    explicit SegmentListDecorator(QObject *parent = 0);

signals:

public slots:
    nodeListElement * source(segmentListElement *self);
    nodeListElement * target(segmentListElement *self);
    uint64_t source_id(segmentListElement *self);
    uint64_t target_id(segmentListElement *self);
    QString static_Segment_help();
};

#endif // SEGMENTLISTDECORATOR_H
