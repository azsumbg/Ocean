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

	move_ex = _ex;
	move_ey = _ey;

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
	y_rad = _height / 2.0f;

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

// FIELD class *************************

dll::FIELD::FIELD()
{
	float x_pos = -scr_width;
	float y_pos = -scr_height;

	for (int rows = 0; rows < 3; ++rows)
	{
		for (int cols = 0; cols < 3; ++cols)
		{
			ocean_tiles[rows][cols].left = x_pos;
			ocean_tiles[rows][cols].right = x_pos + scr_width;
			ocean_tiles[rows][cols].up = y_pos;
			ocean_tiles[rows][cols].down = y_pos + scr_height;

			x_pos += scr_width;
		}

		x_pos = -scr_width;
		y_pos += scr_height;
	}
}

void dll::FIELD::move_ocean(dirs to_where, float gear)
{
	float my_speed = _speed + gear / 10.0f;

	switch (to_where)
	{
	case dirs::up:
		if (ocean_tiles[2][0].down - my_speed >= scr_height)
		{
			for (int rows = 0; rows < 3; ++rows)
			{
				for (int cols = 0; cols < 3; ++cols)
				{
					ocean_tiles[rows][cols].up -= my_speed;
					ocean_tiles[rows][cols].down -= my_speed;
				}
			}
		}
		break;

	case dirs::down:
		if (ocean_tiles[0][0].up + my_speed <= sky)
		{
			for (int rows = 0; rows < 3; ++rows)
			{
				for (int cols = 0; cols < 3; ++cols)
				{
					ocean_tiles[rows][cols].up += my_speed;
					ocean_tiles[rows][cols].down += my_speed;
				}
			}
		}
		break;

	case dirs::left:
		if (ocean_tiles[0][2].right - my_speed >= scr_width)
		{
			for (int rows = 0; rows < 3; ++rows)
			{
				for (int cols = 0; cols < 3; ++cols)
				{
					ocean_tiles[rows][cols].left -= my_speed;
					ocean_tiles[rows][cols].right -= my_speed;
				}
			}
		}
		break;

	case dirs::right:
		if (ocean_tiles[0][0].left + my_speed <= 0)
		{
			for (int rows = 0; rows < 3; ++rows)
			{
				for (int cols = 0; cols < 3; ++cols)
				{
					ocean_tiles[rows][cols].left += my_speed;
					ocean_tiles[rows][cols].right += my_speed;
				}
			}
		}
		break;
	}
}

bool dll::FIELD::in_view_port(int row, int col)const
{
	if (row < 0 || row > 2 || col < 0 || col > 2)return false;
	else if (!(ocean_tiles[row][col].left >= _ViewPort.right || ocean_tiles[row][col].right <= _ViewPort.left
		|| ocean_tiles[row][col].up >= _ViewPort.down || ocean_tiles[row][col].down <= _ViewPort.up))return true;
	
	return false;
}
bool dll::FIELD::in_view_port(FRECT what)const
{
	if (!(what.left >= _ViewPort.right || what.right <= _ViewPort.left
		|| what.up >= _ViewPort.down || what.down <= _ViewPort.up))return true;

	return false;
}

////////////////////////////////////////

// HERO class *************************

dll::HERO::HERO(float _sx, float _sy) :PROTON(_sx, _sy)
{
	new_dims(150.0f, 140.0f);
}

bool dll::HERO::obstacle_bumped(BAG<FRECT>& obstacles)
{
	if (obstacles.empty())return false;
	else if (obstacles.size() == 1)
	{
		if (Intersect(FRECT{ start.x,start.y,end.x,end.y }, obstacles[0]))return true;
	}
	else
	{
		for (size_t i = 0; i < obstacles.size(); ++i)
		{
			if (Intersect(FRECT{ start.x,start.y,end.x,end.y }, obstacles[i]))return true;
		}
	}

	return false;
}

void dll::HERO::move(float ex, float ey, float gear, BAG<FRECT>& field_obst)
{
	float my_speed = _speed + gear / 10.0f;

	set_path(ex, ey);

	if (ex > center.x)
	{
		if (ey > center.y)dir = dirs::down_right;
		else if (ey < center.y)dir = dirs::up_right;
		else dir = dirs::right;
	}
	else if (ex < center.x)
	{
		if (ey > center.y)dir = dirs::down_left;
		else if (ey < center.y)dir = dirs::up_left;
		else dir = dirs::left;
	}
	else
	{
		if (ey > center.y)dir = dirs::down;
		else if (ey < center.y) dir = dirs::up;
		else dir = dirs::stop;
	}

	if (hor_dir)
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= scr_width)
			{
				start.x += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					set_edges();
					dir = dirs::stop;
				}
			}
			else dir = dirs::stop;

			if (center.x >= move_ex)dir = dirs::stop;
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= 0)
			{
				start.x -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					set_edges();
					dir = dirs::stop;
				}
			}
			else dir = dirs::stop;

			if (center.x <= move_ex)dir = dirs::stop;
		}
	}
	else if (ver_dir)
	{
		if (move_ey > move_sy)
		{
			if (end.y + my_speed <= ground)
			{
				start.y += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y -= my_speed;
					set_edges();
					dir = dirs::stop;
				}
				
				if (center.y >= move_ey)dir = dirs::stop;
			}
			else dir = dirs::stop;
		}
		else if (move_ey < move_sy)
		{
			if (start.y - my_speed >= sky)
			{
				start.y -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y += my_speed;
					set_edges();
					dir = dirs::stop;
				}
			}
			else dir = dirs::stop;

			if (center.y <= move_ey)dir = dirs::stop;
		}
	}
	else
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= scr_width)
			{
				start.x += my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
					dir = dirs::stop;
				}
			}
			else dir = dirs::stop;

			if (center.x >= move_ex)
			{
				if ((move_ey > move_sy && center.y >= move_ey) || (move_ey < move_sy && center.y <= move_ey))
					dir = dirs::stop;
			}
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= 0)
			{
				start.x -= my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
					dir = dirs::stop;
				}
			}
			else dir = dirs::stop;

			if (center.x <= move_ex)
			{
				if ((move_ey > move_sy && center.y >= move_ey) || (move_ey < move_sy && center.y <= move_ey))
					dir = dirs::stop;
			}
		}
	}

	if (start.y < sky)
	{
		start.y = sky;
		set_edges();
		dir = dirs::stop;
	}
	if (end.y > ground)
	{
		start.y = ground - _height;
		set_edges();
		dir = dirs::stop;
	}
	if (start.x < 0)
	{
		start.x = 0;
		set_edges();
		dir = dirs::stop;
	}
	if (end.x > scr_width)
	{
		start.x = scr_width - _width;
		set_edges();
		dir = dirs::stop;
	}
}

int dll::HERO::attack()
{
	--attack_delay;
	if (attack_delay <= 0)
	{
		attack_delay = max_attack_delay;
		return damage;
	}

	return 0;
}
int dll::HERO::get_frame()
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = max_frame_delay;
		++frame;
		if (frame > max_frames)frame = 0;
	}

	return frame;
}

void dll::HERO::Release()
{
	delete this;
}

dll::HERO* dll::HERO::create(float sx, float sy)
{
	HERO* ret{ nullptr };

	ret = new HERO(sx, sy);

	return ret;
}

///////////////////////////////////////

// OBSTACLE class *********************

dll::OBSTACLE::OBSTACLE(obstacles _what, float _sx, float _sy) :PROTON(_sx, _sy)
{
	type = _what;

	switch (type)
	{
	case obstacles::big_rock:
		new_dims(200.0f, 244.0f);
		break;

	case obstacles::mid_rock:
		new_dims(150.0f, 128.0f);
		break;

	case obstacles::small_rock:
		new_dims(107.0f, 120.0f);
		break;

	case obstacles::port:
		new_dims(199.0f, 167.0f);
		break;

	case obstacles::island:
		new_dims(300.0f, 191.0f);
		break;
	}

	my_rect.left = start.x;
	my_rect.right = end.x;
	my_rect.up = start.y;
	my_rect.down = end.y;
}

void dll::OBSTACLE::move(dirs to_where, float gear)
{
	float my_speed = _speed + gear / 10.0f;
	
	switch (to_where)
	{
	case dirs::up:
		if (my_rect.up - my_speed >= -scr_height)
		{
			start.y -= my_speed;
			set_edges();
		}
		break;

	case dirs::down:
		if (my_rect.down + my_speed <= 2.0f * scr_height)
		{
			start.y += my_speed;
			set_edges();
		}
		break;

	case dirs::left:
		if (my_rect.left - my_speed >= -scr_width)
		{
			start.x -= my_speed;
			set_edges();
		}
		break;

	case dirs::right:
		if (my_rect.right + my_speed >= 2.0f * scr_width)
		{
			start.x += my_speed;
			set_edges();
		}
		break;
	}

	my_rect.left = start.x;
	my_rect.right = end.x;
	my_rect.up = start.y;
	my_rect.down = end.y;
}

int dll::OBSTACLE::get_frame()
{
	if (type != obstacles::swirl)return 0;

	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = 6;
		++frame;
		if (frame > max_frames)frame = 0;
	}

	return frame;
}

bool dll::OBSTACLE::in_view_port(FRECT what)const
{
	if (!(what.left >= _ViewPort.right || what.right <= _ViewPort.left
		|| what.up >= _ViewPort.down || what.down <= _ViewPort.up))return true;

	return false;
}

void dll::OBSTACLE::Release()
{
	delete this;
}

dll::OBSTACLE* dll::OBSTACLE::create(obstacles what, float sx, float sy)
{
	OBSTACLE* ret{ nullptr };

	ret = new OBSTACLE(what, sx, sy);

	return ret;
}

/////////////////////////////////////////

// BOAT class ***************************

dll::BOAT::BOAT(float _sx, float _sy, bool _hero) :PROTON(_sx, _sy)
{
	new_dims(50.0f, 45.0f);
	dir = dirs::up;

	hero_boat = _hero;
}

void dll::BOAT::Release() 
{
	delete this;
}

bool dll::BOAT::obstacle_bumped(BAG<FRECT>& obstacles)
{
	if (obstacles.empty())return false;
	else if (obstacles.size() == 1)
	{
		if (Intersect(FRECT{ start.x,start.y,end.x,end.y }, obstacles[0]))return true;
	}
	else
	{
		for (size_t i = 0; i < obstacles.size(); ++i)
		{
			if (Intersect(FRECT{ start.x,start.y,end.x,end.y }, obstacles[i]))return true;
		}
	}

	return false;
}

void dll::BOAT::move(float ex, float ey, float gear, BAG<FRECT>& field_obst)
{
	float my_speed = _speed + gear / 10.0f;

	set_path(ex, ey);

	if (ex > center.x)
	{
		if (ey > center.y)dir = dirs::down_right;
		else if (ey < center.y)dir = dirs::up_right;
		else dir = dirs::right;
	}
	else if (ex < center.x)
	{
		if (ey > center.y)dir = dirs::down_left;
		else if (ey < center.y)dir = dirs::up_left;
		else dir = dirs::left;
	}
	else
	{
		if (ey > center.y)dir = dirs::down;
		else dir = dirs::up;
	}

	if (hor_dir)
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= scr_width)
			{
				start.x += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					set_edges();
				}
			}
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= 0)
			{
				start.x -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					set_edges();
				}
			}
		}
	}
	else if (ver_dir)
	{
		if (move_ey > move_sy)
		{
			if (end.y + my_speed <= ground)
			{
				start.y += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y -= my_speed;
					set_edges();
				}
			}
		}
		else if (move_ey < move_sy)
		{
			if (start.y - my_speed >= sky)
			{
				start.y -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y += my_speed;
					set_edges();
				}
			}
		}
	}
	else
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= scr_width)
			{
				start.x += my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
				}
			}
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= 0)
			{
				start.x -= my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
				}
			}
		}
	}

	if (start.y < sky)
	{
		start.y = sky;
		set_edges();
	}
	if (end.y > ground)
	{
		start.y = ground - _height;
		set_edges();
	}
	if (start.x < 0)
	{
		start.x = 0;
		set_edges();
	}
	if (end.x > scr_width)
	{
		start.x = scr_width - _width;
		set_edges();
	}
}

dll::BOAT* dll::BOAT::create(float sx, float sy, bool hero)
{
	BOAT* ret{ nullptr };
	
	ret = new BOAT(sx, sy, hero);

	return ret;
}

/////////////////////////////////////////

// SHOT class ***************************

dll::SHOT::SHOT(float _sx, float _sy, float _ex, float _ey) :PROTON(_sx, _sy, 20.0f, 20.0f)
{
	set_path(_ex, _ey);
}

bool dll::SHOT::move(float gear)
{
	float my_speed = _speed + gear / 10.0f;

	if (ver_dir)
	{
		if (move_ey < move_sy)
		{
			start.y -= my_speed;
			set_edges();

			if (start.y < move_ey || start.y <= sky)return false;
		}
		else if (move_ey > move_sy)
		{
			start.y += my_speed;
			set_edges();

			if (end.y > move_ey || end.y >= ground)return false;
		}
		else return false;
	}
	else if (hor_dir)
	{
		if (move_ex < move_sx)
		{
			start.x -= my_speed;
			set_edges();

			if (start.x < move_ex || start.x <= 0)return false;
		}
		else if (move_ex > move_sx)
		{
			start.x += my_speed;
			set_edges();

			if (end.x > move_ex || end.x >= scr_width)return false;
		}
		else return false;
	}
	else
	{
		if (move_ex < move_sx)
		{
			start.x -= my_speed;
			start.y = start.x * slope + intercept;
			set_edges();

			if (start.x < move_ex || start.x <= 0 || start.y < move_ey || start.y <= sky
				|| end.y > move_ey || end.y >= ground)return false;
		}
		else if (move_ex > move_sx)
		{
			start.x += my_speed;
			start.y = start.x * slope + intercept;
			set_edges();

			if (end.x > move_ex || end.x >= scr_width || start.y < move_ey || start.y <= sky
				|| end.y > move_ey || end.y >= ground)return false;
		}
		else return false;
	}

	return true;
}

void dll::SHOT::Release()
{
	delete this;
}

dll::SHOT* dll::SHOT::create(float sx, float sy, float ex, float ey)
{
	SHOT* ret{ nullptr };
	
	ret = new SHOT(sx, sy, ex, ey);

	return ret;
}

////////////////////////////////////////

// EVIL class **************************

dll::EVIL::EVIL(evils _what, float _sx, float _sy) :PROTON(_sx, _sy)
{
	type = _what;

	switch (type)
	{
	case evils::evil1:
		new_dims(132.0f, 115.0f);
		frame_delay = 13;
		max_frames = 5;
		attack_delay = 70;
		_speed = 1.1f;
		lifes = 100;
		view_range = 250.0f;
		attack_range = 150.0f;
		armor = 2;
		damage = 20;
		break;

	case evils::evil2:
		new_dims(150.0f, 175.0f);
		frame_delay = 12;
		max_frames = 6;
		attack_delay = 85;
		_speed = 0.8f;
		lifes = 140;
		view_range = 280.0f;
		attack_range = 180.0f;
		armor = 4;
		damage = 40;
		break;

	case evils::evil3:
		new_dims(100.0f, 108.0f);
		frame_delay = 20;
		max_frames = 3;
		attack_delay = 50;
		_speed = 1.3f;
		lifes = 100;
		view_range = 250.0f;
		attack_range = 120.0f;
		armor = 1;
		damage = 10;
		break;

	case evils::evil4:
		new_dims(120.0f, 80.0f);
		frame_delay = 15;
		max_frames = 4;
		attack_delay = 50;
		_speed = 1.5f;
		lifes = 80;
		view_range = 200.0f;
		attack_range = 100.0f;
		armor = 0;
		damage = 8;
		break;
	}

	max_frame_delay = frame_delay;
	max_attack_delay = attack_delay;

	if (start.x > scr_width / 2.0f)
	{
		if (start.y < scr_height / 2.0f)
		{
			dir = dirs::down_left;
			set_path(start.x - 300.0f, ground);
		}
		else if (start.y > scr_height / 2.0f)
		{
			dir = dirs::up_left;
			set_path(start.x - 300.0f, sky);
		}
		else
		{
			dir = dirs::left;
			set_path(start.x - 300.0f, start.y);
		}
	}
	else if (start.x < scr_width / 2.0f)
	{
		if (start.y < scr_height / 2.0f)
		{
			dir = dirs::down_right;
			set_path(start.x + 300.0f, ground);
		}
		else if (start.y > scr_height / 2.0f)
		{
			dir = dirs::up_right;
			set_path(start.x - 300.0f, sky);
		}
		else
		{
			dir = dirs::right;
			set_path(start.x + 300.0f, start.y);
		}
	}
	else
	{
		if (start.y < scr_height / 2.0f)
		{
			dir = dirs::down_left;
			set_path(start.x - 300.0f, ground);
		}
		else
		{
			dir = dirs::up_right;
			set_path(start.x + 300.0f, sky);
		}
	}

	max_lifes = lifes;
}
	
bool dll::EVIL::obstacle_bumped(BAG<FRECT>& obstacles)
{
	if (obstacles.empty())return false;
	else if (obstacles.size() == 1)
	{
		if (Intersect(FRECT{ start.x,start.y,end.x,end.y }, obstacles[0]))return true;
	}
	else
	{
		for (size_t i = 0; i < obstacles.size(); ++i)
		{
			if (Intersect(FRECT{ start.x,start.y,end.x,end.y }, obstacles[i]))return true;
		}
	}

	return false;
}

void dll::EVIL::patrol(float gear, BAG<FRECT>& field_obst)
{
	float my_speed = _speed + gear / 10.0f;

	if (move_ex > center.x)
	{
		if (move_ey > center.y)dir = dirs::down_right;
		else if (move_ey < center.y)dir = dirs::up_right;
		else dir = dirs::right;
	}
	else if (move_ex < center.x)
	{
		if (move_ey > center.y)dir = dirs::down_left;
		else if (move_ey < center.y)dir = dirs::up_left;
		else dir = dirs::left;
	}
	else
	{
		if (move_ey > center.y)dir = dirs::down;
		else dir = dirs::up;
	}

	if (hor_dir)
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= move_ex)
			{
				start.x += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					set_edges();

					set_path(start.x - 300.0f, move_ey);
				}
			}
			else set_path(start.x - 300.0f, move_ey);
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= move_ex)
			{
				start.x -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					set_edges();
					set_path(start.x + 300.0f, move_ey);
				}
			}
			else set_path(start.x + 300.0f, move_ey);
		}
		else
		{
			if (center.y >= scr_height / 2.0f)set_path(start.x + 300.0f, sky);
			else set_path(start.x - 300.0f, ground);
		}
	}
	else if (ver_dir)
	{
		if (move_ey > move_sy)
		{
			if (end.y + my_speed <= ground)
			{
				start.y += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y -= my_speed;
					set_edges();
					set_path(start.x, sky);
				}
			}
			else set_path(start.x, sky);
		}
		else if (move_ey < move_sy)
		{
			if (start.y - my_speed >= sky)
			{
				start.y -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y += my_speed;
					set_edges();
					set_path(start.x, ground);
				}
			}
			else set_path(start.x, ground);
		}
		else
		{
			if (center.x= scr_width / 2.0f)set_path(start.x + 300.0f, sky);
			else set_path(start.x - 300.0f, ground);
		}
	}
	else
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= move_ex)
			{
				start.x += my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
					set_path(start.x - 300.0f, move_ey);
				}
			}
			else set_path(start.x - 300.0f, move_ey);
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= move_ex)
			{
				start.x -= my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
					set_path(start.x + 300.0f, move_ey);
				}
			}
			else set_path(start.x + 300.0f, move_ey);
		}
		else
		{
			if (center.y >= scr_height / 2.0f)set_path(start.x + 300.0f, sky);
			else set_path(start.x - 300.0f, ground);
		}
	}	
}

bool dll::EVIL::move(float _ex, float _ey, float gear, BAG<FRECT>& field_obst)
{
	float my_speed = _speed + gear / 10.0f;

	set_path(_ex, _ey);

	if (move_ex > center.x)
	{
		if (move_ey > center.y)dir = dirs::down_right;
		else if (move_ey < center.y)dir = dirs::up_right;
		else dir = dirs::right;
	}
	else if (move_ex < center.x)
	{
		if (move_ey > center.y)dir = dirs::down_left;
		else if (move_ey < center.y)dir = dirs::up_left;
		else dir = dirs::left;
	}
	else
	{
		if (move_ey > center.y)dir = dirs::down;
		else dir = dirs::up;
	}

	if (hor_dir)
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= move_ex)
			{
				start.x += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					set_edges();

					set_path(start.x - 300.0f, move_ey);
				}
			}
			else set_path(start.x - 300.0f, move_ey);
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= move_ex)
			{
				start.x -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					set_edges();
					set_path(start.x + 300.0f, move_ey);
				}
			}
			else set_path(start.x + 300.0f, move_ey);
		}
		else
		{
			if (center.y >= scr_height / 2.0f)set_path(start.x + 300.0f, sky);
			else set_path(start.x - 300.0f, ground);
		}
	}
	else if (ver_dir)
	{
		if (move_ey > move_sy)
		{
			if (end.y + my_speed <= ground)
			{
				start.y += my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y -= my_speed;
					set_edges();
					set_path(start.x, sky);
				}
			}
			else set_path(start.x, sky);
		}
		else if (move_ey < move_sy)
		{
			if (start.y - my_speed >= sky)
			{
				start.y -= my_speed;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.y += my_speed;
					set_edges();
					set_path(start.x, ground);
				}
			}
			else set_path(start.x, ground);
		}
		else
		{
			if (center.x = scr_width / 2.0f)set_path(start.x + 300.0f, sky);
			else set_path(start.x - 300.0f, ground);
		}
	}
	else
	{
		if (move_ex > move_sx)
		{
			if (end.x + my_speed <= move_ex)
			{
				start.x += my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x -= my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
					set_path(start.x - 300.0f, move_ey);
				}
			}
			else set_path(start.x - 300.0f, move_ey);
		}
		else if (move_ex < move_sx)
		{
			if (start.x - my_speed >= move_ex)
			{
				start.x -= my_speed;
				start.y = start.x * slope + intercept;
				set_edges();

				if (obstacle_bumped(field_obst))
				{
					start.x += my_speed;
					start.y = start.x * slope + intercept;
					set_edges();
					set_path(start.x + 300.0f, move_ey);
				}
			}
			else set_path(start.x + 300.0f, move_ey);
		}
		else
		{
			if (center.y >= scr_height / 2.0f)set_path(start.x + 300.0f, sky);
			else set_path(start.x - 300.0f, ground);
		}
	}

	if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= -scr_height || start.y >= 2.0f * scr_height)return false;

	return true;
}

int dll::EVIL::attack()
{
	--attack_delay;
	if (attack_delay <= 0)
	{
		attack_delay = max_attack_delay;
		return damage;
	}
	return 0;
}

void dll::EVIL::Release()
{
	delete this;
}

dll::EVIL* dll::EVIL::create(evils what, float sx, float sy)
{
	EVIL* ret{ nullptr };

	ret = new EVIL(what, sx, sy);

	return ret;
}

////////////////////////////////////////


// FUNCTIONS ****************************************

bool dll::Intersect(FRECT first, FRECT second)
{
	if (!(first.left > second.right || first.right<second.left || first.up>second.down || first.down < second.up))return true;

	return false;
}
bool dll::Intersect(FPOINT first_center, FPOINT second_center, float first_xrad, float second_xrad,
	float first_yrad, float second_yrad)
{
	if (abs(second_center.x - first_center.x) <= first_xrad + second_xrad &&
		abs(second_center.y - first_center.y) <= first_yrad + second_yrad)return true;

	return false;
}

float dll::Distance(FPOINT first, FPOINT second)
{
	float a{ (float)(pow(abs(second.x - first.x), 2)) };
	float b{ (float)(pow(abs(second.y - first.y), 2)) };

	return (float)(sqrt(a + b));
}

void dll::Sort(BAG<FPOINT>& bag, FPOINT ref_point)
{
	if (bag.size() < 2)return;
	else
	{
		bool ok = false;

		while (!ok)
		{
			ok = true;

			for (size_t count = 0; count < bag.size() - 1; ++count)
			{
				if (Distance(bag[count], ref_point) > Distance(bag[count + 1], ref_point))
				{
					FPOINT temp{ bag[count] };

					bag[count] = bag[count + 1];
					bag[count] = temp;
					ok = false;
				}
			}
		}
	}
}

actions dll::AINextMove(EVIL& my_unit, BAG<FPOINT>& ObstBag, BAG<FPOINT>& AssetBag, FPOINT hero_center, FPOINT treasure_center)
{
	actions ret = my_unit.current_action;

	if (!AssetBag.empty())Sort(AssetBag, my_unit.center);

	if (ret == actions::move)
	{
		float targ_x = my_unit.get_target_x();
		float targ_y = my_unit.get_target_y();
		
		if ((my_unit.dir == dirs::left && targ_x >= my_unit.start.x) || (my_unit.dir == dirs::right && targ_x <= my_unit.end.x)
			|| (my_unit.dir == dirs::up && targ_y >= my_unit.start.y) || (my_unit.dir == dirs::down && targ_y <= my_unit.end.y)
			|| (my_unit.dir == dirs::up_left && targ_y >= my_unit.start.y && targ_x >= my_unit.start.x)
			|| (my_unit.dir == dirs::up_right && targ_y >= my_unit.start.y && targ_x <= my_unit.end.x)
			|| (my_unit.dir == dirs::down_left && targ_y <= my_unit.end.y && targ_x >= my_unit.start.x)
			|| (my_unit.dir == dirs::down_right && targ_y <= my_unit.end.y && targ_x <= my_unit.end.x))
			ret = actions::patrol;
	}
	else if (ret == actions::shoot && Distance(my_unit.center, hero_center) > my_unit.attack_range)
	{
		if(Distance(my_unit.center, hero_center) <= my_unit.view_range)
		{
			if (my_unit.lifes >= my_unit.max_lifes / 2)
			{
				my_unit.set_path(hero_center.x, hero_center.y);
				ret = actions::hero_spotted;
			}
			else
			{
				if (hero_center.x > my_unit.center.x) my_unit.set_path(my_unit.center.x - 300.0f, my_unit.get_target_y());
				else if (hero_center.x < my_unit.center.x) my_unit.set_path(my_unit.center.x + 300.0f, my_unit.get_target_y());
				else
				{
					if (hero_center.y > my_unit.center.y) my_unit.set_path(my_unit.center.x, my_unit.get_target_y() - 300.0f);
					else my_unit.set_path(my_unit.center.x, my_unit.get_target_y() + 300.0f);
				}
				ret = actions::move;
			}
		}
		else
		{
			if (my_unit.start.x > scr_width / 2.0f)
			{
				if (my_unit.start.y < scr_height / 2.0f)
				{
					my_unit.dir = dirs::down_left;
					my_unit.set_path(my_unit.start.x - 300.0f, ground);
				}
				else if (my_unit.start.y > scr_height / 2.0f)
				{
					my_unit.dir = dirs::up_left;
					my_unit.set_path(my_unit.start.x - 300.0f, sky);
				}
				else
				{
					my_unit.dir = dirs::left;
					my_unit.set_path(my_unit.start.x - 300.0f, my_unit.start.y);
				}
			}
			else if (my_unit.start.x < scr_width / 2.0f)
			{
				if (my_unit.start.y < scr_height / 2.0f)
				{
					my_unit.dir = dirs::down_right;
					my_unit.set_path(my_unit.start.x + 300.0f, ground);
				}
				else if (my_unit.start.y > scr_height / 2.0f)
				{
					my_unit.dir = dirs::up_right;
					my_unit.set_path(my_unit.start.x - 300.0f, sky);
				}
				else
				{
					my_unit.dir = dirs::right;
					my_unit.set_path(my_unit.start.x + 300.0f, my_unit.start.y);
				}
			}
			else
			{
				if (my_unit.start.y < scr_height / 2.0f)
				{
					my_unit.dir = dirs::down_left;
					my_unit.set_path(my_unit.start.x - 300.0f, ground);
				}
				else
				{
					my_unit.dir = dirs::up_right;
					my_unit.set_path(my_unit.start.x + 300.0f, sky);
				}
			}
		}

		ret = actions::move;
	}
	else if (Distance(my_unit.center, hero_center) <= my_unit.attack_range)ret = actions::shoot;
	else if (Distance(my_unit.center, hero_center) <= my_unit.view_range)
	{
		if (my_unit.lifes >= my_unit.max_lifes / 2)
		{
			my_unit.set_path(hero_center.x, hero_center.y);
			ret = actions::hero_spotted;
		}
		else
		{
			if (hero_center.x > my_unit.center.x) my_unit.set_path(my_unit.center.x - 300.0f, my_unit.get_target_y());
			else if (hero_center.x < my_unit.center.x) my_unit.set_path(my_unit.center.x + 300.0f, my_unit.get_target_y());
			else
			{
				if (hero_center.y > my_unit.center.y) my_unit.set_path(my_unit.center.x, my_unit.get_target_y() - 300.0f);
				else my_unit.set_path(my_unit.center.x, my_unit.get_target_y() + 300.0f);
			}
			ret = actions::move;
		}
	}
	else if (!AssetBag.empty())
	{
		if (Distance(my_unit.center, AssetBag[0]) <= my_unit.view_range)
		{
			my_unit.set_path(AssetBag[0].x, AssetBag[0].y);
			ret = actions::move;
		}
	}

	return ret;
}