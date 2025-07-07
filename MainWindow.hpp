#pragma once

#include "Document.hpp"
#include <QMainWindow>
#include <QGraphicsScene>





// fwd:
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE





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

	void gvMousePressed(QPointF aScenePos, Qt::MouseButton aButton);
	void gvMouseReleased(QPointF aScenePos, Qt::MouseButton aButton);

	void gvMouseReleasedSelectObject(QPointF aScenePos);
	void gvMouseReleasedAddFixedPoint(QPointF aScenePos);
	void gvMouseReleasedAddSpring(QPointF aScenePos);
	void gvMouseReleasedRemoveObject(QPointF aScenePos);

	/** Sets the current tool, updates the actions. */
	void setCurrentTool(CurrentTool aNewTool);

	/** Updates the mGraphicsScene from the current document. */
	void updateScene();

	/** Returns {true, ptIdx} when the specified query point is close enough to (the closest) ptIdx to snap.
	Returns {false, ?} if too far away. */
	std::pair<bool, size_t> snapToPoint(QPointF aQueryPt);
};
