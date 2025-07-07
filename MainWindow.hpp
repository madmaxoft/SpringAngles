#pragma once

#include "Document.hpp"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsLineItem>





// fwd:
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE





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

	void paint(
		QPainter * aPainter,
		const QStyleOptionGraphicsItem * aOption,
		QWidget * widget = nullptr
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
	GraphicsSpringItem * mNewSpringLine;


	/** Connects the actions to their slots in this form. */
	void connectActions();

	// Action slots:
	void fileNew();
	void fileOpen();
	void fileSave();
	void fileSaveAs();

	void toolSelectObject();
	void toolAddFixedPoint();
	void toolAddSpring();
	void toolRemoveObject();

	void zoomIn();
	void zoomOut();
	void zoomAll();

	void gvMouseMoved(QPointF aScenePos);
	void gvMousePressed(QPointF aScenePos, Qt::MouseButton aButton);
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

	/** Returns {true, ptIdx} when the specified query point is close enough to (the closest) ptIdx to snap.
	Returns {false, ?} if too far away. */
	std::pair<bool, size_t> snapToPoint(QPointF aQueryPt);
};
