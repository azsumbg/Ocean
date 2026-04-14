#include "pch.h"
#include "Ocean.h"

// EXCEPTION class ***********************

dll::EXCEPTION::EXCEPTION(int _what_happened) :_what{ _what_happened } {};

const wchar_t* dll::EXCEPTION::messsage()
{
	switch (_what)
	{
	case BAG_BAD_INDEX:
		return L"Bad index passed to a BAG container instance !";

	case BAG_BAD_PARAM:
		return L"Bad parameter passed to a BAG container instance !";

	case BAG_BAD_PTR:
		return L"Bad base pointer in BAG container instance !";

	case BAG_UNKNOWN_ERR:
		return L"Unknown error occurred in BAG container instance !";
	}

	return L"Unknown error occurred in BAG container instance !";
}

//////////////////////////////////////////

// RANDIT class *************************

dll::RANDIT::RANDIT()
{
	std::random_device rd{};
	std::seed_seq sq{ rd(),rd(), rd(), rd(), rd(), rd(), rd(), rd() };

	twister = new std::mt19937(sq);
}
dll::RANDIT::~RANDIT()
{
	if (twister)delete twister;
}

int dll::RANDIT::operator ()(int min, int max)
{
	if (max <= min)return 0;

	std::uniform_int_distribution distrib(min, max);

	return distrib(*twister);
}
float dll::RANDIT::operator ()(float min, float max)
{
	if (max <= min)return 0;

	std::uniform_real_distribution<float> distrib(min, max);

	return distrib(*twister);
}

/////////////////////////////////////////

// PROTON class ************************

dll::PROTON::PROTON()
{
	_width = 1.0f;
	_height = 1.0f;

	x_rad = _width / 2.0f;
	y_rad = _height * 2.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;
}
dll::PROTON::PROTON(float _sx, float _sy)
{
	start.x = _sx;
	start.y = _sy;

	_width = 1.0f;
	_height = 1.0f;

	x_rad = _width / 2.0f;
	y_rad = _height * 2.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;
};
dll::PROTON::PROTON(float _sx, float _sy, float _swidth, float _sheight)
{
	start.x = _sx;
	start.y = _sy;

	_width = _swidth;
	_height = _sheight;
	
	x_rad = _width / 2.0f;
	y_rad = _height * 2.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;
}

void dll::PROTON::set_edges()
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;
}
void dll::PROTON::set_path(float _ex, float _ey)
{
	ver_dir = false;
	hor_dir = false;

	move_sx = start.x;
	move_sy = start.y;

	move_ex = end.x;
	move_ey = end.y;

	if (move_sx == move_ex || (move_ex > start.x && move_ex <= end.x))
	{
		ver_dir = true;
		return;
	}
	if (move_sy == move_ey || (move_ey > start.y && move_ey <= end.y))
	{
		hor_dir = true;
		return;
	}

	slope = (move_ey - move_sy) / (move_ex - move_sx);
	intercept = move_sy - slope * move_sx;
}

void dll::PROTON::set_width(float _new_width)
{
	_width = _new_width;

	x_rad = _width / 2.0f;

	end.x = start.x + _width;

	center.x = start.x + x_rad;
}
void dll::PROTON::set_height(float _new_height)
{
	_height = _new_height;

	y_rad = _height * 2.0f;

	end.y = start.y + _height;

	center.y = start.y + y_rad;
}
void dll::PROTON::new_dims(float _new_width, float _new_height)
{
	_width = _new_width;
	_height = _new_height;

	x_rad = _width / 2.0f;
	y_rad = _height * 2.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;
}

float dll::PROTON::get_width()const
{
	return _width;
}
float dll::PROTON::get_height()const
{
	return _height;
}

float dll::PROTON::get_target_x()const
{
	return move_ex;
}
float dll::PROTON::get_target_y()const
{
	return move_ey;
}

/////////////////////////////////////////