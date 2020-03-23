
#include "Chart.hpp"


void Chart::calculate() {
	Logger.writeln(U"Function calculate() called");

	//������
	imosTime.clear();
	imosHSTime.clear(); 
	HSArray.clear();
	imosClock.clear();
	splitVector.clear();
	laneVector.clear();
	isDrawMeasureLine.clear();
	tickTime.clear();
	tickTimeReal.clear();

	//���ߐ������킹��
	measureNum = measureAttribute.size();

	//���ߎ��̂̃f�[�^�ɂ��Čv�Z����
	//�e���߂̎��Ԃ�ݐϘa����
	Quot nowLength = 1;
	double nowBPM = 60;
	Quot innerHS = 1;
	imosTime.push_back(0);
	imosHSTime.push_back(0);
	imosClock.push_back({ 0.0,nowBPM });
	for (auto& measure : measureAttribute) {
		if (measure.count(U"length")) {
			nowLength = Quot(measure.at(U"length"));
		}
		if (measure.count(U"innerHS")) {
			innerHS = Quot(Parse<int>(measure.at(U"innerHS")));
		}
		if (measure.count(U"bpm")) {
			nowBPM = Parse<double>(measure.at(U"bpm"));
		}
		imosTime.push_back(imosTime.back() + nowLength);
		HSArray.push_back(innerHS);
		imosHSTime.push_back(imosHSTime.back() + nowLength * innerHS);
		imosClock.push_back({ imosClock.back().first + MillisecBPMProduct / nowBPM * nowLength.real<double>(), nowBPM });
		//�e���߂̏��ߐ���`�悷�邩���v�Z����
		if (measure.count(U"barDisplay") && measure.at(U"barDisplay") == U"false") {
			isDrawMeasureLine.push_back(false);
		}
		else {
			isDrawMeasureLine.push_back(true);
		}
	}
	//BPM�����炷
	for (int i = 1; i < imosClock.size(); ++i) {
		imosClock[i - 1].second = imosClock[i].second;
	}
	//�m�[�c�̒u����ꏊ���v�Z����
	for (auto& measure : step(measureNum)) {
		Quot start = imosTime[measure];
		Quot end = imosTime[measure + 1];
		for (Quot x = start; x < end; x += Quot(1, granularity)) {
			tickTime.push_back(x);
			tickTimeReal.push_back(x.real());
		}
	}

	//�������̈ʒu�̌v�Z������
	splitVector.resize(divNum);
	//��������pos��speed�𓝍������f�[�^(true:pos, false:speed) [�����[����][�Ӗ��̂Ȃ�index] = {���Ԉʒu,{flag,�����ʒu}}
	std::vector<std::vector<std::pair<Quot, std::pair<bool, Quot>>>>sortInfo(divNum);
	for (auto measure : step(measureNum)) {
		//������
		auto& divPosition = divPositionData[measure];
		auto& divVelocity = divVelocityData[measure];
		for (auto lane : step(divNum)) {
			for (auto& mp : divPosition[lane]) {
				//mp.first==������
				//mp.second==�����ʒu
				sortInfo[lane].push_back({ imosTime[measure] + mp.first,{true,mp.second} });
			}
		}
		for (auto lane : step(divNum)) {
			for (auto& mp : divVelocity[lane]) {
				//mp.first==������
				//mp.second==�����ʒu
				sortInfo[lane].push_back({ imosTime[measure] + mp.first,{false,mp.second} });
			}
		}
	}
	//time�̏�����sortInfo��sort
	sort(sortInfo.begin(), sortInfo.end());
	//sortInfo����splitVector�ɗ�������
	for (auto lane : step(divNum)) {
		Quot nowTime = 0;
		Quot nowTheta = 0;
		Quot nowOmega = 0;
		auto& splitLane = splitVector[lane];
		splitLane.push_back({ nowTime,{nowTheta,nowOmega} });
		for (auto& dat : sortInfo[lane]) {
			if (dat.first == nowTime) {
				splitLane.pop_back();
			}
			else {
				//nowTheta���o�ߎ��ԂɊ�Â��čČv�Z
				nowTheta += nowOmega * (dat.first - nowTime);
				nowTheta = nowTheta.loop();
			}
			if (dat.second.first) {
				nowTheta = dat.second.second;
			}
			else {
				nowOmega = dat.second.second;
			}
			nowTime = dat.first;
			splitLane.push_back({ nowTime,{nowTheta,nowOmega} });
		}
	}

	//���[����̔w�i�̈ʒu���v�Z����
	//�����O�m�[�c�A�����A���E�̕������̃f�[�^���ڎ��@�I�Ƀ}�[�W����
	laneVector.resize(divNum);
	for (auto lane : step(divNum)) {
		auto& leftDiv = splitVector[lane];
		auto& rightDiv = splitVector[(lane + 1) % divNum];
		std::vector<std::pair<Quot, String>>tmpLongNote;
		for (auto measure : step(measureNum)) {
			for (auto elm : laneNoteData[measure][lane]) {
				//elm.first==������ elm.second=="value"
				if (elm.second == longNoteString[0] || elm.second == longNoteString[1])
					tmpLongNote.push_back({ imosTime[measure] + elm.first,holdColor.at(elm.second) });
			}
		}
		std::vector<std::pair<Quot, String>>tmpDisplay;
		for (auto measure : step(measureNum)) {
			for (auto elm : laneDisplayData[measure][lane]) {
				//elm.first==������ elm.second=="value"
				tmpDisplay.push_back({ imosTime[measure] + elm.first,elm.second });
			}
		}
		auto leftItr = leftDiv.begin();
		auto rightItr = rightDiv.begin();
		auto longItr = tmpLongNote.begin();
		auto dispItr = tmpDisplay.begin();
		String longNow = noColor;
		String dispNow = noColor;
		for (;;) {
			auto leftT = (leftItr != leftDiv.end()) ? leftItr->first : imosTime.back();
			auto rightT = (rightItr != rightDiv.end()) ? rightItr->first : imosTime.back();
			auto longT = (longItr != tmpLongNote.end()) ? longItr->first : imosTime.back();
			auto dispT = (dispItr != tmpDisplay.end()) ? dispItr->first : imosTime.back();
			auto minT = std::min(std::min(leftT, rightT), std::min(longT, dispT));
			if (minT == imosTime.back())
				break;
			if (leftT == minT) {
				leftItr++;
			}
			if (rightT == minT) {
				rightItr++;
			}
			if (longT == minT) {
				longNow = longItr->second;
				longItr++;
			}
			if (dispT == minT) {
				dispNow = dispItr->second;
				dispItr++;
			}
			laneVector[lane].push_back({ minT,{ longNow, dispNow} });
		}
	}



}