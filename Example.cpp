/*!
  @file    Example.cpp

  Copyright (c) 2014 Graham Wihlidal

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  @author  Graham Wihlidal
  @date    January 19, 2014
*/

#include <QGuiApplication>
#include <QGraphicsScene>
#include <QFileDialog>

#include <QNodeViewBlock.h>
#include <QNodeViewEditor.h>
#include <QNodeViewPort.h>
#include <QNodeViewCanvas.h>

#include <Example.h>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    ExampleMainWindow window;
    window.show();
    return application.exec();
}

ExampleMainWindow::ExampleMainWindow(QWidget* parent)
: QMainWindow(parent)
{
    setWindowTitle(tr("QNodeView Example"));

    createMenus();

    m_scene = new QGraphicsScene();

    QDockWidget* dock = new QDockWidget(tr("Nodes"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_view = new QNodeViewCanvas(m_scene, dock);
    dock->setWidget(m_view);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    m_editor = new QNodeViewEditor(this);
    m_editor->install(m_scene);

    addBlockInternal(QPointF(0, 0));
    addBlockInternal(QPointF(150, 0));
    addBlockInternal(QPointF(150, 150));
}

ExampleMainWindow::~ExampleMainWindow()
{
}

void ExampleMainWindow::addBlock()
{
    addBlockInternal(m_view->sceneRect().center().toPoint());
}

void ExampleMainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName();
    if (fileName.isEmpty())
		return;

    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QDataStream stream(&file);
    m_editor->save(stream);
}

void ExampleMainWindow::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName();
    if (fileName.isEmpty())
		return;

    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QDataStream stream(&file);
    m_editor->load(stream);
}

void ExampleMainWindow::createMenus()
{
    QAction* quitAction = new QAction(tr("&Quit"), this);
    quitAction->setShortcuts(QKeySequence::Quit);
    quitAction->setStatusTip(tr("Quit the example"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QAction* loadAction = new QAction(tr("&Load"), this);
    loadAction->setShortcuts(QKeySequence::Open);
    loadAction->setStatusTip(tr("Load node view from file"));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFile()));

    QAction* saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save node view to file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

    QAction* addAction = new QAction(tr("&Add"), this);
    addAction->setStatusTip(tr("Add new block"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(addBlock()));

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(addAction);
    m_fileMenu->addAction(loadAction);
    m_fileMenu->addAction(saveAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(quitAction);
}

void ExampleMainWindow::addBlockInternal(const QPointF& position)
{
    QNodeViewBlock* block = new QNodeViewBlock(NULL);
    m_scene->addItem(block);

    static qint32 index = 1;
    QString blockName = QString("myTest%1").arg(index++);

    block->addPort(blockName, 0, QNodeViewPortLabel_Name);
    block->addPort("TestEntity", 0, QNodeViewPortLabel_Type);

    block->addInputPort("Input 1");
    block->addInputPort("Input 2");
    block->addInputPort("Input 3");

    block->addOutputPort("Output 1");
    block->addOutputPort("Output 2");
    block->addOutputPort("Output 3");
    block->addOutputPort("Output 4");
    block->setPos(position);
}
