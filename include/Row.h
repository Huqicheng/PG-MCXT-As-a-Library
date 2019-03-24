#ifndef ROW_H
#define ROW_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

typedef unsigned int uint;

/* 跳过一个cell */
static const string em_cell = "";

class Row{
	public:
		Row(uint num, uint lev = 0);

		/**左对齐输出
		 *
		 */
		Row &left()
		{
			r_left = true;
			r_centre = r_right = r_width = false;
			return *this;
		}

		/**居中输出
		 *
		 */
		Row &centre()
		{
			r_centre = true;
			r_left = r_right = r_width = false;
			return *this;
		}

		/**右对齐输出
		 *
		 */
		Row &right()
		{
			r_right = true;
			r_left = r_centre = r_width = false;
			return *this;
		}

		/**初始化每一列宽度的状态
		 *
		 */
		Row &setWidth()
		{
			r_width = true;
			return *this;
		}

		/**设置超过初始化宽度值，后面输出是否忽略
		 * true 表示忽略
		 */
		Row &setIsExceed(bool flag)
		{
			isExceed = flag;
			return *this;
		}

		/**如果输出的值小于初始化的宽度，设置是否用空白补齐
		 * true 表示补齐
		 */
		Row &setIsLack(bool flag)
		{
			isLack = flag;
			return *this;
		}

		/**设置输出的缩进层级
		 *
		 */
		Row &setLevel(uint lev)
		{
			level = lev;
			return *this;
		}

		Row &operator << (uint value);

		Row &operator << (string value);

		Row &operator << (ostream &(*pf)(ostream&));
	private:
		Row(const Row &);
		
	 	Row &operator == (const Row &);

		uint getWidth(uint value);

		inline void splitLine();

		inline string stringSpace(uint num);

		inline string getPad(int level);
	private:
		uint		rowNum;		/*< 一共有几行>*/
		uint		rowRecord;	/*< 当前输出第几行>*/
		vector<uint>	widths;		/*< 宽度列表>*/
		uint		size;		/*< 一行单元数>*/
		uint		column;		/*< 标识输出到第几个单元>*/
		int		level;		/*< 每一行开头的缩进层级>*/
		bool		header;		/*< 是否输出头>*/
		bool		isExceed;	/*< 输入超过size是否截断输出>*/
		bool		isLack;		/*< 输入不够size是否以空格补全>*/
		/* output status */
		bool		r_left;		/*< 左对齐输出>*/
		bool		r_centre;	/*< 居中输出>*/
		bool		r_right;	/*< 右对齐输出>*/
		bool		r_width;	/*< 输入宽度的状态>*/
};

#endif
