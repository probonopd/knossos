#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <QObject>
#include <QThread>
#include <PythonQt/PythonQt.h>
#include <PythonQt/PythonQtClassInfo.h>
#include <PythonQt/gui/PythonQtScriptingConsole.h>


class ColorDecorator;
class FloatCoordinateDecorator;
class CoordinateDecorator;
class TreeListDecorator;
class NodeListDecorator;
class SegmentListDecorator;
class MeshDecorator;

class TransformDecorator;
class PointDecorator;

class Highlighter;
class QSettings;

/** This class intializes the python qt engine */
class Scripting : public QThread
{
    Q_OBJECT
public:

    explicit Scripting(QObject *parent = 0);

    //NicePyConsole *console;
    PythonQtScriptingConsole *console;
    CoordinateDecorator *coordinateDecorator;
    FloatCoordinateDecorator *floatCoordinateDecorator;
    ColorDecorator *colorDecorator;
    TreeListDecorator *treeListDecorator;
    NodeListDecorator *nodeListDecorator;
    SegmentListDecorator *segmentListDecorator;
    MeshDecorator *meshDecorator;

    TransformDecorator *transformDecorator;
    PointDecorator *pointDecorator;

    Highlighter *highlighter;

    void run();
signals:
    
public slots:
    static void addScriptingObject(const QString &name, QObject *obj);
    void saveSettings(const QString &key, const QVariant &value);
    void addDoc();
    static void reflect(QObject *obj);
protected:
    QSettings *settings;
};

#endif // SCRIPTING_H
