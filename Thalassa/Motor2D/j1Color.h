#ifndef _j1COLOR_H_
#define _j1COLOR_H_

struct j1Color
{
	float r, g, b, a;

	j1Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{}

	j1Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
	{}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}
};

extern j1Color Black;
extern j1Color Grey;
extern j1Color White;
extern j1Color Red;
extern j1Color Green;
extern j1Color Blue;

#endif