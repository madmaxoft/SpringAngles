#include "Document.hpp"

#include <QFile>





static const char gDocumentHeader[] = "SpringAngles document\n";





Document::Document()
{
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
