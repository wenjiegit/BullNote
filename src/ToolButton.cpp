﻿/*
The MIT License (MIT)

Copyright (c) wenjie.zhao

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QPainter>
#include <QMouseEvent>

#include "ToolButton.h"

#define DEFAULT_FONT_COLOR "#646464"
#define SELECTED_FONT_COLOR "#fe4f00"

ToolButton::ToolButton(QWidget *parent)
    : QToolButton(parent)
    , alpha(0)
    , status(STATUS_NORMAL)
    , m_xOffset(0)
{
    setStyleSheet("QToolButton{border:0px;}");

    m_fontColor = DEFAULT_FONT_COLOR;

    timeLine.setDuration(200);
    connect(&timeLine, SIGNAL(frameChanged(int)), this, SLOT(frameChanged(int)));
    connect(&timeLine, SIGNAL(finished()), this, SLOT(finished()));
    timeLine.setFrameRange(0, 10);
}

ToolButton::~ToolButton()
{
    timeLine.stop();
}

void ToolButton::setPixmap(const QPixmap &pixmap)
{
    int nPixWidth = pixmap.width();
    int nPixHeight = pixmap.height();

    m_normal = pixmap.copy( 0, 0, nPixWidth/4, nPixHeight);
    m_hover = pixmap.copy( nPixWidth/4 , 0, nPixWidth/4, nPixHeight);
    m_pressed = pixmap.copy( nPixWidth/4 * 2, 0, nPixWidth/4, nPixHeight);
    m_disabled = pixmap.copy( nPixWidth/4 * 3, 0, nPixWidth/4, nPixHeight);

    if(!m_normal.isNull())
    {
       setFixedSize((nPixWidth-4)/4+4, nPixHeight+6);
       setIcon(m_normal);
       setIconSize(QSize(nPixWidth/4, nPixHeight));
    }

    resultImage = m_hover.toImage();
    sourceImage = m_hover.toImage();

    setIcon(m_normal);
}

void ToolButton::setButtonStatus(int status)
{
    timeLine.stop();

    QPixmap bg;
    switch(status)
    {
    case STATUS_NORMAL:
        bg = m_normal;
        break;
    case STATUS_HOVE:
        bg = m_hover;
        break;
    case STATUS_PRESS:
        bg = m_pressed;
        break;
    case STATUS_DISABLED:
        bg = m_disabled;
        break;
    }

    setIcon(bg);
}

void ToolButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);
    QPainter p(this);

    QPixmap bg(width(), height());
    bg.fill(QColor(255, 255, 255, 0));
    p.drawPixmap(rect(), bg);

    if(!text().isEmpty())
    {
        QRect tr = rect();
        p.setPen(m_fontColor);
        p.setBrush(m_fontColor);
        if (m_xOffset == 0) {
            p.drawText(tr, Qt::AlignHCenter | Qt::AlignVCenter, text());
        } else {
            tr.moveTop(m_xOffset);
            p.drawText(tr, Qt::AlignHCenter | Qt::AlignLeft, text());
        }
    }
}

void ToolButton::enterEvent(QEvent *event)
{
    status = STATUS_HOVE;
    if(isEnabled())
    {
        setCursor(QCursor(Qt::PointingHandCursor));
        setIcon(m_hover);
        timeLine.stop();
        timeLine.setDirection(QTimeLine::Forward);
        //timeLine.start();
    }
    else
        setIcon(m_disabled);

    QToolButton::enterEvent(event);
}

void ToolButton::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
}

void ToolButton::frameChanged(int frame)
{
    Q_UNUSED(frame);
    setIcon(m_hover);
}

void ToolButton::finished()
{
    int direction = timeLine.direction();
    switch(direction)
    {
    case QTimeLine::Backward:
        setIcon(m_normal);
    }
}

void ToolButton::leaveEvent(QEvent *event)
{
    status = STATUS_NORMAL;
    setCursor(QCursor(Qt::OpenHandCursor));
    if(isEnabled())
    {
        timeLine.stop();
        timeLine.setDirection(QTimeLine::Backward);
        //timeLine.start();
        setIcon(m_normal);
    }
    else
        setIcon(m_disabled);
    QToolButton::leaveEvent(event);
}

void ToolButton::mousePressEvent(QMouseEvent *event)
{
    timeLine.stop();
    status = STATUS_PRESS;
    if(isEnabled())
        setIcon(m_pressed);
    else
        setIcon(m_disabled);
    QToolButton::mousePressEvent(event);
}

void ToolButton::mouseReleaseEvent(QMouseEvent *event)
{
    status = STATUS_HOVE;
    if(isEnabled())
        setIcon(m_hover);
    else
        setIcon(m_disabled);
    QToolButton::mouseReleaseEvent(event);
}

void ToolButton::setOffset(const int xOffset)
{
    m_xOffset = xOffset;
}

void ToolButton::setEnabled(bool enable)
{
    QToolButton::setEnabled(enable);
    if (isEnabled()) {
        if (status == STATUS_NORMAL) {
            setIcon(m_normal);
        } else if (status == STATUS_PRESS) {
            setIcon(m_pressed);
        } else if (status == STATUS_HOVE){
            setIcon(m_hover);
        } else {
            return;
        }
    } else {
        setIcon(m_disabled);
    }
}
