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
, m_connection(NULL)
{
}

void QNodeViewEditor::install(QGraphicsScene* scene)
{
    Q_ASSERT(scene);
    scene->installEventFilter(this);
    m_scene = scene;
}

bool QNodeViewEditor::eventFilter(QObject* object, QEvent* event)
{
    QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

    switch (static_cast<qint32>(event->type()))
	{
        case QEvent::GraphicsSceneMousePress:
        {
            switch (static_cast<qint32>(mouseEvent->button()))
            {
                case Qt::LeftButton:
                {
                    QGraphicsItem* item = itemAt(mouseEvent->scenePos());
                    if (!item)
                        break;

                    if (item->type() == QNodeViewType_Port)
                    {
                        m_connection = new QNodeViewConnection(NULL);
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
                    QGraphicsItem* item = itemAt(mouseEvent->scenePos());
                    if (!item)
                        break;

                    if (item->type() == QNodeViewType_Connection || item->type() == QNodeViewType_Block)
                        delete item;

                    break;
                }
            }
        }

        case QEvent::GraphicsSceneMouseMove:
        {
            if (m_connection)
            {
                m_connection->setEndPosition(mouseEvent->scenePos());
                m_connection->updatePath();
                return true;
            }

            break;
        }

        case QEvent::GraphicsSceneMouseRelease:
        {
            if (m_connection && mouseEvent->button() == Qt::LeftButton)
            {
                QGraphicsItem* item = itemAt(mouseEvent->scenePos());
                if (item && item->type() == QNodeViewType_Port)
                {
                    QNodeViewPort* startPort = m_connection->startPort();
                    QNodeViewPort* endPort = static_cast<QNodeViewPort*>(item);

                    if (startPort->block()    != endPort->block() &&
                        startPort->isOutput() != endPort->isOutput() &&
                        !startPort->isConnected(endPort))
                    {
                        m_connection->setEndPosition(endPort->scenePos());
                        m_connection->setEndPort(endPort);
                        m_connection->updatePath();
                        m_connection = NULL;
                        return true;
                    }
                }

                delete m_connection;
                m_connection = NULL;
                return true;
            }

            break;
        }
	}

    return QObject::eventFilter(object, event);
}

void QNodeViewEditor::save(QDataStream& stream)
{
    Q_FOREACH (QGraphicsItem* item, m_scene->items())
    {
        if (item->type() == QNodeViewType_Block)
		{
            stream << item->type();
            static_cast<QNodeViewBlock*>(item)->save(stream);
		}
    }

    Q_FOREACH (QGraphicsItem* item, m_scene->items())
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

    Q_ASSERT(m_scene);
    m_scene->clear();

    while (!stream.atEnd())
	{
        qint32 type;
        stream >> type;

        if (type == QNodeViewType_Block)
		{
            QNodeViewBlock* block = new QNodeViewBlock(NULL);
            m_scene->addItem(block);
            block->load(stream, portMap);
        }
        else if (type == QNodeViewType_Connection)
		{
            QNodeViewConnection* connection = new QNodeViewConnection(NULL);
            m_scene->addItem(connection);
            connection->load(stream, portMap);
		}
	}
}

QGraphicsItem* QNodeViewEditor::itemAt(const QPointF& point)
{
    Q_ASSERT(m_scene);

    QList<QGraphicsItem*> items = m_scene->items(QRectF(point - QPointF(1, 1), QSize(3, 3)));

    Q_FOREACH (QGraphicsItem* item, items)
    {
        // Filter out non-user scene items
        if (item->type() > QGraphicsItem::UserType)
            return item;
    }

    // No user scene items found at point
    return NULL;
}
