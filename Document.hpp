#pragma once

#include "SpringNet.hpp"

#include <QObject>





/** Contains all the data for a single "project". */
class Document:
	public QObject
{
	Q_OBJECT

	SpringNet mSpringNet;
	QString mFileName;


public:

	Document();

	SpringNet & springNet() { return mSpringNet; }
	const SpringNet & springNet() const { return mSpringNet; }
	const QString & fileName() const { return mFileName; }

	void loadFromFile(const QString & aFileName);
	void loadFromIO(QIODevice * aIO);
	void saveToFile(const QString & aFileName);
	void saveToIO(QIODevice * aIO);
};
