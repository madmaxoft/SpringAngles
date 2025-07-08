#include "Document.hpp"

#include <QFile>





static const char gDocumentHeader[] = "SpringAngles document\n";





Document::Document()
{
}





void Document::loadFromFile(const QString & aFileName)
{
	QFile f(aFileName);
	if (!f.open(QIODevice::ReadOnly))
	{
		throw std::runtime_error("Cannot open file for reading.");
	}
	loadFromIO(&f);
	mFileName = aFileName;
}





void Document::loadFromIO(QIODevice * aIO)
{
	mSpringNet.clear();
	auto line = aIO->readLine();
	auto hdr = QByteArray(gDocumentHeader, sizeof(gDocumentHeader) - 1);
	if (line.compare(hdr) != 0)
	{
		throw std::runtime_error("Not a SpringAngles document.");
	}
	line = aIO->readLine();
	if (line.compare("0\n") != 0)
	{
		throw std::runtime_error("Unknown document version.");
	}

	// Read points:
	bool isOK = true;
	auto numPoints = aIO->readLine().toULongLong(&isOK);
	if (!isOK)
	{
		throw std::runtime_error("Failed to read point count.");
	}
	for (qulonglong i = 0; i < numPoints; ++i)
	{
		auto x = aIO->readLine().toDouble(&isOK);
		if (!isOK)
		{
			throw std::runtime_error("Failed to read point X coord");
		}
		auto y = aIO->readLine().toDouble(&isOK);
		if (!isOK)
		{
			throw std::runtime_error("Failed to read point Y coord");
		}
		auto isFixed = (aIO->readLine().compare("1\n") == 0);
		mSpringNet.addPoint(x, y, isFixed);
	}

	// Read springs:
	auto numSprings = aIO->readLine().toULongLong(&isOK);
	if (!isOK)
	{
		throw std::runtime_error("Failed to read spring count.");
	}
	for (qulonglong i = 0; i < numSprings; ++i)
	{
		auto idealLength = aIO->readLine().toDouble(&isOK);
		if (!isOK)
		{
			throw std::runtime_error("Failed to read spring ideal length");
		}
		auto force = aIO->readLine().toDouble(&isOK);
		if (!isOK)
		{
			throw std::runtime_error("Failed to read spring force");
		}
		auto ptIdx1 = aIO->readLine().toULongLong(&isOK);
		if (!isOK)
		{
			throw std::runtime_error("Failed to read spring point index 1.");
		}
		auto ptIdx2 = aIO->readLine().toULongLong(&isOK);
		if (!isOK)
		{
			throw std::runtime_error("Failed to read spring point index 2.");
		}
		mSpringNet.addSpring(idealLength, force, ptIdx1, ptIdx2);
	}
}





void Document::saveToFile(const QString & aFileName)
{
	QFile f(aFileName);
	if (!f.open(QIODevice::WriteOnly))
	{
		throw std::runtime_error("Cannot open file for writing.");
	}
	saveToIO(&f);
	mFileName = aFileName;
}





void Document::saveToIO(QIODevice * aIO)
{
	aIO->write(gDocumentHeader, sizeof(gDocumentHeader) - 1);
	aIO->write("0\n", 2);  // Version

	// Write points:
	aIO->write(QByteArray::number(mSpringNet.numPoints()));
	aIO->write("\n", 1);
	for (const auto & p: mSpringNet.points())
	{
		aIO->write(QByteArray::number(p.x()));
		aIO->write("\n", 1);
		aIO->write(QByteArray::number(p.y()));
		aIO->write("\n", 1);
		aIO->write(p.isFixed() ? "1\n" : "0\n", 2);
	}

	// Write springs:
	aIO->write(QByteArray::number(mSpringNet.numSprings()));
	aIO->write("\n", 1);
	for (const auto & s: mSpringNet.springs())
	{
		aIO->write(QByteArray::number(s.idealLength()));
		aIO->write("\n", 1);
		aIO->write(QByteArray::number(s.force()));
		aIO->write("\n", 1);
		aIO->write(QByteArray::number(s.pointIdx1()));
		aIO->write("\n", 1);
		aIO->write(QByteArray::number(s.pointIdx2()));
		aIO->write("\n", 1);
	}
}
