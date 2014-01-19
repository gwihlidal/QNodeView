/*!
  @file    QNodeViewEditor.cpp

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

#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QNodeViewEditor.h>
#include <QNodeViewPort.h>
#include <QNodeViewConnection.h>
#include <QNodeViewBlock.h>

QNodeViewEditor::QNodeViewEditor(QObject* parent)
: QObject(parent)
, m_connection(nullptr)
{
}

void QNodeViewEditor::install(QGraphicsScene* scene)
{
    Q_ASSERT(scene != nullptr);
    scene->installEventFilter(this);
    m_scene = scene;
}

bool QNodeViewEditor::eventFilter(QObject* object, QEvent* event)
{
    const auto mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

    switch (static_cast<qint32>(event->type()))
	{
        case QEvent::GraphicsSceneMousePress:
        {
            switch (static_cast<qint32>(mouseEvent->button()))
            {
                case Qt::LeftButton:
                {
                    auto item = itemAt(mouseEvent->scenePos());
                    if (item == nullptr)
                        break;

                    if (item->type() == QNodeViewType_Port)
                    {
                        m_connection = new QNodeViewConnection(nullptr);
                        m_scene->addItem(m_connection);
                        m_connection->setStartPort(static_cast<QNodeViewPort*>(item));
                        m_connection->setStartPosition(item->scenePos());
                        m_connection->setEndPosition(mouseEvent->scenePos());
                        m_connection->updatePath();
                        return true;
                    }
                    else if (item->type() == QNodeViewType_Block)
                    {
                        // GW-TODO:
                    }

                    break;
                }

                case Qt::RightButton:
                {
                    auto item = itemAt(mouseEvent->scenePos());
                    if (item == nullptr)
                        break;

                    if (item->type() == QNodeViewType_Connection || item->type() == QNodeViewType_Block)
                        delete item;

                    break;
                }
            }
        }

        case QEvent::GraphicsSceneMouseMove:
        {
            if (m_connection != nullptr)
            {
                m_connection->setEndPosition(mouseEvent->scenePos());
                m_connection->updatePath();
                return true;
            }

            break;
        }

        case QEvent::GraphicsSceneMouseRelease:
        {
            if (m_connection != nullptr && mouseEvent->button() == Qt::LeftButton)
            {
                auto item = itemAt(mouseEvent->scenePos());
                if (item != nullptr && item->type() == QNodeViewType_Port)
                {
                    auto startPort = m_connection->startPort();
                    auto endPort = static_cast<QNodeViewPort*>(item);

                    if (startPort->block()    != endPort->block() &&
                        startPort->isOutput() != endPort->isOutput() &&
                        !startPort->isConnected(endPort))
                    {
                        m_connection->setEndPosition(endPort->scenePos());
                        m_connection->setEndPort(endPort);
                        m_connection->updatePath();
                        m_connection = nullptr;
                        return true;
                    }
                }

                delete m_connection;
                m_connection = nullptr;
                return true;
            }

            break;
        }
	}

    return QObject::eventFilter(object, event);
}

void QNodeViewEditor::save(QDataStream& stream)
{
    Q_FOREACH (auto item, m_scene->items())
    {
        if (item->type() == QNodeViewType_Block)
		{
            stream << item->type();
            static_cast<QNodeViewBlock*>(item)->save(stream);
		}
    }

    Q_FOREACH (auto item, m_scene->items())
    {
        if (item->type() == QNodeViewType_Connection)
		{
            stream << item->type();
            static_cast<QNodeViewConnection*>(item)->save(stream);
		}
    }
}

void QNodeViewEditor::load(QDataStream& stream)
{
    QMap<quint64, QNodeViewPort*> portMap;

    Q_ASSERT(m_scene != nullptr);
    m_scene->clear();

    while (!stream.atEnd())
	{
        qint32 type;
        stream >> type;

        if (type == QNodeViewType_Block)
		{
            auto block = new QNodeViewBlock(nullptr);
            m_scene->addItem(block);
            block->load(stream, portMap);
        }
        else if (type == QNodeViewType_Connection)
		{
            auto connection = new QNodeViewConnection(nullptr);
            m_scene->addItem(connection);
            connection->load(stream, portMap);
		}
	}
}

QGraphicsItem* QNodeViewEditor::itemAt(const QPointF& point)
{
    Q_ASSERT(m_scene != nullptr);

    const auto& items = m_scene->items(QRectF(point - QPointF(1, 1), QSize(3, 3)));

    Q_FOREACH (auto item, items)
    {
        // Filter out non-user scene items
        if (item->type() > QGraphicsItem::UserType)
            return item;
    }

    // No user scene items found at point
    return nullptr;
}
