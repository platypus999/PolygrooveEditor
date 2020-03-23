#pragma once

#include <Siv3D.hpp>

//���蕶���̃G�t�F�N�g
struct GreatEffect : IEffect
{
	Vec2 m_start;

	String m_number;

	Font m_font = Font(15);

	GreatEffect(const Vec2& start, String num)
		: m_start(start)
		, m_number(num)
	{}

	bool update(double t) override
	{
		m_font(m_number).drawAt(m_start.movedBy(0, t * -120), Color(Palette::Blue).setA(Max(1.0 - (t * 2.0), 0.0) * 255));

		// 0.5 �b�ȏ�o�߂ŏ���
		return t < 0.5;
	}
};