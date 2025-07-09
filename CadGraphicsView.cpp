#include "CadGraphicsView.hpp"

#include <QMouseEvent>






CadGraphicsView::CadGraphicsView(QWidget * aParentWidget):
	Super(aParentWidget),
	mZoomSpeed(DEFAULT_ZOOM_SPEED)
{
	// Disable scrollbars:
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// Disable zoom anchor (we're providing our own):
	setTransformationAnchor(QGraphicsView::NoAnchor);

	auto min = std::numeric_limits<qint32>::min();
	auto span = std::numeric_limits<quint32>::max();
	setSceneRect(QRectF(min, min, span, span));
}





void CadGraphicsView::zoomTo(QRectF aRect)
{
	static constexpr int EDGE = 2;  // How many pixels to leave free at each edge of the view
	if (!aRect.isValid())
	{
		return;
	}
	QTransform transform;
	auto factor = (viewport()->width() - 2 * EDGE) / aRect.width();
	auto factor2 = (viewport()->height() - 2 * EDGE) / aRect.height();
	factor = std::min(factor, factor2);
	transform.scale(factor, factor);
	transform.translate(EDGE / factor - aRect.left(), EDGE / factor + aRect.bottom());  // TODO: This only puts the whole image into the viewport, but doesn't center it
	setTransform(transform);
}





void CadGraphicsView::wheelEvent(QWheelEvent * aEvent)
{
	auto angle = aEvent->angleDelta().y() / 120.0;
	auto factor = std::pow(mZoomSpeed, angle);
	auto scale = transform().m22() * factor;
	auto dx = transform().dx() * factor;
	auto dy = transform().dy() * factor;
	auto mousePos = aEvent->position();
	// Adjust dx and dy by the current mouse position:
	dx += mousePos.x() * (1 - factor);
	dy += mousePos.y() * (1 - factor);
	setTransform(QTransform(scale, 0, 0, scale, dx, dy));
	aEvent->setAccepted(true);
	Q_EMIT mouseMoved(mapToScene(mousePos.toPoint()));
}





QSize CadGraphicsView::sizeHint() const
{
	// Disable size-by-content implemented in QGraphicsView
	return QAbstractScrollArea::sizeHint();
}





void CadGraphicsView::mouseMoveEvent(QMouseEvent * aEvent)
{
	if (aEvent->buttons() & Qt::MiddleButton)
	{
		// Pan the view:
		auto scale = transform().m22();
		auto dx = (aEvent->pos().x() - mMousePanLastPos.x()) / scale;
		auto dy = (aEvent->pos().y() - mMousePanLastPos.y()) / scale;
		setTransform(transform().translate(dx, dy));
		mMousePanLastPos = aEvent->pos();
	}
	Q_EMIT mouseMoved(mapToScene(aEvent->pos()));
}





void CadGraphicsView::mousePressEvent(QMouseEvent * aEvent)
{
	if (aEvent->button() == Qt::MiddleButton)
	{
		// Prepare for panning the view:
		mMousePanLastPos = aEvent->pos();
	}
	Q_EMIT mousePressed(mapToScene(aEvent->pos()), aEvent->button());
}





void CadGraphicsView::mouseReleaseEvent(QMouseEvent * aEvent)
{
	// Finish any pending drag-operation by processing the final mousemove:
	mouseMoveEvent(aEvent);
	Q_EMIT mouseReleased(mapToScene(aEvent->pos()), aEvent->button());
}





void CadGraphicsView::mouseDoubleClickEvent(QMouseEvent * aEvent)
{
	Q_EMIT mouseDblClicked(mapToScene(aEvent->pos()), aEvent->button());
	Super::mouseDoubleClickEvent(aEvent);
}
