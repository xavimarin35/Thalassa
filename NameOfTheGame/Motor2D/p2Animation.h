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

	//pugi::xml_document	animations_file;

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

	SDL_Rect& GetCurrentFrame(float dt)
	{
		if (this)
		{
			current_frame += speed * dt;
			if (current_frame >= last_frame)
			{
				current_frame = (loop) ? 0.0f : last_frame - 1;
				loops++;
			}
			return frames[(int)current_frame];
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
