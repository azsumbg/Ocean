#pragma once

#ifdef OCEAN_EXPORTS
#define OCEAN_API __declspec(dllexport)
#else
#define OCEAN_API __declspec(dllimport)
#endif

#include <random>

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr int BAG_BAD_PTR{ 11001 };
constexpr int BAG_BAD_INDEX{ 11002 };
constexpr int BAG_BAD_PARAM{ 11003 };
constexpr int BAG_UNKNOWN_ERR{ 11004 };

constexpr float nature_speed{ 1.5f };

enum class dirs { left = 0, right = 1, up = 2, down = 3, up_left = 4, up_right = 5, down_left = 6, down_right = 7, stop = 8 };
enum class obstacles { big_rock = 0, mid_rock = 1, small_rock = 2, island = 3, swirl = 4, port = 5 };
enum class evils { evil1 = 0, evil2 = 1, evil3 = 2, evil4 = 3 };
enum class bonus { gold = 0, gun = 1, armor = 2 };

enum class actions { stop = 0, move = 1, patrol = 2, shoot = 3, release_boat = 4, hero_spotted = 5 };

dirs nature_dir{ dirs::stop };

struct OCEAN_API FPOINT
{
	float x{ 0 };
	float y{ 0 };
};
struct OCEAN_API FRECT
{
	float left{ 0 };
	float up{ 0 };
	float right{ 0 };
	float down{ 0 };
};

struct OCEAN_API FADE_BONUS
{
	bonus type{ bonus::gold };

	float sx{ 0 };
	float sy{ 0 };

	float opacity = 1.0f;
	int delay = 8;

	float get_opacity()
	{
		--delay;
		if (delay <= 0)
		{
			opacity -= 0.1f;
			delay = 8;
		}
		if (opacity <= 0)opacity = 0;
		return opacity;
	}
};

struct OCEAN_API EXPLOSION
{
	int frame = 0;
	int max_frames = 23;
	int frame_delay = 3;

	int get_frame()
	{
		--frame_delay;
		if(frame_delay <= 0)
		{
			frame_delay = 3;
			++frame;
			if (frame > max_frames)frame = -1;
		}

		return frame; // -1 - EXPLOSION ENDED
	}
};

namespace dll
{
	// FUNCTIONS ****************************************

	bool OCEAN_API Intersect(FRECT first, FRECT second);
	bool OCEAN_API Intersect(FPOINT first_center, FPOINT second_center, float first_xrad, float second_xrad,
		float first_yrad, float second_yrad);

	float OCEAN_API Distance(FPOINT first, FPOINT second);


	/////////////////////////////////////////////////////////

	// CLASSES *********************************************

	class OCEAN_API EXCEPTION
	{
	private:
		int _what = 0;

	public:
		EXCEPTION(int _what_happened);

		const wchar_t* messsage();
	};

	class OCEAN_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RANDIT();
		~RANDIT();

		int operator ()(int min, int max);
		float operator ()(float min, float max);
	};

	template<typename T>class BAG
	{
	private:
		size_t max_size{ 0 };
		T* mPtr{ nullptr };
		size_t next_pos{ 0 };

	public:
		BAG() :max_size{ 1 }, mPtr{ reinterpret_cast<T*>(calloc(1,sizeof(T))) } {};
		BAG(size_t lenght)
		{
			if (lenght < 0)throw EXCEPTION(BAG_BAD_PARAM);
			else if (lenght < 2)
			{
				max_size = 1;
				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			}
			else
			{
				max_size = lenght;
				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			}
		}
		BAG(BAG& other)
		{
			if (this->mPtr != other.mPtr)
			{
				if (other.mPtr == nullptr)throw EXCEPTION(BAG_BAD_PTR);

				this->max_size = other.max_size;
				this->next_pos = other.next_pos;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
				else if (next_pos > 0)for (size_t i = 0; i < next_pos; ++i)this->mPtr[i] = other.mPtr[i];
				
			}
		}
		BAG(BAG&& other)
		{
			if (other.mPtr == nullptr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				this->mPtr = other.mPtr;
				this->max_size = other.max_size;
				this->next_pos = other.next_pos;

				other.mPtr = nullptr;
			}
		}

		~BAG()
		{
			free(mPtr);
		}

		bool empty() const
		{
			return (next_pos == 0);
		}
		size_t size() const
		{
			return next_pos;
		}
		size_t capacity() const
		{
			return max_size;
		}

		T& front() const
		{
			return *mPtr;
		}
		T& back() const
		{
			return mPtr[next_pos - 1];
		}

		BAG& operator = (BAG& other)
		{
			if (!other.mPtr)throw EXCEPTION(BAG_BAD_PARAM);
			else
			{
				free(this->mPtr);

				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
				else if (next_pos > 0)for (size_t i = 0; i < next_pos; ++i)mPtr[i] = other.mPtr[i];
			}

			return *this;
		}
		BAG& operator = (BAG&& other)
		{
			if (!other.mPtr)throw EXCEPTION(BAG_BAD_PTR);

			free(mPtr);

			this->next_pos = other.next_pos;
			this->max_size = other.max_size;

			this->mPtr = other.mPtr;

			other.mPtr = nullptr;

			return *this;
		}

		T& operator [] (size_t index)
		{
			if (index < 0)throw EXCEPTION(BAG_BAD_PARAM);

			if (mPtr == nullptr)throw EXCEPTION(BAG_BAD_PTR);

			if (index >= next_pos)return mPtr[next_pos - 1];

			return mPtr[index];
		}

		void clear()
		{
			free(mPtr);
			next_pos = 0;
			max_size = 1;

			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}

		void push_back(T element)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = element;
					++next_pos;
				}
				else
				{
					++max_size;
					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
					else
					{
						mPtr[next_pos] = element;
						++next_pos;
					}
				}
			}
		}
		void push_back(T* element)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = *element;
					++next_pos;
				}
				else
				{
					++max_size;
					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
					else
					{
						mPtr[next_pos] = *element;
						++next_pos;
					}
				}
			}
		}

		void push_front(T element)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (next_pos == 0)
				{
					*mPtr = element;
					++next_pos;
				}
				else if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
					*mPtr = element;
					++next_pos;
				}
				else
				{
					++max_size;
					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
					*mPtr = element;
					++next_pos;
				}
			}
		}
		void push_front(T* element)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (next_pos == 0)
				{
					*mPtr = *element;
					++next_pos;
				}
				else if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
					*mPtr = *element;
					++next_pos;
				}
				else
				{
					++max_size;
					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
					*mPtr = *element;
					++next_pos;
				}
			}
		}

		void erase(size_t index)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (index < 0 || index >= next_pos)throw EXCEPTION(BAG_BAD_PARAM);

				for (size_t count = index; count < next_pos - 1; ++count)mPtr[count] = mPtr[count + 1];

				--next_pos;
			}
		}

		void insert(size_t index, T element)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (index < 0 || index >= next_pos)throw EXCEPTION(BAG_BAD_INDEX);
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];
						mPtr[index] = element;
						++next_pos;
					}
					else
					{
						++max_size;
						mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
						if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
						else
						{
							for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];
							mPtr[index] = element;
							++next_pos;
						}
					}
				}
			}
		}
		void insert(size_t index, T* element)
		{
			if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
			else
			{
				if (index < 0 || index >= next_pos)throw EXCEPTION(BAG_BAD_INDEX);
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];
						mPtr[index] = *element;
						++next_pos;
					}
					else
					{
						++max_size;
						mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
						if (!mPtr)throw EXCEPTION(BAG_BAD_PTR);
						else
						{
							for (size_t count = next_pos; count > index; --count)mPtr[count] = mPtr[count - 1];
							mPtr[index] = *element;
							++next_pos;
						}
					}
				}
			}
		}
	};

	class OCEAN_API PROTON
	{
	protected:
		float _width{ 0 };
		float _height{ 0 };

		float move_sx{ 0 };
		float move_sy{ 0 };
		float move_ex{ 0 };
		float move_ey{ 0 };

		bool hor_dir{ false };
		bool ver_dir{ false };

		float slope{ 0 };
		float intercept{ 0 };

	public:
		FPOINT start{};
		FPOINT end{};
		FPOINT center{};

		float x_rad{ 0 };
		float y_rad{ 0 };

		dirs dir{ dirs::stop };

		PROTON();
		PROTON(float _sx, float _sy);
		PROTON(float _sx, float _sy, float _swidth, float _sheight);

		virtual ~PROTON() {};

		void set_edges();
		void set_path(float _ex, float _ey);

		void set_width(float _new_width);
		void set_height(float _new_height);
		void new_dims(float _new_width, float _new_height);

		float get_width()const;
		float get_height()const;

		float get_target_x()const;
		float get_target_y()const;
	};

	class OCEAN_API FIELD
	{
	private:
		float _speed = nature_speed;

		FRECT _ViewPort{ 0, sky, scr_width, scr_height };

	public:
		FRECT ocean_tiles[3][3]{};

		FIELD();

		void move_ocean(dirs to_where, float gear);

		bool in_view_port(int row, int col)const;
		bool in_view_port(FRECT what)const;
	};

	class OCEAN_API HERO :public PROTON
	{
	private:
		float _speed{ 2.0f };

		int attack_delay = 40;
		int max_attack_delay = 40;

		int frame = 0;
		int max_frames = 6;
		int frame_delay = 12;
		int max_frame_delay = 12;

		HERO(float _sx, float _sy);

	public:
		int lifes = 200;
		int damage = 10;
		int armor = 5;

		bool obstacle_bumped(BAG<FRECT>& obstacles);

		void move(float ex, float ey, float gear, BAG<FRECT>& field_obst);

		int attack();
		int get_frame();

		void Release();

		static HERO* create(float sx, float sy);
	};

	class OCEAN_API OBSTACLE : public PROTON
	{
	private:
		float _speed = nature_speed;
		FRECT _ViewPort{ 0, sky, scr_width, scr_height };

		int frame = 0;
		int frame_delay = 6;
		int max_frames = 11;

		OBSTACLE(obstacles _what, float _sx, float _sy);

	public:
		obstacles type{ obstacles::small_rock };
		FRECT my_rect{};
		
		void move(dirs to_where, float gear);

		int get_frame();

		bool in_view_port(FRECT what)const;

		void Release();

		static OBSTACLE* create(obstacles what, float sx, float sy);
	};

	class OCEAN_API BOAT :public PROTON
	{
	private:
		float _speed{ 1.5f };

		BOAT(float _sx, float _sy, bool _hero);

	public:
		bool hero_boat{ false };
		int lifes = 50;

		bool obstacle_bumped(BAG<FRECT>& obstacles);

		void Release();

		void move(float ex, float ey, float gear, BAG<FRECT>& field_obst);

		static BOAT* create(float sx, float sy, bool hero = false);
	};

	class OCEAN_API SHOT :public PROTON
	{
	private:
		float _speed{ 5.0f };

		SHOT(float _sx, float _sy, float _ex, float _ey);

	public:
		int damage{ 40 };

		bool move(float gear);

		void Release();

		static SHOT* create(float sx, float sy, float ex, float ey);
	};

	class OCEAN_API EVIL:public PROTON
	{
	private:
		float _speed{ 0.8f };

		int frame{ 0 };
		int max_frames{ 0 };
		int frame_delay{ 0 };
		int max_frame_delay{ 0 };

		int attack_delay{ 0 };
		int max_attack_delay{ 0 };

		EVIL(evils _what, float _sx, float _sy);

	public:
		evils type{ evils::evil1 };
		actions current_action{ actions::patrol };

		int lifes{ 0 };
		int max_lifes{ 0 };

		int damage{ 0 };
		int armor{ 0 };

		float view_range{ 0 };
		float attack_range{ 0 };

		bool obstacle_bumped(BAG<FRECT>& obstacles);

		void patrol(float gear, BAG<FRECT>& field_obst);
		bool move(float _ex, float _ey, float gear, BAG<FRECT>& field_obst);

		int attack();

		void Release();

		static EVIL* create(evils what, float sx, float sy);
	};

	/////////////////////////////////////////////////////////

	// SORT AND AI FUNCTIONS *************************************

	void OCEAN_API Sort(BAG<FPOINT>& bag, FPOINT ref_point);

	actions OCEAN_API AINextMove(EVIL& my_unit, BAG<FPOINT>& ObstBag, BAG<FPOINT>& AssetBag, FPOINT hero_center, FPOINT treasure_center);
}