/*!
  @file    Example.h

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

#pragma once

#include <QMainWindow>
#include <QtWidgets>

class QNodeViewEditor;

class ExampleMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExampleMainWindow(QWidget* parent = NULL);
    ~ExampleMainWindow();

private slots:
    void addBlock();

	void saveFile();
	void loadFile();

private:
    void createMenus();

    void addBlockInternal(const QPointF& position);

private:
    QNodeViewEditor* m_editor;
    QMenu* m_fileMenu;
    QGraphicsView* m_view;
    QGraphicsScene* m_scene;
};
