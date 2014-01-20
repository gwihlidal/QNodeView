/*!
  @file    QNodeViewPort.cpp

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

#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPen>

#include <QNodeViewPort.h>
#include <QNodeViewConnection.h>

QNodeViewPort::QNodeViewPort(QGraphicsItem* parent)
: QGraphicsPathItem(parent)
, m_radius(5)
, m_margin(2)
, m_portFlags(0x0)
{
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

    m_label = new QGraphicsTextItem(this);

    QPainterPath path;
    path.addEllipse(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
    setPath(path);

    setPen(QPen(QColor(100, 100, 100))); // GW-TODO: Expose to QStyle
    setBrush(QColor(155, 155, 155)); // GW-TODO: Expose to QStyle
}

QNodeViewPort::~QNodeViewPort()
{
    Q_FOREACH (QNodeViewConnection* connection, m_connections)
        delete connection;
}

void QNodeViewPort::setBlock(QNodeViewBlock* block)
{
    m_block = block;
}

void QNodeViewPort::setName(const QString& name)
{
    m_name = name;
    m_label->setPlainText(name);
}

void QNodeViewPort::setIsOutput(bool isOutput)
{
    m_isOutput = isOutput;

    const qreal boundingWidth = m_label->boundingRect().width();
    const qreal boundingHalfHeight = m_label->boundingRect().height() / 2;

    if (m_isOutput)
        m_label->setPos(-m_radius - m_margin - boundingWidth, -boundingHalfHeight);
	else
        m_label->setPos(m_radius + m_margin, -boundingHalfHeight);

    m_label->setDefaultTextColor(QColor(155, 155, 155)); // GW-TODO: Expose to QStyle
}

void QNodeViewPort::setPortFlags(qint32 flags)
{
    m_portFlags = flags;

    if (m_portFlags & QNodeViewPortLabel_Type)
    {
        QFont font(scene()->font());
        font.setItalic(true);
        m_label->setFont(font);
        setPath(QPainterPath());
    }
    else if (m_portFlags & QNodeViewPortLabel_Name)
    {
        QFont font(scene()->font());
        font.setBold(true);
        m_label->setFont(font);
        setPath(QPainterPath());
    }
}

void QNodeViewPort::setIndex(quint64 index)
{
    m_index = index;
}

bool QNodeViewPort::isConnected(QNodeViewPort* other)
{
    Q_FOREACH (QNodeViewConnection* connection, m_connections)
    {
        if (connection->startPort() == other || connection->endPort() == other)
            return true;
    }

    return false;
}

bool QNodeViewPort::isOutput()
{
    return m_isOutput;
}


qint32 QNodeViewPort::radius()
{
    return m_radius;
}

QVector<QNodeViewConnection*>& QNodeViewPort::connections()
{
	return m_connections;
}

QNodeViewBlock* QNodeViewPort::block() const
{
	return m_block;
}

quint64 QNodeViewPort::index()
{
    return m_index;
}

QVariant QNodeViewPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemScenePositionHasChanged)
	{
        Q_FOREACH (QNodeViewConnection* connection, m_connections)
		{
            connection->updatePosition();
            connection->updatePath();
		}
	}

	return value;
}
