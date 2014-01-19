/*!
  @file    QNodeViewBlock.h

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

#include <QGraphicsPathItem>
#include <QGraphicsDropShadowEffect>
#include <QNodeViewCommon.h>

class QNodeViewPort;

class QNodeViewBlock : public QGraphicsPathItem
{
public:
    QNodeViewBlock(QGraphicsItem* parent = nullptr);

    QNodeViewPort* addPort(const QString& name, bool isOutput, qint32 flags = 0, qint32 index = 0);

    void addInputPort(const QString& name);
    void addOutputPort(const QString& name);
    void addInputPorts(const QStringList& names);
    void addOutputPorts(const QStringList& names);

public:
    void save(QDataStream& stream);
    void load(QDataStream&, QMap<quint64, QNodeViewPort*>& portMap);

public:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

public:
    QNodeViewBlock* clone();
    QVector<QNodeViewPort*> ports();

    // QGraphicsItem
    int type() const { return QNodeViewType_Block; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    QGraphicsDropShadowEffect m_dropShadow;
    qint32 m_width;
    qint32 m_height;
    qint32 m_horizontalMargin;
    qint32 m_verticalMargin;
};
