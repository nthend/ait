#ifndef HEXLOCATOR_HPP
#define HEXLOCATOR_HPP

#include<functional>

#include<4u/util/const.hpp>
#include<4u/util/op.hpp>
#include<4u/la/vec.hpp>

#include<model/config.hpp>

#define _floor floor

static const vec2 __tile_vertex[6] = {
	vec2(0.0,1.0),
	vec2(S32,0.5),
	vec2(S32,-0.5),
	vec2(0.0,-1.0),
	vec2(-S32,-0.5),
	vec2(-S32,0.5)
};

static const ivec2 __neighbours[6] = {
	ivec2(1,0),
	ivec2(0,1),
	ivec2(-1,1),
	ivec2(-1,0),
	ivec2(0,-1),
	ivec2(1,-1)
};

template <int N>
class HexLocator
{
private:
	static const int
		AX = 1,	      AY = -(3*N + 1), AD = N - (3*N + 1)*(N + 1),
		BX = 3*N + 2, BY = 1,          BD = -(3*N + 2)*(N + 1) + (2*N + 1),
		CX = 3*N + 1, CY = 3*N + 2,    CD = -(3*N + 1)*(N + 1) + (3*N + 2)*(2*N + 1);

private:
	HexLocator() {}
	
public:
	class iterator
    {
    private:
        ivec2 _pos;
        int _end_x;
        inline int _get_end_x(int f_y)
        {
            return N - f_y*(f_y>=0);
        }
        inline int _get_begin_x(int f_y)
        {
            return -N - f_y*(f_y<=0);
        }
    public:
		inline iterator(ivec2 c_pos) :
            _pos(c_pos)
        {
            _end_x = _get_end_x(_pos.y());
        }
		inline void next()
        {
            ++_pos.x();
            if(_pos.x() > _end_x)
            {
                ++_pos.y();
                _pos.x() = _get_begin_x(_pos.y());
                _end_x = _get_end_x(_pos.y());
            }
        }
		inline iterator &operator ++ ()
        {
            next();
            return (*this);
        }
		inline ivec2 pos() const
        {
            return _pos;
        }
        inline ivec2 operator ~() const
        {
            return pos();
        }
		inline bool isEquals(const iterator &f_iter) const
		{
			return f_iter.pos() == _pos;
		}
		inline bool operator == (const iterator &f_iter)
        {
            return isEquals(f_iter);
        }
		inline bool operator != (const iterator &f_iter)
        {
            return !isEquals(f_iter);
        }
	};
	static iterator begin()
    {
		return iterator(ivec2(0,-N));
    }
    static iterator end()
    {
		return iterator(ivec2(-N,N+1));
    }
	static void for_each(std::function<void(const ivec2 &p)> func)
	{
		for(iterator i = begin(); i != end(); ++i)
		{
			func(~i);
		}
	}

public:
	inline static ivec2 getTileByPos(const vec2 &p, double tile_size = config::TILE_SIZE)
    {
		int a = _floor(p.x()/(S32*tile_size));
		int b = _floor((p.x()/S3 + p.y())/tile_size);
		int c = _floor((-p.x()/S3 + p.y())/tile_size);
        return ivec2(_div(a - c + 1, 3),_div(b + c + 2, 3));
    }
	inline static ivec2 getRegionByTile(const ivec2 &p)
    {
        int A = _div(AX*p.x() + AY*p.y(), AD);
        int B = _div(BX*p.x() + BY*p.y(), BD);
        int C = _div(CX*p.x() + CY*p.y(), CD);
        return ivec2(_div(C-B+1,3),_div(A+B+2,3));
    }
	inline static ivec2 getRegionCenterTile(const ivec2 &reg)
    {
		return ivec2(reg.x()*N - reg.y()*(N+1), reg.x()*(N+1) + reg.y()*(2*N+1));
    }
	inline static vec2 getTileCenterPos(const ivec2 &pos, double tile_size = config::TILE_SIZE)
    {
		return vec2(S3*tile_size*(pos.x() + 0.5*pos.y()),1.5*tile_size*pos.y());
    }
	inline static vec2 getVertex(int n, double tile_size = config::TILE_SIZE)
	{
		return __tile_vertex[n]*tile_size;
	}
	inline static void getNeighbours(ivec2 p, std::function<void(const ivec2 &)> h)
	{
		unsigned char mask = 0x3f;
		mask &= ~((p.x() <= -N)*(0x0c));
		mask &= ~((p.x() >=  N)*(0x21));
		mask &= ~((p.y() <= -N)*(0x30));
		mask &= ~((p.y() >=  N)*(0x06));
		mask &= ~((p.x() + p.y() <= -N)*(0x18));
		mask &= ~((p.x() + p.y() >=  N)*(0x03));
		for(int i = 0; i < 6; ++i)
		{
			if(mask&(1<<i))
			{
				h(p + __neighbours[i]);
			}
		}
	}
	inline static bool isInside(const ivec2 &p)
    {
        return _abs(p.x()) <= N && _abs(p.y()) <= N && _abs(p.x() + p.y()) <= N;
    }
};

#endif // HEXLOCATOR_HPP
