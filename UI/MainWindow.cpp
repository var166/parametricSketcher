#include "MainWindow.h"
#include "../Rendering/Canvas.h"
#include "../GeometryEngine/Sketch.h"
#include "../GeometryEngine/Point.h"
#include "../GeometryEngine/Circle.h"
#include "../GeometryEngine/Ellipse.h"
#include "../Persistence/PersistenceManager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    resize(1200, 800);
    m_sketch = std::make_shared<Sketch>();

    m_canvas = new Canvas(this);
    m_canvas->setSketch(m_sketch);
    setCentralWidget(m_canvas);

    m_toolBar = addToolBar("Tools");
    
    auto addAction = [&](const QString& text, auto slot) {
        QAction* action = new QAction(text, this);
        connect(action, &QAction::triggered, this, slot);
        m_toolBar->addAction(action);
        return action;
    };

    addAction("Select", &MainWindow::setDrawingModeSelect);
    addAction("Point", &MainWindow::setDrawingModePoint);
    addAction("Line", &MainWindow::setDrawingModeLine);
    addAction("Circle", &MainWindow::setDrawingModeCircle);
    addAction("Ellipse", &MainWindow::setDrawingModeEllipse);
    addAction("Bezier", &MainWindow::setDrawingModeBezier);
    m_toolBar->addSeparator();
    addAction("Grid", &MainWindow::toggleGrid);
    m_toolBar->addSeparator();
    addAction("Save", &MainWindow::saveSketch);
    addAction("Load", &MainWindow::loadSketch);

    setupDocks();
    
    connect(m_canvas, &Canvas::selectionChanged, this, &MainWindow::updateProperties);
}

void MainWindow::setupDocks() {
    QDockWidget* propDock = new QDockWidget("Properties", this);
    m_propertyTree = new QTreeWidget();
    m_propertyTree->setHeaderLabels({"Property", "Value"});
    connect(m_propertyTree, &QTreeWidget::itemChanged, this, &MainWindow::onPropertyChanged);
    propDock->setWidget(m_propertyTree);
    addDockWidget(Qt::RightDockWidgetArea, propDock);
}

void MainWindow::setDrawingModeSelect() { m_canvas->setDrawingMode(Canvas::Mode::Select); }
void MainWindow::setDrawingModePoint() { m_canvas->setDrawingMode(Canvas::Mode::Point); }
void MainWindow::setDrawingModeLine() { m_canvas->setDrawingMode(Canvas::Mode::Line); }
void MainWindow::setDrawingModeCircle() { m_canvas->setDrawingMode(Canvas::Mode::Circle); }
void MainWindow::setDrawingModeEllipse() { m_canvas->setDrawingMode(Canvas::Mode::Ellipse); }
void MainWindow::setDrawingModeBezier() { m_canvas->setDrawingMode(Canvas::Mode::Bezier); }
void MainWindow::toggleGrid() { m_canvas->toggleGrid(); }

void MainWindow::updateProperties() {
    m_propertyTree->blockSignals(true);
    m_propertyTree->clear();
    
    if (!m_sketch) {
        m_propertyTree->blockSignals(false);
        return;
    }

    std::shared_ptr<GeometricEntity> selectedEntity = nullptr;
    for (const auto& entity : m_sketch->getEntities()) {
        if (entity->isSelected()) {
            selectedEntity = entity;
            break;
        }
    }

    if (selectedEntity) {
        auto addProp = [&](const QString& name, const QString& value, bool editable = true) {
            auto item = new QTreeWidgetItem(m_propertyTree);
            item->setText(0, name);
            item->setText(1, value);
            if (editable) item->setFlags(item->flags() | Qt::ItemIsEditable);
            return item;
        };

        addProp("Type", QString::fromStdString([&](){
            switch(selectedEntity->getType()){
                case EntityType::Point: return "Point";
                case EntityType::Line: return "Line";
                case EntityType::Circle: return "Circle";
                case EntityType::Ellipse: return "Ellipse";
                case EntityType::BezierCurve: return "BezierCurve";
                case EntityType::RegularPolygon: return "RegularPolygon";
                default: return "Unknown";
            }
        }()), false);

        addProp("Color", selectedEntity->color().name());
        addProp("Thickness", QString::number(selectedEntity->thickness()));
        
        // Specific properties
        if (selectedEntity->getType() == EntityType::Point) {
            auto p = std::dynamic_pointer_cast<Point>(selectedEntity);
            addProp("X", QString::number(p->x()));
            addProp("Y", QString::number(p->y()));
        } else if (selectedEntity->getType() == EntityType::Circle) {
            auto c = std::dynamic_pointer_cast<Circle>(selectedEntity);
            addProp("Radius", QString::number(c->radius()));
        } else if (selectedEntity->getType() == EntityType::Ellipse) {
            auto e = std::dynamic_pointer_cast<Ellipse>(selectedEntity);
            // I should add getters for rx/ry if I want to edit them here, 
            // but for now let's just do color/thickness.
        }
    }

    m_propertyTree->blockSignals(false);
}

void MainWindow::onPropertyChanged(QTreeWidgetItem* item, int column) {
    if (column != 1) return;

    std::shared_ptr<GeometricEntity> selectedEntity = nullptr;
    for (const auto& entity : m_sketch->getEntities()) {
        if (entity->isSelected()) {
            selectedEntity = entity;
            break;
        }
    }

    if (!selectedEntity) return;

    QString propName = item->text(0);
    QString val = item->text(1);

    if (propName == "Color") {
        QColor color(val);
        if (color.isValid()) selectedEntity->setColor(color);
    } else if (propName == "Thickness") {
        bool ok;
        double t = val.toDouble(&ok);
        if (ok) selectedEntity->setThickness(t);
    } else if (propName == "X" || propName == "Y" || propName == "Radius") {
        bool ok;
        double v = val.toDouble(&ok);
        if (ok) {
            auto params = selectedEntity->getParameters();
            if (propName == "X" && params.size() >= 1) *params[0] = v;
            else if (propName == "Y" && params.size() >= 2) *params[1] = v;
            else if (propName == "Radius" && selectedEntity->getType() == EntityType::Circle && params.size() >= 3) *params[2] = v;
        }
    }

    m_canvas->update();
}

void MainWindow::saveSketch() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Sketch", "", "Parametric Sketch (*.psk)");
    if (fileName.isEmpty()) return;

    if (PersistenceManager::saveSketch(m_sketch, fileName)) {
        statusBar()->showMessage("Sketch saved to " + fileName, 3000);
    }
}

void MainWindow::loadSketch() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Sketch", "", "Parametric Sketch (*.psk)");
    if (fileName.isEmpty()) return;

    auto loadedSketch = PersistenceManager::loadSketch(fileName);
    if (loadedSketch) {
        m_sketch = loadedSketch;
        m_canvas->setSketch(m_sketch);
        m_canvas->update();
        statusBar()->showMessage("Sketch loaded from " + fileName, 3000);
    }
}