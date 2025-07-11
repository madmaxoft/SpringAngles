#pragma once

#include "Document.hpp"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsLineItem>





// fwd:
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE





/** QGraphicsItem descendant that is used for drawing points. */
class GraphicsPointItem:
	public QGraphicsRectItem
{
	using Super = QGraphicsRectItem;

	/** A fixed point has a different graphics representation. */
	bool mIsFixed;


public:
	GraphicsPointItem(QPointF aPt, bool aIsFixed):
		Super(aPt.x() - 1, aPt.y() - 1, 3, 3),
		mIsFixed(aIsFixed)
	{
	}


	void paint(
		QPainter * aPainter,
		const QStyleOptionGraphicsItem * aOption,
		QWidget * aWidget = nullptr
	) override;

	/** Paints the raw shape using the current pen / brush. */
	void paintRaw(QPainter * aPainter);
};





/** QGraphicsItem descendant that is used for drawing springs. */
class GraphicsSpringItem:
	public QGraphicsLineItem
{
	/** The ideal length, to be displayed in the middle of the line. */
	double mIdealLength;

	using Super = QGraphicsLineItem;


public:
	explicit GraphicsSpringItem(double aX1, double aY1, double aX2, double aY2, double aLength):
		Super(aX1, aY1, aX2, aY2),
		mIdealLength(aLength)
	{
	}

	void setIdealLength(double aIdealLength) { mIdealLength = aIdealLength; update(); }

	void setLine(QPointF aPt1, QPointF aPt2)
	{
		Super::setLine(aPt1.x(), aPt1.y(), aPt2.x(), aPt2.y());
	}

	void paint(
		QPainter * aPainter,
		const QStyleOptionGraphicsItem * aOption,
		QWidget * aWidget = nullptr
	) override;
};





/** The main window of the application. */
class MainWindow:
	public QMainWindow
{
	Q_OBJECT

	using Super = QMainWindow;


	/** The tool currently selected by the user. */
	enum CurrentTool
	{
		SelectObject,
		AddFixedPoint,
		AddSpring,
		RemoveObject,
	} mCurrentTool;


public:

    MainWindow(QWidget * aParent = nullptr);
	~MainWindow();


private:

	/** The Qt-managed UI. */
	std::unique_ptr<Ui::MainWindow> mUI;

	/** The scene calculated from the document and displayed in gvMain. */
	std::unique_ptr<QGraphicsScene> mGraphicsScene;

	/** The document encompassing all the data. */
	std::unique_ptr<Document> mDocument;

	/** The scene pos in gvMain where the last mouse-down has occurred. */
	QPointF mMouseDownPos;

	/** The line used to show newly created spring. */
	GraphicsSpringItem * mNewSpringLine = nullptr;

	/** The QGraphicsItem-s representing the points, in the same order as the points. */
	std::vector<QGraphicsItem *> mItemsForPoints;

	/** The QGraphicsItem-s representing the springs, in the same order as the springs. */
	std::vector<QGraphicsItem *> mItemsForSprings;

	/** The object that is currently being manipulated. */
	std::pair<SpringNet::ObjectType, size_t> mCurrentObject = {SpringNet::ObjectType::None, 0};


	/** Connects the actions to their slots in this form. */
	void connectActions();


public:

	// Action slots:
	void fileNew();
	void fileOpen();
	void fileOpenByName(const QString & aFileName);
	void fileSave();
	void fileSaveAs();

	void toolSelectObject();
	void toolAddFixedPoint();
	void toolAddSpring();
	void toolRemoveObject();

	void zoomIn();
	void zoomOut();
	void zoomAll();


private:

	void gvMouseMoved(QPointF aScenePos);
	void gvMousePressed(QPointF aScenePos, Qt::MouseButton aButton);
	void gvMouseDblClicked(QPointF aScenePos, Qt::MouseButton aButton);
	void gvMouseReleased(QPointF aScenePos, Qt::MouseButton aButton);

	void gvMouseReleasedSelectObject(QPointF aScenePos);
	void gvMouseReleasedAddFixedPoint(QPointF aScenePos);
	void gvMouseReleasedAddSpring(QPointF aScenePos);
	void gvMouseReleasedRemoveObject(QPointF aScenePos);

	void doAdjust();

	/** Sets the current tool, updates the actions. */
	void setCurrentTool(CurrentTool aNewTool);

	/** Updates the mGraphicsScene from the current document. */
	void updateScene();

	/** Scales the specified threshold from screen coords to scene coords. */
	double scaleThreshold(double aThreshold) const;

	/** Returns the threshold to snap to objects in scene coord length, squared. */
	double snapThresholdSquared() const;

	/** Returns the graphics item representing the specified point visually, nullptr if not found. */
	QGraphicsItem * itemForPoint(size_t aPointIdx);

	/** Returns the graphics item representing the specified spring visually, nullptr if not found. */
	QGraphicsItem * itemForSpring(size_t aSpringIdx);

	/** Returns the graphics item representing the specified object visually.
	Returns nullptr if no such item. */
	QGraphicsItem * itemForObject(std::pair<SpringNet::ObjectType, size_t> aObjectDef);

	/** Selects the nearest object, deselecting any previous selection. */
	void selectNearestObject(QPointF aScenePos);
};
