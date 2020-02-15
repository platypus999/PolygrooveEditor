
#include "Chart.hpp"

void Chart::ReadFromFile() {
	const auto save = Dialog::OpenFile({ FileFilter::JSON() });
	fileName = save.value_or(U"");
	if (fileName.empty())
		return;
	JSONReader reader(fileName);
	divNum = reader[U"divNum"].get<int>();
	measureNum = reader[U"measureNum"].get<int>();
	for (auto&& elm : reader[U"musicAttribute"].objectView()) {
		musicAttribute[elm.name] = elm.value.getString();
	}
	initAllDirectData(measureNum, divNum);
	int measureIndex = 0;
	for (auto&& measureObject : reader[U"sheetData"].arrayView()) {
		for (auto&& elm : measureObject.objectView()) {
			if (elm.value.isString()) {
				//valueÇ™ï∂éöóÒÇÃèÍçáÇÕmeasureAttribute
				measureAttribute[measureIndex][elm.name] = elm.value.get<String>();
			}
		}
		{
			int laneIndex = 0;
			laneNoteData.resize(divNum);
			for (auto&& clump : measureObject[U"laneNote"].arrayView()) {
				for (auto&& elm : clump.objectView()) {
					laneNoteData[measureIndex][laneIndex][Quot(elm.name)] = elm.value.get<String>();
				}
				++laneIndex;
			}
		}
		{
			int laneIndex = 0;
			laneDisplayData.resize(divNum);
			for (auto&& clump : measureObject[U"laneDisp"].arrayView()) {
				for (auto&& elm : clump.objectView()) {
					laneDisplayData[measureIndex][laneIndex][Quot(elm.name)] = elm.value.get<String>();
				}
				++laneIndex;
			}
		}
		{
			int laneIndex = 0;
			divPositionData.resize(divNum);
			for (auto&& clump : measureObject[U"divTheta"].arrayView()) {
				for (auto&& elm : clump.objectView()) {
					divPositionData[measureIndex][laneIndex][Quot(elm.name)] = Quot(elm.value.get<String>());
				}
				++laneIndex;
			}
		}
		{
			int laneIndex = 0;
			divVelocityData.resize(divNum);
			for (auto&& clump : measureObject[U"divOmega"].arrayView()) {
				for (auto&& elm : clump.objectView()) {
					divVelocityData[measureIndex][laneIndex][Rational(elm.name)] = Quot(elm.value.get<String>());
				}
				++laneIndex;
			}
		}
		++measureIndex;
	}
	calculate();
}