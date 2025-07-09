#pragma once

#include <QGraphicsView>





/** A QGraphicsView subclass that presents mouseclicks as signals, as well as other CAD-like improvements. */
class CadGraphicsView:
	public QGraphicsView
{
	Q_OBJECT

	using Super = QGraphicsView;


public:

	CadGraphicsView(QWidget * aParent);

	/** Zooms the view so that the specified rectangle is just about visible (touches the edges either
	horizontally, or vertically, depending on the aspect ratio). */
	void zoomTo(QRectF aRect);


Q_SIGNALS:

	/** Emitted when a mouse is moved within the space of this widget (and mouse-tracking is on). */
	void mouseMoved(QPointF aSceneMousePos);

	/** Emitted when a mouse button is pressed within the space of this widget. */
	void mousePressed(QPointF aSceneMousePos, Qt::MouseButton aButton);

	/** Emitted when a mouse button is released within the space of this widget. */
	void mouseReleased(QPointF aSceneMousePos, Qt::MouseButton aButton);

	/** Emitted when a mouse button is dbl-clicked within the space of this widget. */
	void mouseDblClicked(QPointF aSceneMousePos, Qt::MouseButton aButton);


protected:

	/** The value that mZoomSpeed is initialized to. */
	static constexpr qreal DEFAULT_ZOOM_SPEED = 1.2;

	/** The speed at which the zoom-by-wheel is performed.
	Should be a number slightly larger than 1, such as DEFAULT_ZOOM_SPEED (the default).
	Zooming multiplies or divides the current zoom by this value. */
	qreal mZoomSpeed;

	/** The last processed mouse position (in screen coords) while middle-mouse panning. */
	QPointF mMousePanLastPos;


	// QGraphicsView overrides:
	virtual void wheelEvent(QWheelEvent * aEvent) override;
	virtual QSize sizeHint() const override;
	virtual void mouseMoveEvent(QMouseEvent * aEvent) override;
	virtual void mousePressEvent(QMouseEvent * aEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent * aEvent) override;
	virtual void mouseDoubleClickEvent(QMouseEvent * aEvent) override;
};
