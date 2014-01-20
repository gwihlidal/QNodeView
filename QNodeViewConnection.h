/*!
  @file    QNodeViewConnection.h

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
#include <QNodeViewCommon.h>

class QNodeViewPort;

class QNodeViewConnection : public QGraphicsPathItem
{
public:
    QNodeViewConnection(QGraphicsItem* parent = NULL);
    virtual ~QNodeViewConnection();

    void setStartPosition(const QPointF& position);
    void setEndPosition(const QPointF& position);

    void setStartPort(QNodeViewPort* port);
    void setEndPort(QNodeViewPort* port);

    void updatePosition();
	void updatePath();

    QNodeViewPort* startPort() const;
    QNodeViewPort* endPort() const;

public:
    void save(QDataStream& stream);
    void load(QDataStream&, const QMap<quint64, QNodeViewPort*>& portMap);

public:
    // QGraphicsItem
    int type() const { return QNodeViewType_Connection; }

private:
    QPointF m_startPosition;
    QPointF m_endPosition;

    QNodeViewPort* m_startPort;
    QNodeViewPort* m_endPort;
};
