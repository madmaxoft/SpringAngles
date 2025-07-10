#include "MainWindow.hpp"

#include <QActionGroup>
#include <QGraphicsLineItem>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_MainWindow.h"
#include "PointCoordsDlg.hpp"
#include "SpringParamsDlg.hpp"





namespace {
/** The max distance (in pixels) to snap to points. */
static const double POINT_SNAP_THRESHOLD = 10;
}  // anonymous namespace





//////////////////////////////////////////////////////////////////////////////
// GraphicsPointItem:

void GraphicsPointItem::paint(
	QPainter * aPainter,
	const QStyleOptionGraphicsItem * aOption,
	QWidget * aWidget
)
{
	Q_UNUSED(aOption);
	Q_UNUSED(aWidget);

	if (isSelected())
	{
		auto p = pen();
		p.setWidth(p.width() + 4);
		p.setColor(QColor::fromRgb(0, 0xff, 0xff));
		aPainter->setPen(p);
		aPainter->drawRect(rect());
	}
	aPainter->setPen(pen());
	aPainter->drawRect(rect());
}





//////////////////////////////////////////////////////////////////////////////
// GraphicsSpringItem:

void GraphicsSpringItem::paint(
	QPainter * aPainter,
	const QStyleOptionGraphicsItem * aOption,
	QWidget * aWidget
)
{
	Q_UNUSED(aOption);
	Q_UNUSED(aWidget);

	auto currentLength = line().length();
	auto txt = QString("%1 / %2").arg(currentLength).arg(mIdealLength);
	if (isSelected())
	{
		auto p = pen();
		p.setWidth(p.width() + 3);
		p.setColor(QColor::fromRgb(0, 0xff, 0xff));
		aPainter->setPen(p);
		aPainter->drawLine(line());
	}
	aPainter->setPen(pen());
	aPainter->drawLine(line());
	aPainter->drawText(line().center(), txt);
}






///////////////////////////////////////////////////////////////////////////////
// MainWindow:

MainWindow::MainWindow(QWidget * aParent):
	Super(aParent),
	mUI(new Ui::MainWindow),
	mGraphicsScene(new QGraphicsScene),
	mDocument(new Document)
{
	mUI->setupUi(this);
	mUI->gvMain->setScene(mGraphicsScene.get());

	connectActions();
	connect(mUI->gvMain, &CadGraphicsView::mouseReleased,   this, &MainWindow::gvMouseReleased);
	connect(mUI->gvMain, &CadGraphicsView::mousePressed,    this, &MainWindow::gvMousePressed);
	connect(mUI->gvMain, &CadGraphicsView::mouseMoved,      this, &MainWindow::gvMouseMoved);
	connect(mUI->gvMain, &CadGraphicsView::mouseDblClicked, this, &MainWindow::gvMouseDblClicked);

	setCurrentTool(CurrentTool::SelectObject);
	updateScene();
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

	connect(mUI->actAdjust, &QAction::triggered, this, &MainWindow::doAdjust);
}





void MainWindow::fileNew()
{
	mDocument = std::make_unique<Document>();
	updateScene();
}





void MainWindow::fileOpen()
{
	auto fnam = QFileDialog::getOpenFileName(
		this,
		tr("SpringAngles: Open file"),
		{},
		tr("SpringAngles documents (*.SpringAngles)")
	);
	fileOpenByName(fnam);
}





void MainWindow::fileOpenByName(const QString & aFileName)
{
	auto doc = std::make_unique<Document>();
	try
	{
		doc->loadFromFile(aFileName);
	}
	catch (const std::exception & exc)
	{
		QMessageBox::warning(
			this,
			tr("SpringAngles: Cannot open file"),
			tr("Cannot open file %1: %2").arg(aFileName, QString::fromUtf8(exc.what()))
		);
		return;
	}
	mDocument = std::move(doc);
	updateScene();
}





void MainWindow::fileSave()
{
	if (mDocument->fileName().isEmpty())
	{
		return fileSaveAs();
	}
	try
	{
		mDocument->saveToFile(mDocument->fileName());
	}
	catch (const std::exception & exc)
	{
		QMessageBox::warning(
			this,
			tr("SpringAngles: Cannot save file"),
			tr("Cannot save file %1: %2").arg(mDocument->fileName(), QString::fromUtf8(exc.what()))
		);
		return;
	}
}





void MainWindow::fileSaveAs()
{
	auto fnam = QFileDialog::getSaveFileName(
		this,
		tr("SpringAngles: Save file"),
		{},
		tr("SpringAngles documents (*.SpringAngles)")
	);
	if (fnam.isEmpty())
	{
		return;
	}
	mDocument->setFileName(fnam);
	fileSave();
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
	auto all = mGraphicsScene->itemsBoundingRect();
	mUI->gvMain->zoomTo(all);
	mUI->gvMain->centerOn(all.center());
}





void MainWindow::gvMouseMoved(QPointF aScenePos)
{
	switch (mCurrentTool)
	{
		case CurrentTool::SelectObject:
		{
			if (QApplication::mouseButtons() & Qt::LeftButton)
			{
				switch (mCurrentObject.first)
				{
					case SpringNet::ObjectType::Point:
					{
						mDocument->springNet().point(mCurrentObject.second).set(aScenePos);
						updateScene();
						break;
					}
					default: break;
				}
			}
			else
			{
				selectNearestObject(aScenePos);
			}
			break;
		}
		case CurrentTool::AddSpring:
		{
			if (QApplication::mouseButtons() & Qt::LeftButton)
			{
				auto snap = mDocument->springNet().snapToPoint(aScenePos, snapThresholdSquared());
				if (snap.first)
				{
					auto snapPt = mDocument->springNet().point(snap.second);
					aScenePos = QPointF(snapPt.x(), snapPt.y());
				}
				mNewSpringLine->setLine(mMouseDownPos, aScenePos);
				auto length = mNewSpringLine->line().length();
				mNewSpringLine->setIdealLength(length);
			}
			break;
		}
		case CurrentTool::RemoveObject:
		{
			selectNearestObject(aScenePos);
			break;
		}
		default: break;
	}
}





void MainWindow::gvMousePressed(QPointF aScenePos, Qt::MouseButton aButton)
{
	if (aButton != Qt::LeftButton)
	{
		return;
	}
	mMouseDownPos = aScenePos;
	switch (mCurrentTool)
	{
		case CurrentTool::SelectObject:
		{
			mCurrentObject = mDocument->springNet().nearestObject(mMouseDownPos, snapThresholdSquared());
			break;
		}
		case CurrentTool::AddSpring:
		{
			auto startPtIdx = mDocument->springNet().nearestPointIdx(aScenePos);
			auto startPt = mDocument->springNet().point(startPtIdx);
			mMouseDownPos = QPointF(startPt.x(), startPt.y());
			mNewSpringLine->setLine(startPt, startPt);
			mNewSpringLine->show();
			break;
		}
		default: break;
	}
}





void MainWindow::gvMouseDblClicked(QPointF aScenePos, Qt::MouseButton aButton)
{
	if (aButton != Qt::LeftButton)
	{
		return;
	}
	switch (mCurrentTool)
	{
		case CurrentTool::SelectObject:
		{
			auto nearestObj = mDocument->springNet().nearestObject(aScenePos, snapThresholdSquared());
			switch (nearestObj.first)
			{
				case SpringNet::ObjectType::None: break;
				case SpringNet::ObjectType::Point:
				{
					auto newCoords = PointCoordsDlg::ask(this, mDocument->springNet().point(nearestObj.second));
					if (newCoords != std::nullopt)
					{
						mDocument->springNet().point(nearestObj.second).set(newCoords->x(), newCoords->y());
						updateScene();
					}
					break;
				}
				case SpringNet::ObjectType::Spring:
				{
					auto & spring = mDocument->springNet().spring(nearestObj.second);
					auto newParams = SpringParamsDlg::ask(this, spring.idealLength(), spring.force());
					if (newParams != std::nullopt)
					{
						spring.setIdealLength(newParams->mIdealLength);
						spring.setForce(newParams->mForce);
						updateScene();
					}
					break;
				}
			}
			break;
		}
		default: break;
	}
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
	gvMouseMoved(aScenePos);
	mCurrentObject = {SpringNet::ObjectType::None, 0};
}





void MainWindow::gvMouseReleasedAddFixedPoint(QPointF aScenePos)
{
	auto coords = PointCoordsDlg::ask(this, aScenePos);
	if (!coords)
	{
		return;
	}
	mDocument->springNet().addPoint(*coords, true);
	updateScene();
}





void MainWindow::gvMouseReleasedAddSpring(QPointF aScenePos)
{
	if (mDocument->springNet().numPoints() == 0)
	{
		return;
	}
	auto startPointIdx = mDocument->springNet().nearestPointIdx(mMouseDownPos);
	auto startPoint = mDocument->springNet().point(startPointIdx);
	auto snap = mDocument->springNet().snapToPoint(aScenePos, snapThresholdSquared());
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
		springParams = SpringParamsDlg::ask(this, len, 1);
		endPointIdx = snap.second;
	}
	else
	{
		// Not snapping to a point
		auto diffX = startPoint.x() - aScenePos.x();
		auto diffY = startPoint.y() - aScenePos.y();
		auto len = std::sqrt(diffX * diffX + diffY * diffY);
		springParams = SpringParamsDlg::ask(this, len, 1);
		if (!springParams)
		{
			return;
		}
		// Add a new point for the spring:
		auto x = startPoint.x() - diffX * springParams->mIdealLength / len;
		auto y = startPoint.y() - diffY * springParams->mIdealLength / len;
		mDocument->springNet().addPoint({x, y}, false);
		endPointIdx = mDocument->springNet().numPoints() - 1;
	}

	// Add the spring:
	if (!springParams)
	{
		return;
	}
	mDocument->springNet().addSpring(springParams->mIdealLength, springParams->mForce, startPointIdx, endPointIdx);

	updateScene();
}





void MainWindow::gvMouseReleasedRemoveObject(QPointF aScenePos)
{
	auto nearestObj = mDocument->springNet().nearestObject(aScenePos, snapThresholdSquared());
	switch (nearestObj.first)
	{
		case SpringNet::ObjectType::None: return;
		case SpringNet::ObjectType::Point:
		{
			mDocument->springNet().removePoint(nearestObj.second);
			break;
		}
		case SpringNet::ObjectType::Spring:
		{
			mDocument->springNet().removeSpring(nearestObj.second);
			break;
		}
	}
	updateScene();
}





void MainWindow::doAdjust()
{
	mDocument->springNet().adjust();
	updateScene();
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
	mItemsForPoints.clear();
	mItemsForSprings.clear();
	for (const auto & p: mDocument->springNet().points())
	{
		auto pt = new GraphicsPointItem(*p);
		mGraphicsScene->addItem(pt);
		pt->setFlag(QGraphicsItem::ItemIsSelectable);
		mItemsForPoints.push_back(pt);
	}
	for (const auto & s: mDocument->springNet().springs())
	{
		auto x1 = s->point1().x();
		auto y1 = s->point1().y();
		auto x2 = s->point2().x();
		auto y2 = s->point2().y();
		auto line = new GraphicsSpringItem(x1, y1, x2, y2, s->idealLength());
		mGraphicsScene->addItem(line);
		line->setFlag(QGraphicsItem::ItemIsSelectable);
		mItemsForSprings.push_back(line);
	}
	mNewSpringLine = new GraphicsSpringItem(0, 0, 0, 0, 0);
	mGraphicsScene->addItem(mNewSpringLine);

	mNewSpringLine->hide();
}





double MainWindow::scaleThreshold(double aThreshold) const
{
	return aThreshold * (mUI->gvMain->transform().m22() + mUI->gvMain->transform().m11()) / 2;
}





double MainWindow::snapThresholdSquared() const
{
	auto thr = scaleThreshold(POINT_SNAP_THRESHOLD);
	return thr * thr;
}





QGraphicsItem * MainWindow::itemForPoint(size_t aPointIdx)
{
	if (aPointIdx >= mDocument->springNet().numPoints())
	{
		return nullptr;
	}
	return mItemsForPoints[aPointIdx];
}





QGraphicsItem * MainWindow::itemForSpring(size_t aSpringIdx)
{
	if (aSpringIdx >= mDocument->springNet().numSprings())
	{
		return nullptr;
	}
	return mItemsForSprings[aSpringIdx];
}





QGraphicsItem * MainWindow::itemForObject(std::pair<SpringNet::ObjectType, size_t> aObjectDef)
{
	switch (aObjectDef.first)
	{
		case SpringNet::ObjectType::None:   return nullptr;
		case SpringNet::ObjectType::Point:  return itemForPoint(aObjectDef.second);
		case SpringNet::ObjectType::Spring: return itemForSpring(aObjectDef.second);
	}
	return nullptr;
}





void MainWindow::selectNearestObject(QPointF aScenePos)
{
	auto nearest = mDocument->springNet().nearestObject(aScenePos, snapThresholdSquared());
	auto item = itemForObject(nearest);
	if (item != nullptr)
	{
		mGraphicsScene->clearSelection();
		item->setSelected(true);
	}
}
