/*!
  @file    QNodeViewPort.h

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

class QNodeViewBlock;
class QNodeViewConnection;

class QNodeViewPort : public QGraphicsPathItem
{
public:
    QNodeViewPort(QGraphicsItem* parent = NULL);
    virtual ~QNodeViewPort();

    void setBlock(QNodeViewBlock* block);
    void setName(const QString& name);
    void setIsOutput(bool isOutput);
    void setPortFlags(qint32 index);
    void setIndex(quint64);

    bool isConnected(QNodeViewPort*);
    bool isOutput();

    qint32 radius();
    QVector<QNodeViewConnection*>& connections();
    QNodeViewBlock* block() const;
    quint64 index();

    const QString& portName() const { return m_name; }
	int portFlags() const { return m_portFlags; }

public:
    // QGraphicsItem
    int type() const { return QNodeViewType_Port; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    QVector<QNodeViewConnection*> m_connections;
    QString m_name;
    QNodeViewBlock* m_block;
    QGraphicsTextItem* m_label;

    quint64 m_index;
    qint32 m_radius;
    qint32 m_margin;
    qint32 m_portFlags;

    bool m_isOutput;
};
