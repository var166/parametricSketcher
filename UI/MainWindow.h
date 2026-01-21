#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include "../Rendering/Canvas.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void setDrawingModeSelect();
    void setDrawingModePoint();
    void setDrawingModeLine();
    void setDrawingModeCircle();
    void setDrawingModeEllipse();
    void setDrawingModeBezier();
    void toggleGrid();
    void saveSketch();
    void loadSketch();
    void updateProperties();
    void onPropertyChanged(QTreeWidgetItem* item, int column);

private:
    void setupDocks();

    Canvas* m_canvas;
    std::shared_ptr<Sketch> m_sketch;
    QToolBar* m_toolBar;

    QTreeWidget* m_propertyTree;
};

#endif