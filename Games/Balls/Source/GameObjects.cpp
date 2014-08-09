#include "BallsGame.h"

using namespace Render2D;

//--------------------------------ChargedStaticBall--------------------------------//

void ChargedStaticBall::UpdateForces()
{
	float32x2 force = playerBall.position - position;
	float distance = max(1.0f, force.length() / MAGNETIC_FORCE_UNIT_SIZE);
	if (distance < MAGNETIC_FORCE_MAX_DISTANCE / MAGNETIC_FORCE_UNIT_SIZE)
	{
		interactionStrength = 1.0f / sqr(distance) * charge * playerBall.charge;
		force.normalize();
		force *= MAGNETIC_FORCE_COEF * interactionStrength / sqr(MAGNETIC_FORCE_UNIT_SIZE);
		playerBall.acceleration += force;
	}
	else
		interactionStrength = 0.0f;
}

void ChargedStaticBall::UpdateCollisions()
{
	float32x2 av = position - playerBall.position;
	float32x2 xv = playerBall.translation;
	float translation = playerBall.translation.length();
	if (translation == 0.0f)
		return;

	float a = av.length();
	float centersRadius = radius + playerBall.radius;
	if (a < centersRadius)
	{
		av *= centersRadius / a;
		playerBall.position = position - av;
		playerBall.translation.set(0.0f, 0.0f);
		a = centersRadius;
	}
	xv /= translation;	//normalize
	float cosAlpha = dot(av, xv) / a;
	float b = (centersRadius) / a;
	b = sqr(cosAlpha) + sqr(b) - 1.0f;

	if (b < 0.0f)
		return;

	float x = a * (cosAlpha - sqrtf(b));

	if (x <= 0.0f || x > translation)
		return;

	float32x2 fallVector = xv * x;
	float32x2 nv = fallVector - av;
	float fallReflectLengthRatio = (translation - x) / x;

	float32x2 reflectionVector = (fallVector - nv * dot(nv, fallVector) / dot(nv, nv) * 2.0f);
	//playerBall.position += fallVector - reflectionVector /** (translation - x) / x*/;
	playerBall.position += fallVector;
	playerBall.translation = reflectionVector * (translation - x) / x;
	playerBall.speed = reflectionVector * playerBall.speed.length() / x * BOUNCE_COEF_CHARGED_BALLS;
}

void ChargedStaticBall::Draw()
{
	if (interactionStrength == 0.0f)
		interactionSpriteDisplacement = 0.0f;
	else
	{
		float distance = (position - playerBall.position).length();
		interactionSpriteDisplacement += 1.0f / distance / 20.0f;
		Render::DrawBitmap(interactionStrength > 0.0f ? &Textures::interactionPushSprite : &Textures::interactionPullSprite,
			position, playerBall.position, 0.06f,
			&rectf32(interactionSpriteDisplacement, 0.0f, sqrtf(distance) * 10.0f + interactionSpriteDisplacement, 1.0f),
			/*absval(interactionStrength) * 1.6f*/ /*0.3f / distance*/ 1.0f - (distance - 0.2f) / 0.8f);
	}
	Render::DrawEllipse(position, radius - 0.001f, charge > 0 ? colors::lightRed : colors::skyBlue);
	Render::DrawBitmap(&Textures::ballMask, &rectf32(position.x - radius, position.y - radius, position.x + radius, position.y + radius));
	Render::DrawEllipse(position, radius, colors::black, 0.002f);
}

void ChargedStaticBall::DrawShadows()
{
	Render::DrawEllipse(position, radius, colors::black);
}

void ChargedStaticBall::Dispatch(GameObject::Message message)
{
	switch (message)
	{
	case GameObject::Message::UpdateForces:
		UpdateForces();
		break;

	case GameObject::Message::UpdateCollisions:
		UpdateCollisions();
		break;

	case GameObject::Message::DrawShadows:
		DrawShadows();
		break;

	case GameObject::Message::Draw:
		Draw();
		break;
	}
}