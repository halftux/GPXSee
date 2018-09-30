#include <QFont>
#include <QPainter>
#include "config.h"
#include "infoitem.h"

#define PADDING     10

InfoItem::InfoItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	_font.setPixelSize(FONT_SIZE);
	_font.setFamily(FONT_FAMILY);

#ifndef Q_OS_MAC
	setCacheMode(QGraphicsItem::DeviceCoordinateCache);
#endif // Q_OS_MAC
}

void InfoItem::updateBoundingRect()
{
	QFontMetrics fm(_font);
	qreal width = 0;

	for (QList<KV>::const_iterator i = _list.constBegin();
	  i != _list.constEnd(); i++) {
		width += fm.width(i->key + ": ");
		width += fm.width(i->value) + ((i == _list.constEnd() - 1)
		  ? 0 : PADDING);
	}

	_boundingRect = QRectF(0, 0, width, _list.isEmpty() ? 0 : fm.height());
}

void InfoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	  QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QFontMetrics fm(_font);
	int width = 0;

	painter->setFont(_font);
	painter->setRenderHint(QPainter::Antialiasing, false);

	for (QList<KV>::const_iterator i = _list.constBegin();
	  i != _list.constEnd(); i++) {
#ifdef Q_WS_MAEMO_5
		painter->setPen(Qt::white);
#endif
		painter->drawText(width, fm.height() - fm.descent(), i->key + ": ");
		width += fm.width(i->key + ": ");
		painter->drawText(width, fm.height() - fm.descent(), i->value);
		width += fm.width(i->value) + ((i == _list.constEnd() - 1)
		  ? 0 : PADDING);
		if (i != _list.constEnd() - 1) {
			painter->save();
			painter->setPen(Qt::gray);
			painter->drawLine(width - PADDING/2, fm.descent(),
			  width - PADDING/2, fm.height() - fm.descent());
			painter->restore();
		}
	}

/*
	painter->setPen(Qt::red);
	painter->drawRect(boundingRect());
*/
}

void InfoItem::insert(const QString &key, const QString &value)
{
	KV kv(key, value);
	int i;

	prepareGeometryChange();

	if ((i = _list.indexOf(kv)) < 0)
		_list.append(kv);
	else
		_list[i] = kv;

	updateBoundingRect();
	update();
}

void InfoItem::clear()
{
	prepareGeometryChange();
	_list.clear();
	updateBoundingRect();
}
