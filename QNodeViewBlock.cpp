/*!
  @file    QNodeViewBlock.cpp

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

#include <QPen>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <QNodeViewBlock.h>
#include <QNodeViewPort.h>

QNodeViewBlock::QNodeViewBlock(QGraphicsItem* parent)
: QGraphicsPathItem(parent)
, m_width(100)
, m_height(5)
, m_horizontalMargin(20)
, m_verticalMargin(5)
{
    setCacheMode(DeviceCoordinateCache);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    QPainterPath path;
    path.addRoundedRect(-50, -15, 100, 30, 5, 5);
    setPath(path);
    setPen(QPen(QColor(30, 30, 30))); // GW-TODO: Expose to QStyle
    setBrush(QColor(50, 50, 50)); // GW-TODO: Expose to QStyle

    // GW-TODO:
    //     This causes a crash in the BSP intersect code deep within Qt5.
    //     Need to investigate further, but many issues are open it appears...
    // https://codereview.qt-project.org/#change,40931
    // https://bugreports.qt-project.org/browse/QTBUG-23205
    // https://bugreports.qt-project.org/browse/QTBUG-24792
    // https://bugreports.qt-project.org/browse/QTBUG-18021
    // https://bugreports.qt-project.org/browse/QTBUG-5090
#if 0
    m_dropShadow.setBlurRadius(16);
    m_dropShadow.setXOffset(0.0);
    m_dropShadow.setYOffset(5.0);

    setGraphicsEffect(&m_dropShadow);
#endif
}

QNodeViewBlock::~QNodeViewBlock()
{
}

QNodeViewPort* QNodeViewBlock::addPort(const QString& name, bool isOutput, qint32 flags, qint32 index)
{
    QNodeViewPort* port = new QNodeViewPort(this);
	port->setName(name);
	port->setIsOutput(isOutput);
    port->setBlock(this);
	port->setPortFlags(flags);
    port->setIndex(index);

    QFontMetrics fontMetrics(scene()->font());
    const qint32 width  = fontMetrics.width(name);
    const qint32 height = fontMetrics.height();

    if (width > m_width - m_horizontalMargin)
        m_width = width + m_horizontalMargin;

    m_height += height;

    QPainterPath path;
    path.addRoundedRect(-(m_width >> 1), -(m_height >> 1), m_width, m_height, 5, 5);
    setPath(path);

    qint32 y = -(m_height >> 1) + m_verticalMargin + port->radius();

    Q_FOREACH (QGraphicsItem* childItem, childItems())
    {
        if (childItem->type() != QNodeViewType_Port)
			continue;

        QNodeViewPort* childPort = static_cast<QNodeViewPort*>(childItem);

        if (childPort->isOutput())
            childPort->setPos((m_width >> 1) + childPort->radius(), y);
		else
            childPort->setPos(-(m_width >> 1) - childPort->radius(), y);

        y += height;
	}

	return port;
}

void QNodeViewBlock::addInputPort(const QString& name)
{
	addPort(name, false);
}

void QNodeViewBlock::addOutputPort(const QString& name)
{
	addPort(name, true);
}

void QNodeViewBlock::addInputPorts(const QStringList& names)
{
    Q_FOREACH (const QString& name, names)
        addInputPort(name);
}

void QNodeViewBlock::addOutputPorts(const QStringList& names)
{
    Q_FOREACH (const QString& name, names)
        addOutputPort(name);
}

void QNodeViewBlock::save(QDataStream& stream)
{
    stream << pos();

    qint32 count = 0;

    Q_FOREACH (QGraphicsItem* childItem, childItems())
	{
        if (childItem->type() != QNodeViewType_Port)
			continue;

        ++count;
	}

    stream << count;

    Q_FOREACH (QGraphicsItem* childItem, childItems())
	{
        if (childItem->type() != QNodeViewType_Port)
			continue;

        QNodeViewPort* port = static_cast<QNodeViewPort*>(childItem);
        stream << reinterpret_cast<quint64>(port);
        stream << port->portName();
        stream << port->isOutput();
        stream << port->portFlags();
	}
}

void QNodeViewBlock::load(QDataStream& stream, QMap<quint64, QNodeViewPort*>& portMap)
{
    QPointF position;
    stream >> position;
    setPos(position);

    qint32 count;
    stream >> count;

    for (qint32 iter = 0; iter < count; iter++)
	{
        quint64 index;
        stream >> index;

        QString name;
        stream >> name;

        bool output;
        stream >> output;

        qint32 flags;
        stream >> flags;

        portMap[index] = addPort(name, output, flags, index);
	}
}

void QNodeViewBlock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    // Only paint dirty regions for increased performance
    painter->setClipRect(option->exposedRect);

    if (isSelected())
    {
        painter->setPen(QPen(QColor(30, 30, 30))); // GW-TODO: Expose to QStyle
        painter->setBrush(QColor(100, 100, 100)); // GW-TODO: Expose to QStyle
    }
    else
    {
        painter->setPen(QPen(QColor(30, 30, 30))); // GW-TODO: Expose to QStyle
        painter->setBrush(QColor(80, 80, 80)); // GW-TODO: Expose to QStyle
	}

	painter->drawPath(path());
}

QNodeViewBlock* QNodeViewBlock::clone()
{
    QNodeViewBlock* block = new QNodeViewBlock(NULL);
    this->scene()->addItem(block);

    Q_FOREACH (QGraphicsItem* childPort, childItems())
	{
        if (childPort->type() == QNodeViewType_Port)
		{
            QNodeViewPort* clonePort = static_cast<QNodeViewPort*>(childPort);
            block->addPort(
                    clonePort->portName(),
                    clonePort->isOutput(),
                    clonePort->portFlags(),
                    clonePort->index());
		}
	}

    return block;
}

QVector<QNodeViewPort*> QNodeViewBlock::ports()
{
    QVector<QNodeViewPort*> result;

    Q_FOREACH (QGraphicsItem* childItem, childItems())
	{
        if (childItem->type() == QNodeViewType_Port)
            result.append(static_cast<QNodeViewPort*>(childItem));
	}

    return result;
}

QVariant QNodeViewBlock::itemChange(GraphicsItemChange change, const QVariant& value)
{
    Q_UNUSED(change);
	return value;
}
