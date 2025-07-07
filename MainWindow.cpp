#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "PointCoordsDlg.hpp"
#include "SpringParamsDlg.hpp"

#include <QActionGroup>





namespace {
/** The max distance (in pixels) to snap to points. */
static const double POINT_SNAP_THRESHOLD = 10;
}





MainWindow::MainWindow(QWidget * aParent):
	Super(aParent),
	mUI(new Ui::MainWindow),
	mGraphicsScene(new QGraphicsScene),
	mDocument(new Document)
{
	mUI->setupUi(this);
	mUI->gvMain->setScene(mGraphicsScene.get());

	connectActions();
	connect(mUI->gvMain, &CadGraphicsView::mouseReleased, this, &MainWindow::gvMouseReleased);
	connect(mUI->gvMain, &CadGraphicsView::mousePressed,  this, &MainWindow::gvMousePressed);

	setCurrentTool(CurrentTool::AddFixedPoint);
}





MainWindow::~MainWindow()
{
	// Nothing explicit needed yet
}





void MainWindow::connectActions()
{
	// File:
	connect(mUI->actFileNew,    &QAction::triggered, this, &MainWindow::fileNew);
	connect(mUI->actFileOpen,   &QAction::triggered, this, &MainWindow::fileOpen);
	connect(mUI->actFileSave,   &QAction::triggered, this, &MainWindow::fileSave);
	connect(mUI->actFileSaveAs, &QAction::triggered, this, &MainWindow::fileSaveAs);
	connect(mUI->actFileExit,   &QAction::triggered, this, &MainWindow::close);

	// Tool:
	connect(mUI->actToolSelectObject,  &QAction::triggered, this, &MainWindow::toolSelectObject);
	connect(mUI->actToolAddFixedPoint, &QAction::triggered, this, &MainWindow::toolAddFixedPoint);
	connect(mUI->actToolAddSpring,     &QAction::triggered, this, &MainWindow::toolAddSpring);
	connect(mUI->actToolRemoveObject,  &QAction::triggered, this, &MainWindow::toolRemoveObject);

	// Zoom:
	connect(mUI->actZoomIn,  &QAction::triggered, this, &MainWindow::zoomIn);
	connect(mUI->actZoomOut, &QAction::triggered, this, &MainWindow::zoomOut);
	connect(mUI->actZoomAll, &QAction::triggered, this, &MainWindow::zoomAll);
}





void MainWindow::fileNew()
{
	mDocument = std::make_unique<Document>();
	updateScene();
}





void MainWindow::fileOpen()
{
	// TODO
}





void MainWindow::fileSave()
{
	// TODO
}





void MainWindow::fileSaveAs()
{
	// TODO
}





void MainWindow::toolSelectObject()
{
	setCurrentTool(CurrentTool::SelectObject);
}





void MainWindow::toolAddFixedPoint()
{
	setCurrentTool(CurrentTool::AddFixedPoint);
}





void MainWindow::toolAddSpring()
{
	setCurrentTool(CurrentTool::AddSpring);
}





void MainWindow::toolRemoveObject()
{
	setCurrentTool(CurrentTool::RemoveObject);
}





void MainWindow::zoomIn()
{
	// TODO
}





void MainWindow::zoomOut()
{
	// TODO
}





void MainWindow::zoomAll()
{
	// TODO
}





void MainWindow::gvMousePressed(QPointF aScenePos, Qt::MouseButton aButton)
{
	if (aButton != Qt::LeftButton)
	{
		return;
	}
	mMouseDownPos = aScenePos;
}





void MainWindow::gvMouseReleased(QPointF aScenePos, Qt::MouseButton aButton)
{
	if (aButton != Qt::LeftButton)
	{
		return;
	}
	switch (mCurrentTool)
	{
		case CurrentTool::SelectObject:  return gvMouseReleasedSelectObject (aScenePos);
		case CurrentTool::AddFixedPoint: return gvMouseReleasedAddFixedPoint(aScenePos);
		case CurrentTool::AddSpring:     return gvMouseReleasedAddSpring    (aScenePos);
		case CurrentTool::RemoveObject:  return gvMouseReleasedRemoveObject (aScenePos);
	}
}





void MainWindow::gvMouseReleasedSelectObject(QPointF aScenePos)
{
	// TODO
}





void MainWindow::gvMouseReleasedAddFixedPoint(QPointF aScenePos)
{
	auto coords = PointCoordsDlg::ask(this, aScenePos);
	if (!coords)
	{
		return;
	}
	mDocument->springNet().addPoint(coords->x(), coords->y(), true);
	updateScene();
}





void MainWindow::gvMouseReleasedAddSpring(QPointF aScenePos)
{
	if (mDocument->springNet().numPoints() == 0)
	{
		return;
	}
	auto startPointIdx = mDocument->springNet().nearestPointIdx(mMouseDownPos.x(), mMouseDownPos.y());
	auto startPoint = mDocument->springNet().point(startPointIdx);
	auto snap = snapToPoint(aScenePos);
	size_t endPointIdx;
	std::optional<SpringParams> springParams;
	if (snap.first)
	{
		if (snap.second == startPointIdx)
		{
			// Trying to connect the spring to the same point on both ends
			return;
		}
		auto pt2 = mDocument->springNet().point(snap.second);
		auto diffX = startPoint.x() - pt2.x();
		auto diffY = startPoint.y() - pt2.y();
		auto len = std::sqrt(diffX * diffX + diffY * diffY);
		springParams = SpringParamsDlg::ask(this, true, len);
		endPointIdx = snap.second;
	}
	else
	{
		// Not snapping to a point
		auto diffX = startPoint.x() - aScenePos.x();
		auto diffY = startPoint.y() - aScenePos.y();
		auto len = std::sqrt(diffX * diffX + diffY * diffY);
		springParams = SpringParamsDlg::ask(this, false, len);
		if (!springParams)
		{
			return;
		}
		// Add a new point for the spring:
		auto x = startPoint.x() - diffX * springParams->mLength / len;
		auto y = startPoint.y() - diffY * springParams->mLength / len;
		mDocument->springNet().addPoint(x, y, false);
		endPointIdx = mDocument->springNet().numPoints() - 1;
	}

	// Add the spring:
	if (!springParams)
	{
		return;
	}
	mDocument->springNet().addSpring(springParams->mLength, springParams->mForce, startPointIdx, endPointIdx);

	updateScene();
}





void MainWindow::gvMouseReleasedRemoveObject(QPointF aScenePos)
{
	// TODO
}





void MainWindow::setCurrentTool(CurrentTool aNewTool)
{
	mCurrentTool = aNewTool;
	mUI->actToolSelectObject->setChecked(aNewTool == CurrentTool::SelectObject);
	mUI->actToolAddFixedPoint->setChecked(aNewTool == CurrentTool::AddFixedPoint);
	mUI->actToolAddSpring->setChecked(aNewTool == CurrentTool::AddSpring);
	mUI->actToolRemoveObject->setChecked(aNewTool == CurrentTool::RemoveObject);
}





void MainWindow::updateScene()
{
	mGraphicsScene->clear();
	for (const auto & p: mDocument->springNet().points())
	{
		mGraphicsScene->addEllipse(p.x(), p.y(), 3, 3);
	}
	for (const auto & s: mDocument->springNet().springs())
	{
		auto x1 = s.point1().x();
		auto y1 = s.point1().y();
		auto x2 = s.point2().x();
		auto y2 = s.point2().y();
		mGraphicsScene->addLine(x1, y1, x2, y2);
	}
}





std::pair<bool, size_t> MainWindow::snapToPoint(QPointF aQueryPt)
{
	auto nearestPtIdx = mDocument->springNet().nearestPointIdx(aQueryPt.x(), aQueryPt.y());
	const auto & pt = mDocument->springNet().point(nearestPtIdx);
	auto ptScreenCoords = mUI->gvMain->mapFromScene(pt.x(), pt.y());
	auto queryScreenCoords = mUI->gvMain->mapFromScene(aQueryPt.x(), aQueryPt.y());
	auto diff = queryScreenCoords - ptScreenCoords;
	auto len = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
	return {(len < POINT_SNAP_THRESHOLD), nearestPtIdx};
}






