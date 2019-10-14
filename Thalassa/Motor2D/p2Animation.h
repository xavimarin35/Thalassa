#ifndef __P2ANIMATION_H__
#define __P2ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include <string>
#define MAX_FRAMES 200

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
	bool flip = false;

private:
	float current_frame;
	int last_frame = 0;
	int loops = 0;

	pugi::xml_document	anim_file;

public:

	Animation::Animation() {}

	Animation(const Animation& anim) : loop(anim.loop), speed(anim.speed), last_frame(anim.last_frame)
	{
		SDL_memcpy(&frames, anim.frames, sizeof(frames));
	}

	void PushBack(const SDL_Rect& rect)
	{
		frames[last_frame++] = rect;
	}

	SDL_Rect& GetCurrentFrame()
	{
		if (this)
		{
			current_frame += speed;
			if (current_frame >= last_frame)
			{
				current_frame = (loop) ? 0.0f : last_frame - 1;
				loops++;
			}
			return frames[(int)current_frame];
		}
	}

	void LoadAnimations(p2SString name)
	{
		pugi::xml_parse_result result = anim_file.load_file("config_animations.xml");
		if (result != NULL)
		{
			pugi::xml_node animation_name = anim_file.child("animations").child(name.GetString());
			loop = animation_name.attribute("loop").as_bool();
			speed = animation_name.attribute("speed").as_float();
			for (pugi::xml_node animation = animation_name.child("animation"); animation; animation = animation.next_sibling("animation"))
			{
				PushBack({ animation.attribute("x").as_int(), animation.attribute("y").as_int(), animation.attribute("w").as_int(), animation.attribute("h").as_int() });
			}
		}
	}

	float Getframe_pos() {
		return current_frame;
	}

	bool isLastFrame() {
		return (int)current_frame >= last_frame - 1;
	}

	bool equal(Animation* anim) {
		return (this == anim);
	}

	bool Finished()
	{
		if (loops > 0) {
			return true;
		}
		else
			return false;
	}

	void Reset()
	{
		current_frame = 0;
		loops = 0;
	}
};

#endif // __P2ANIMATION_H__
