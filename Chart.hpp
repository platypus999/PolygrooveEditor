#pragma once
#include <Siv3D.hpp>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include "Rational.hpp"
#include "JsonWriter.hpp"
#include "GUIWindow.h"

//���ʃf�[�^�̃N���X
class Chart {
public:

	//----------------------------------------
	//�萔
	//----------------------------------------
	//�ő剽���߂܂ł̕��ʂ����邩
	static const int maxNumOfMeasures = 1024;
	//���̃o�[�̃s�N�Z����
	static const int leftMargin = 160;
	//�ҏW��ʂ̉��̃s�N�Z����
	static const int editSpaceWidth = 640;
	//�ҏW��ʂ̍����̃s�N�Z����
	static const int editSpaceHeight = 600;
	//�`�惋�[�v���̌��E
	static const int renderLimit = 100;
	//�X�N���[���̃X�s�[�h
	const Quot scrollSpeed = Quot(1, 32);
	//�����̃X�N���[���̃X�s�[�h
	const Quot scrollShiftSpeed = Quot(1, 24);
	//�ő�̏c�̏k�ڔ�
	const int maxPixelPerTime = 1800;
	//�ŏ��̏c�̏k�ڔ�
	const int minPixelPerTime = 40;
	//�k�ڔ��ω�������X�s�[�h
	const double pixelPerTimeSpeed = 0.05;
	//�ŏ�����������u���邩�ǂ���
	const int maxGranularity = 256;
	//bpm*ms�̒萔
	const int MillisecBPMProduct = 60 * 1000 * 4;
	//��ʒ����̑��p�`�̒��S����̃s�N�Z����
	const double polygonRadius = 100;
	//�ꏬ�߂ɓ����m�[�c�̑���(�s�N�Z��/����)
	const double noteSpeed = 400;

	//�����L�[�̕������V���[�g�J�b�g
	const std::map<Key, int> numberShortcut = {
		{Key1,4},
		{Key2,8},
		{Key3,16},
		{Key4,24},
		{Key5,32},
		{Key6,48}
	};

	//�F�����݂��Ȃ��Ƃ��̃J���[�R�[�h
	const String noColor = U"false";
	//noColor�͎��ۂɃG�f�B�^��łǂ��\������邩�̃J���[�R�[�h
	const String blackString = U"#000000";
	//�z�[���h�m�[�c�J�n�̕�����
	const String longNoteString[2] = { U"holdBegin", U"holdEnd" };
	//�z�[���h�m�[�c�̓r���̐F
	const std::map<String, String> holdColor = {
		{longNoteString[0],U"#FFD700"},
		{longNoteString[1],noColor},
	};
	//�X���C�h�m�[�c�̎�ނƃX���C�h����(�p�x)
	const std::pair<String, Quot> swipeNoteString[4] = {
		{U"Left", Quot(3,4)},
		{U"Right", Quot(1,4)},
		{U"Out", Quot(0,4)},
		{U"In", Quot(2,4)}
	};
	//��ނ��Ƃ̃m�[�c�̃G�f�B�^�\���F
	const std::map<String, String> noteColor = {
		{U"tap",U"#FFFFFF"},
		{longNoteString[0],U"#FFFF00"},
		{longNoteString[1],U"#FFFF00"},
		{U"swipeLeft",U"#00BFFF"},
		{U"swipeRight",U"#00BFFF"},
		{U"swipeOut",U"#00BFFF"},
		{U"swipeIn",U"#00BFFF"},
		{U"swipeTapLeft",U"#B0C4DE"},
		{U"swipeTapRight",U"#B0C4DE"},
		{U"swipeTapOut",U"#B0C4DE"},
		{U"swipeTapIn",U"#B0C4DE"},
	};
	//�������̐F
	const String divLineColor = U"#404040";
	//�������̑���
	const double divLineWidth = 2.0;
	//���ߐ��̐F
	const String measureLineColor = U"#C0C0C0";
	//���ߐ��̑���
	static inline const double measureLineWidth = 3.0;
	//�ꔏ���Ƃ̐��̐F
	const String tickLineColor = U"#C0C0C0";
	//�ꔏ���Ƃ̐��̑���
	const double tickLineWidth = 1.0;
	//�������ҏW���̐F
	const String divEditLineColor = U"#FF0000";
	//�������ҏW���̑���
	const double divEditLineWidth = 1.5;
	//�������̐܂���̂Ȃ��ڂ̉~�̔��a
	const double divJointRadius = 8.0;
	//�m�[�c�̕�����
	const double noteThickness = 5.0;
	//�����̕�����
	const double displayThickness = 3.0;
	//�����̓����x
	const uint32 displayAlpha = 127;
	//�����O�m�[�c�̓����x
	const uint32 longNoteAlpha = 255;
	//�S�[�X�g�m�[�c�̓����x
	const uint32 ghostAlpha = 127;
	//���̏��ߏ��̔w�i�̐F
	const String leftMarginColor = U"#000000";
	//���̃^�u���j���[�̔w�i�̐F
	const String tabMenuColor = U"#303030";
	//��Έʒu�̃}�[�J�[�̐��̐F
	const String absoluteLineColor = U"#FF0000";
	//���̏��ߏ��̕`��F
	const String infoColor = U"#FFFFFF";
	//���̏��ߏ��̃t�H���g
	const Font infoFont = Font(15);
	//�e�L�X�g�{�b�N�X�̃t�H���g
	const Font boxFont = Font(25);
	//�E�̃G�f�B�^��ʓ��ɕ\�������e�L�X�g
	const Font editorFont = Font(10);
	//�����̃��[�h�̕����̐F
	const String modeTextColor = U"#00FF00";
	//�v���C��ʂ̃m�[�c�̑���(+-delta)
	const double notePlayDelta = 5;
	//�I������Ă��鏬�߂̐F
	const String selectMeasureColor = U"#1E90FF";
	//�I������Ă��鏬�߂̐F�̓����x
	const uint32 selectMeasureAlpha = 31;
	//���߃E�B���h�E�̃{�^���̕�����
	const String buttonStringCopy = U"Copy";
	const String buttonStringCut = U"Cut";
	const String buttonStringPaste = U"Paste";
	const String buttonStringClear = U"Clear";


private:

	//----------------------------------------
	//�ϐ�
	//----------------------------------------

	//�X�N���[�����ꂽ�c���̗�
	Quot scrollTime;
	//�X�N���[�����ꂽ�����̗�
	Quot scrollPos;
	//�c�̏k�ڔ�
	int pixelPerTime;
	//lane mode��div mode��
	bool isLaneMode;
	//�^�u���j���[���J���Ă��邩�ǂ���
	bool isTabMenu;
	//�ŏ��P�ʂ�����������
	int granularity;
	//�e�L�X�g�{�b�N�X
	TextEditState tesGranularity;
	TextEditState tesNowNoteString;
	TextEditState tesNowDispString;
	//���ߏ���GUI�E�B���h�E
	GUIWindow measureEditWindow;
	//�ҏW���Ă��鏬�ߏ��̔ԍ�
	int measureEditIndex;
	//�ҏW���Ă��鏬�ߏ��̑I��͈͂ɂ��Ԃ��ԍŌ�̔ԍ�
	int measureSelectUntilIndex;
	
	//�N���b�N����Ɣz�u����鑕���̕�����
	String nowDispString;
	//�N���b�N����Ɣz�u�����m�[�c�̕�����
	String nowNoteString;
	//�S�[�X�g�m�[�c�̏c���ʒu(����)
	Quot ghostNoteTime;
	//�S�[�X�g�m�[�c�̃��[���ԍ�
	int ghostNoteLane;
	//�������̕ҏW���̏c���ʒu(����)
	Quot divEditLineTime;
	//�ۑ�����t�@�C���̖��O(==�p�X)
	String fileName;
	//���ʂ��Đ������ǂ���
	bool isPlaying;
	//�Đ����Ă���ꏊ
	double currentPlayClock;
	//�Đ��J�n�����ꏊ
	double startPlayClock;
	//�X�g�b�v�E�H�b�`
	Stopwatch stopwatch;
	//�R�s�[�������ߏ��̃f�[�^
	std::vector<std::map<String, String>>copyboardMeasureAttribute;
	std::vector<std::vector<std::map<Quot, String>>>copyboardLaneNoteData;
	std::vector<std::vector<std::map<Quot, String>>>copyboardLaneDisplayData;
	std::vector<std::vector<std::map<Quot, Quot>>>copyboardDivPositionData;
	std::vector<std::vector<std::map<Quot, Quot>>>copyboardDivVelocityData;

	//----------------------------------------
	//���ړI�ȃf�[�^
	//----------------------------------------

	//�������̌�
	int divNum;

	//���ߐ�
	int measureNum;

	//���ʑS�̂Ɋւ���f�[�^
	std::map<String, String>musicAttribute;

	//�ꏬ�߂��ƂɊւ���f�[�^
	//[���ߔԍ�]["ID"]="value"
	std::vector<std::map<String, String>>measureAttribute;

	//���[����ɂ���m�[�c�̃f�[�^
	//[���ߔԍ�][�����[����][������]="value"
	std::vector<std::vector<std::map<Quot, String>>>laneNoteData;

	//���[����ɂ��鑕���̃f�[�^
	//[���ߔԍ�][�����[����][������]="value"
	std::vector<std::vector<std::map<Quot, String>>>laneDisplayData;

	//�������̈ʒu�̃f�[�^
	//[���ߔԍ�][�����[����][������]=�ʒu
	std::vector<std::vector<std::map<Quot, Quot>>>divPositionData;

	//�������̑��x�̃f�[�^
	//[���ߔԍ�][�����[����][������]=���x
	std::vector<std::vector<std::map<Quot, Quot>>>divVelocityData;


	//----------------------------------------
	//�ԐړI�ȃf�[�^
	//----------------------------------------
	//�e���ߖڂ̐擪����
	std::vector<Quot>imosTime;
	//�e���ߖڂ̓����n�C�X�s���݂̐擪����
	std::vector<Quot>imosHSTime;
	//�e���߂�HS���i�[�����z��
	std::vector<Quot>HSArray;
	//�e���ߖڂ̐擪����(�~���b)��BPM�̃y�A
	std::vector<std::pair<double, double>>imosClock;
	//�e�������̉����ʒu [�����[����][�Ӗ��̂Ȃ�index] = {time,{pos,speed}}
	//time�Ń\�[�g�ς�
	std::vector<std::vector<std::pair<Quot, std::pair<Quot, Quot>>>>splitVector;
	//�e���[���̔w�i(�����O�m�[�c+����) [�����[����][�Ӗ��̂Ȃ�index] = {time,{colorLong,colorDisp}}
	//time�Ń\�[�g�ς�
	std::vector<std::vector<std::pair<Quot, std::pair<String, String>>>>laneVector;
	//�e���߂ɂ��ď��ߐ���`�悷�邩
	std::vector<bool>isDrawMeasureLine;
	//�m�[�c�╪���_�Ȃǂ��u����time�������ɂ܂Ƃ߂��z��
	std::vector<Quot>tickTime;
	//�m�[�c�╪���_�Ȃǂ��u����time�������ɂ܂Ƃ߂��z��(double��)
	std::vector<double>tickTimeReal;

	//�������̂����I������Ă�����̂�true�A����Ă��Ȃ����̂�false
	Array<bool> divEditIndex;


	//���ړI�ȃf�[�^�ƕ������Ɋւ���z������ׂď�����(�����̃T�C�Y�ɕύX)
	void initAllDirectData(int size = 0, int secondSize = 0) {
		measureAttribute.clear();
		laneNoteData.clear();
		laneDisplayData.clear();
		divPositionData.clear();
		divVelocityData.clear();
		measureAttribute.resize(size);
		laneNoteData.resize(size, std::vector<std::map<Quot, String>>(secondSize));
		laneDisplayData.resize(size, std::vector<std::map<Quot, String>>(secondSize));
		divPositionData.resize(size, std::vector<std::map<Quot, Quot>>(secondSize));
		divVelocityData.resize(size, std::vector<std::map<Quot, Quot>>(secondSize));
	}

	//����time�ł̕�����index�̉����ʒu
	Quot divPos(int index, Quot time) const {
		//�񕪒T���ňʒu������o��
		auto itr = std::upper_bound(splitVector[index].begin(), splitVector[index].end(), std::pair<Quot, std::pair<Quot, Quot>>{ Quot(time), { Quot(2,1),Quot() } });
		if (itr != splitVector[index].begin()) {
			--itr;
		}
		return (itr->second.first + (time - itr->first) * itr->second.second).loop();
	}
	//����time�ł̕�����index�̉����ʒu(double��)
	double divPosReal(int index, double time) const {
		//�񕪒T���ňʒu������o��
		auto itr = std::upper_bound(splitVector[index].begin(), splitVector[index].end(),
			std::pair<double, std::pair<Quot, Quot>>{ (time), { Quot(2,1),Quot() } },
			[&](const std::pair<double, std::pair<Quot, Quot>>& val, const std::pair<Quot, std::pair<Quot, Quot>>& emp) {
				auto fps = std::pair<double, std::pair<Quot, Quot>>(emp.first.real(), emp.second);
				return val < fps;
			});
		if (itr != splitVector[index].begin()) {
			--itr;
		}
		auto tmp = (itr->second.first.real() + (time - itr->first.real()) * itr->second.second.real());
		return tmp - floor(tmp);
	}
	//����time�ł̕�����index�̉������x
	Quot divSpeed(int index, Quot time) const {
		//�񕪒T���ňʒu������o��
		auto itr = std::upper_bound(splitVector[index].begin(), splitVector[index].end(), std::pair<Quot, std::pair<Quot, Quot>>{ Quot(time), { Quot(2,1),Quot() } });
		if (itr != splitVector[index].begin()) {
			--itr;
		}
		return itr->second.second;
	}

	//����(�p�x�P��)���s�N�Z���ɕϊ�
	inline double toX(Quot angle) const {
		return leftMargin + double(Quot(editSpaceWidth) * (angle + scrollPos));
	}
	//�c��(���ԒP��)���s�N�Z���ɕϊ�
	inline double toY(Quot time) const {
		return editSpaceHeight - double((time - scrollTime) * pixelPerTime);
	}
	//�s�N�Z����X�������牡��(�p�x�P��)���v�Z
	inline double rawFromX(double pixelX) {
		double solution = (pixelX - leftMargin) / double(editSpaceWidth) - scrollPos.real() + 1.0;
		return solution - floor(solution);
	}
	//�s�N�Z����Y��������c��(���ԒP��)���v�Z
	inline double rawFromY(double pixelY) const {
		return (editSpaceHeight - pixelY) / pixelPerTime + scrollTime.real();
	}

	//����width�̋�Ԃ�0�x�ȏ�180�x�����̋�Ԃ����`�F�b�N
	inline bool isOk(Quot width) const {
		return width >= Quot(0, 1) && width < Quot(1, 2);
	}
	//������noColor��������valid�ȃJ���[�R�[�h(#xxxxxx)���ǂ������`�F�b�N
	inline bool isValidColorString(const String& str) const {
		if (str == noColor)return true;
		if (str.length() != 7)return false;
		for (int i = 1; i <= 6; ++i) {
			if (!(U'0' <= str[i] && str[i] <= U'9') && !(U'a' <= str[i] && str[i] <= U'f'))
				return false;
		}
		return str[0] == U'#';
	}

	//���Ԃ��~���b�ɒ���
	inline double timeToClock(Quot time) const {
		//�����ߖڂ����v�Z
		int measure = upper_bound(imosTime.begin(), imosTime.end(), time) - imosTime.begin() - 1;
		//���ߐ擪����̃~���b�����v�Z
		double jikan = MillisecBPMProduct / imosClock[measure].second * (time - imosTime[measure]).real<double>();
		return imosClock[measure].first + jikan;
	}
	//�~���b������(������double�^)�ɒ���
	inline double clockToRealTime(double clock) const {
		int measure = upper_bound(imosClock.begin(), imosClock.end(), std::make_pair(clock, DBL_MAX)) - imosClock.begin() - 1;

		double shousetsu = (clock - imosClock[measure].first) / (MillisecBPMProduct / imosClock[measure].second);
		return imosTime[measure].real() + shousetsu;
	}
	//�ɍ��W���s�N�Z���ɒ���(���_�͉�ʂ̒��S)
	inline Point polToRec(double radius, double theta) const {
		return Point((int)(radius * Math::Cos(theta)), (int)(radius * Math::Sin(theta))) + Window::ClientCenter();
	}
	//���Ԃ�HS���݂̎��ԂɕύX����
	inline Quot timeToHSTime(Quot time) const {
		int index = upper_bound(imosTime.begin(), imosTime.end(), time) - imosTime.begin() - 1;
		if (index == measureNum)return imosHSTime.back();
		return imosHSTime[index] - (imosTime[index] - time) * HSArray[index];
	}
	//���Ԃ�HS���݂̎��ԂɕύX����(double��)
	inline double realTimeToRealHSTime(double time) const {
		int index = upper_bound(imosTime.begin(), imosTime.end(), time, [&](const double& d,const Quot& q) {return d < q.real(); }) - imosTime.begin() - 1;
		if (index == measureNum)return imosHSTime.back().real();
		return imosHSTime[index].real() - (imosTime[index].real() - time) * HSArray[index].real();
	}
public:
	//�R���X�g���N�^
	Chart();

	//�f�[�^����K�v�ȏ���Parse����
	void calculate();

	//��ʂ�\������
	void Show() const;

	//�G�f�B�^��ʂ�\������
	void ShowEditor() const;

	//�Đ���ʂ�\������
	void ShowPlayer() const;

	//�������s��
	void Update();

	//�G�f�B�^�[��ʂ̏������s��(�Đ����͌Ă΂Ȃ�)
	void UpdateEditor();

	//�Đ���ʂ̏������s��
	void UpdatePlayer();

	//�t�@�C���ɕۑ�����
	void SaveToFile();

	//�t�@�C�����J��
	void ReadFromFile();
};

