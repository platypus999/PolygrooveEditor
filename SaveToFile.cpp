
#include "Chart.hpp"

void Chart::SaveToFile() {
	if (fileName.empty()) {
		const auto save = Dialog::SaveFile({ FileFilter::JSON() });
		fileName = save.value_or(U"");
	}
	if (fileName.empty())
		return;
	JSONWriter writer;
	writer.startObject();

	writer.key(U"divNum");
	writer.writeInt32(divNum);

	writer.key(U"measureNum");
	writer.writeInt32(measureNum);

	writer.key(U"musicAttribute");
	writer.startObject();
	for (const auto& elm : musicAttribute) {
		writer.key(elm.first);
		writer.writeString(elm.second);
	}
	writer.endObject();

	writer.key(U"sheetData");
	//1è¨êﬂÇóvëfÇ∆ÇµÇΩîzóÒ
	writer.startArray();
	for (int i = 0; i < measureNum; ++i) {
		writer.startObject();
		//àÍè¨êﬂÇ≤Ç∆ÇÃèÓïÒ
		for (const auto& elm : measureAttribute[i]) {
			writer.key(elm.first);
			writer.writeString(elm.second);
		}

		writer.key(U"laneNote");
		writer.startArray();
		for (int j = 0; j < divNum; ++j) {
			writer.startObject();
			for (const auto& elm : laneNoteData[i][j]) {
				writer.key(elm.first.parse());
				writer.writeString(elm.second);
			}
			writer.endObject();
		}
		writer.endArray();

		writer.key(U"laneDisp");
		writer.startArray();
		for (int j = 0; j < divNum; ++j) {
			writer.startObject();
			for (const auto& elm : laneDisplayData[i][j]) {
				writer.key(elm.first.parse());
				writer.writeString(elm.second);
			}
			writer.endObject();
		}
		writer.endArray();

		writer.key(U"divTheta");
		writer.startArray();
		for (int j = 0; j < divNum; ++j) {
			writer.startObject();
			for (const auto& elm : divPositionData[i][j]) {
				writer.key(elm.first.parse());
				writer.writeString(elm.second.parse());
			}
			writer.endObject();
		}
		writer.endArray();

		writer.key(U"divOmega");
		writer.startArray();
		for (int j = 0; j < divNum; ++j) {
			writer.startObject();
			for (const auto& elm : divVelocityData[i][j]) {
				writer.key(elm.first.parse());
				writer.writeString(elm.second.parse());
			}
			writer.endObject();
		}
		writer.endArray();

		writer.endObject();
	}
	writer.endArray();

	writer.endObject();

	TextWriter fileOut(fileName);
	fileOut.write(writer.get());
}