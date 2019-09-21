#ifndef ALGDATASTRUCT_H
#define ALGDATASTRUCT_H

#include <iostream>
#include <numeric>
#include <vector>
#include <memory>
#include <algorithm>

#include "CircleBuffer.h"
#include "Threads.h"


namespace MyAlg
{
	using std::string;
	using std::shared_ptr;
	
	//最底层数据结构，(x,y)
	//采用对象锁的话，只有在访问到该对象时线程会锁住
	//采用类锁，调用相应函数即锁住
	//不用考虑多线程时，用class lowest_level_pt : public SingleThreaded<lowest_level_pt>
	class lowest_level_pt : public ClassLevelLockable<lowest_level_pt>
	{
	public:
		//数据的状态
		enum pt_state : unsigned char
		{
			UNINITIALIZED,	//表示未赋值过
			USED,			//表示正在使用的数据
			DISCARD			//表示不需要的数据
		};

	public:
		//初始化
		inline lowest_level_pt() : x_(0), y_(0), pt_state_(UNINITIALIZED) {}
		inline lowest_level_pt(const unsigned int x, const unsigned char y, const pt_state state)
		{
			x_ = x;
			y_ = y;
			pt_state_ = state;
		}
		inline lowest_level_pt(const lowest_level_pt & t) : x_(t.x_), y_(t.y_), pt_state_(t.pt_state_) {}
		inline lowest_level_pt& operator=(const lowest_level_pt & data)
		{
			Lock lock();
			x_ = data.x_;
			y_ = data.y_;
			pt_state_ = data.pt_state_;
			return *this;
		}
		//类中没有用到指针，可以不用定义移动构造、移动赋值
		//下面移动构造、移动赋值仅作为备用
		inline lowest_level_pt(lowest_level_pt && t) : x_(t.x_), y_(t.y_), pt_state_(t.pt_state_) 
		{}
		inline lowest_level_pt& operator=(lowest_level_pt && data)
		{
			x_ = data.x_;
			y_ = data.y_;
			pt_state_ = data.pt_state_;
			return *this;
		}
		inline void Set(const unsigned int x, const unsigned char y, const pt_state state)
		{
			Lock lock();
			x_ = x;
			y_ = y;
			pt_state_ = state;
		}
		inline unsigned int GetX() const
		{
			Lock lock();
			return x_;
		}
		inline unsigned char GetY() const
		{
			Lock lock();
			return y_;
		}
		inline pt_state GetState() const
		{
			Lock lock();
			return pt_state_;
		}
		inline void SetState(lowest_level_pt::pt_state state)
		{
			Lock lock();
			pt_state_ = state;
		}

	private:
		unsigned int x_;	//2字节
		unsigned char y_;	//1字节
		pt_state pt_state_;	//1字节
		//基类有4字节
	};

	//用循环buffer封装底层数据
	typedef CircleBuffer<lowest_level_pt> circle_buffer;



}
#endif