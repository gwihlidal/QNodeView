/*!
  @file    QNodeViewCanvas.cpp

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

#include <QNodeViewCanvas.h>

QNodeViewCanvas::QNodeViewCanvas(QGraphicsScene* scene, QWidget* parent)
: QGraphicsView(scene, parent)
{
}

QNodeViewCanvas::~QNodeViewCanvas()
{
}

void QNodeViewCanvas::contextMenuEvent(QContextMenuEvent* event)
{
    Q_UNUSED(event); // QGraphicsSceneContextMenuEvent
}

void QNodeViewCanvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    // GW-TODO: Expose this to QStyle
    painter->fillRect(rect, QBrush(QColor(50, 50, 50)));

    const qint32 gridInterval = 50;
    painter->setWorldMatrixEnabled(true);

    // GW-TODO: Expose this to QStyle
    QPen linePen(QColor(80, 80, 80), 1, Qt::DotLine, Qt::FlatCap, Qt::RoundJoin);
    painter->setPen(linePen);

    const qreal left = qint32(rect.left()) - (qint32(rect.left()) % gridInterval);
    const qreal top  = qint32(rect.top())  - (qint32(rect.top())  % gridInterval);

    QVarLengthArray<QLineF, 100> linesX;
    for (qreal x = left; x < rect.right(); x += gridInterval)
        linesX.append(QLineF(x, rect.top(), x, rect.bottom()));

    QVarLengthArray<QLineF, 100> linesY;
    for (qreal y = top; y < rect.bottom(); y += gridInterval)
        linesY.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(linesX.data(), linesX.size());
    painter->drawLines(linesY.data(), linesY.size());
}
