/*!
  @file    QNodeViewConnection.cpp

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

#include <QBrush>
#include <QPen>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <QNodeViewConnection.h>
#include <QNodeViewPort.h>

QNodeViewConnectionSplit::QNodeViewConnectionSplit(QNodeViewConnection* connection)
: QGraphicsPathItem(NULL)
, m_connection(connection)
, m_radius(5)
{
    setCacheMode(DeviceCoordinateCache);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

QNodeViewConnectionSplit::~QNodeViewConnectionSplit()
{

}

void QNodeViewConnectionSplit::setSplitPosition(const QPointF& position)
{
    m_splitPosition = position;

    if (pos() != position)
        setPos(m_splitPosition);
}

void QNodeViewConnectionSplit::updatePath()
{
    QPainterPath path;
    path.moveTo(m_splitPosition);
    path.addEllipse(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
    setPath(path);
}

void QNodeViewConnectionSplit::save(QDataStream& stream)
{
    Q_UNUSED(stream);
}

void QNodeViewConnectionSplit::load(QDataStream& stream, const QMap<quint64, QNodeViewPort*>& portMap)
{
    Q_UNUSED(stream);
    Q_UNUSED(portMap);
}

void QNodeViewConnectionSplit::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    // Only paint dirty regions for increased performance
    painter->setClipRect(option->exposedRect);

    if (isSelected())
    {
        painter->setBrush(QColor(180, 180, 180)); // GW-TODO: Expose to QStyle
    }
    else
    {
        painter->setBrush(QColor(155, 155, 155)); // GW-TODO: Expose to QStyle
    }

    painter->drawPath(path());
}

QVariant QNodeViewConnectionSplit::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemScenePositionHasChanged)
    {
        const QPointF newPosition = value.toPointF();
        setSplitPosition(newPosition);
        m_connection->updatePath();
    }

    return value;
}

QNodeViewConnection::QNodeViewConnection(QGraphicsItem* parent)
: QGraphicsPathItem(parent)
, m_startPort(NULL)
, m_endPort(NULL)
{
    setCacheMode(DeviceCoordinateCache);
    setPen(QPen(QColor(170, 170, 170), 2)); // GW-TODO: Expose to QStyle
	setBrush(Qt::NoBrush);
	setZValue(-1);
}

QNodeViewConnection::~QNodeViewConnection()
{
    if (m_startPort)
        m_startPort->connections().remove(m_startPort->connections().indexOf(this));

    if (m_endPort)
        m_endPort->connections().remove(m_endPort->connections().indexOf(this));

    Q_FOREACH (QNodeViewConnectionSplit* split, m_splits)
        delete split;
}

void QNodeViewConnection::setStartPosition(const QPointF& position)
{
    m_startPosition = position;
}

void QNodeViewConnection::setEndPosition(const QPointF& position)
{
    m_endPosition = position;
}

void QNodeViewConnection::setStartPort(QNodeViewPort* port)
{
    m_startPort = port;
    m_startPort->connections().append(this);
}

void QNodeViewConnection::setEndPort(QNodeViewPort* port)
{
    m_endPort = port;
    m_endPort->connections().append(this);
}

void QNodeViewConnection::updatePosition()
{
    m_startPosition = m_startPort->scenePos();
    m_endPosition   = m_endPort->scenePos();
}

void QNodeViewConnection::updatePath()
{
    QPainterPath path;

    QVector<QPointF> curvePoints;
    curvePoints.append(m_startPosition);

    Q_FOREACH (QNodeViewConnectionSplit* split, m_splits)
        curvePoints.append(split->splitPosition());

    curvePoints.append(m_endPosition);

    for (qint32 index = 0; index < curvePoints.size() - 1; ++index)
    {
        const QPointF& startPosition = curvePoints[index + 0];
        const QPointF& endPosition = curvePoints[index + 1];

        const qreal deltaX = endPosition.x() - startPosition.x();
        const qreal deltaY = endPosition.y() - startPosition.y();

        QPointF anchor1(startPosition.x() + deltaX * 0.25, startPosition.y() + deltaY * 0.1);
        QPointF anchor2(startPosition.x() + deltaX * 0.75, startPosition.y() + deltaY * 0.9);

        path.moveTo(startPosition);
        path.cubicTo(anchor1, anchor2, endPosition);
    }

    setPath(path);
}

void QNodeViewConnection::updateSplits()
{
    Q_FOREACH (QNodeViewConnectionSplit* split, m_splits)
    {
        split->updatePath();
    }
}

QNodeViewPort* QNodeViewConnection::startPort() const
{
    return m_startPort;
}

QNodeViewPort* QNodeViewConnection::endPort() const
{
    return m_endPort;
}

void QNodeViewConnection::save(QDataStream& stream)
{
    stream << reinterpret_cast<quint64>(m_startPort);
    stream << reinterpret_cast<quint64>(m_endPort);
}

void QNodeViewConnection::load(QDataStream& stream, const QMap<quint64, QNodeViewPort*>& portMap)
{
    quint64 startPortIndex;
    quint64 endPortIndex;
    stream >> startPortIndex;
    stream >> endPortIndex;

    setStartPort(portMap[startPortIndex]);
    setEndPort(portMap[endPortIndex]);

    updatePosition();
	updatePath();
}
