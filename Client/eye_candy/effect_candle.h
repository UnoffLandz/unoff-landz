/*!
 \brief A special effect involving a small, stable flame, useful for candles.
 */

#ifndef EFFECT_CANDLE_H
#define EFFECT_CANDLE_H

// I N C L U D E S ////////////////////////////////////////////////////////////

#include "eye_candy.h"

namespace ec
{

	// C L A S S E S //////////////////////////////////////////////////////////////

	class CandleEffect : public Effect
	{
		public:
			CandleEffect(EyeCandy* _base, bool* _dead, Vec3* _pos,
				const color_t _hue_adjust, const color_t _saturation_adjust,
				const float scale, const uint16_t _LOD);
			~CandleEffect();

			virtual EffectEnum get_type()
			{
				return EC_CANDLE;
			}
			;
			bool idle(const uint64_t usec);

			GradientMover* mover;
			ParticleSpawner* spawner;
			color_t hue_adjust;
			color_t saturation_adjust;
			float scale;
			float sqrt_scale;
	};

	class CandleParticle : public Particle
	{
		public:
			CandleParticle(Effect* _effect, ParticleMover* _mover,
				const Vec3 _pos, const Vec3 _velocity,
				const color_t hue_adjust, const color_t saturation_adjust,
				const float _scale, const uint16_t _LOD);
			~CandleParticle()
			{
			}
			;

			virtual bool idle(const uint64_t delta_t);
#ifdef	NEW_TEXTURES
			virtual uint32_t get_texture();
			virtual float get_burn() const;
#else	/* NEW_TEXTURES */
			virtual GLuint get_texture(const uint16_t res_index);
			virtual void draw(const uint64_t usec);
#endif	/* NEW_TEXTURES */
			virtual light_t estimate_light_level() const
			{
				return 0.0;
			}
			; // Like above
			virtual light_t get_light_level()
			{
				return 0.0;
			}
			; // Same.

			uint16_t LOD;
	};

///////////////////////////////////////////////////////////////////////////////

} // End namespace ec

#endif	// defined EFFECT_CANDLE_H
