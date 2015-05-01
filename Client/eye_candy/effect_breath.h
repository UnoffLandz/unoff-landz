/*!
 \brief Special effects for dragons or other creatures with breath weapons
 */

#ifndef EFFECT_BREATH_H
#define EFFECT_BREATH_H

// I N C L U D E S ////////////////////////////////////////////////////////////

#include "eye_candy.h"

namespace ec
{

	// C L A S S E S //////////////////////////////////////////////////////////////

	class BreathEffect : public Effect
	{
		public:
			enum BreathType
			{
				FIRE,
				ICE,
				POISON,
				MAGIC,
				LIGHTNING,
				WIND
			};

			BreathEffect(EyeCandy* _base, bool* _dead, Vec3* _pos,
				Vec3* _target, std::vector<ec::Obstruction*>* _obstructions,
				const BreathType _type, const uint16_t _LOD,
				const percent_t _scale);
			~BreathEffect();

			virtual EffectEnum get_type()
			{
				return EC_BREATH;
			}
			;
			bool idle(const uint64_t usec);
			virtual void request_LOD(const float _LOD)
			{
				if (fabs(_LOD - (float)LOD) < 1.0)
					return;
				const uint16_t rounded_LOD = (uint16_t)round(_LOD);
				if (rounded_LOD <= desired_LOD)
					LOD = rounded_LOD;
				else
					LOD = desired_LOD;
				count_scalar = 3000 / LOD;
				size_scalar = scale * std::sqrt(LOD) / sqrt(10.0);
			}
			;
			static uint64_t get_max_end_time()
			{
				return 5000000;
			}
			;
			virtual uint64_t get_expire_time()
			{
				return 5000000 + born;
			}
			;

			ParticleSpawner* spawner;
			ParticleMover* mover;
			Vec3* target;
			percent_t scale;
			BreathType type;
			interval_t count;
			interval_t count_scalar;
			interval_t size_scalar;
	};

	class BreathParticle : public Particle
	{
		public:
			BreathParticle(Effect* _effect, ParticleMover* _mover,
				const Vec3 _pos, const Vec3 _velocity, const coord_t _size,
				const alpha_t _alpha, const color_t red, const color_t green,
#ifdef	NEW_TEXTURES
				const color_t blue, TextureEnum _texture, const uint16_t _LOD,
#else	/* NEW_TEXTURES */
				const color_t blue, Texture* _texture, const uint16_t _LOD,
#endif	/* NEW_TEXTURES */
				const BreathEffect::BreathType _type);
			~BreathParticle()
			{
			}

			virtual bool idle(const uint64_t delta_t);
#ifdef	NEW_TEXTURES
			virtual uint32_t get_texture();
#else	/* NEW_TEXTURES */
			virtual GLuint get_texture(const uint16_t res_index);
#endif	/* NEW_TEXTURES */
			virtual light_t estimate_light_level() const
			{
				return 0.002;
			}
			;

#ifdef	NEW_TEXTURES
			TextureEnum texture;
#else	/* NEW_TEXTURES */
			Texture* texture;
#endif	/* NEW_TEXTURES */
			uint16_t LOD;
			BreathEffect::BreathType type;
	};

	class BreathSmokeParticle : public Particle
	{
		public:
			BreathSmokeParticle(Effect* _effect, ParticleMover* _mover,
				const Vec3 _pos, const Vec3 _velocity, const coord_t _size,
#ifdef	NEW_TEXTURES
				const alpha_t _alpha, TextureEnum _texture, const uint16_t _LOD,
#else	/* NEW_TEXTURES */
				const alpha_t _alpha, Texture* _texture, const uint16_t _LOD,
#endif	/* NEW_TEXTURES */
				const BreathEffect::BreathType _type);
			~BreathSmokeParticle()
			{
			}

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
			; // No glow.
			virtual light_t get_light_level()
			{
				return 0.0;
			}
			; // Same.

#ifdef	NEW_TEXTURES
			TextureEnum texture;
#else	/* NEW_TEXTURES */
			Texture* texture;
#endif	/* NEW_TEXTURES */
			BreathEffect::BreathType type;
	};

///////////////////////////////////////////////////////////////////////////////

} // End namespace ec

#endif	// defined EFFECT_BREATH_H
