/*!
 \brief Special effects for events involving something striking something else,
 or effects that look like that (such as blood or a burst of poison damage)
 */

#ifndef EFFECT_IMPACT_H
#define EFFECT_IMPACT_H

// I N C L U D E S ////////////////////////////////////////////////////////////

#include "eye_candy.h"

namespace ec
{

	// C L A S S E S //////////////////////////////////////////////////////////////

	class ImpactEffect : public Effect
	{
		public:
			enum ImpactType
			{
				MAGIC_PROTECTION,
				SHIELD,
				MAGIC_IMMUNITY,
				POISON,
				BLOOD
			};

			ImpactEffect(EyeCandy* _base, bool* _dead, Vec3* _pos,
				const Vec3 _angle, const ImpactType _type, const uint16_t _LOD,
				const float _strength);
			~ImpactEffect();

			virtual EffectEnum get_type()
			{
				return EC_IMPACT;
			}
			;
			bool idle(const uint64_t usec);
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
			Vec3 center;
			Vec3 angle;
			Vec3 effect_center;
			ImpactType type;
			float strength;
	};

	class ImpactParticle : public Particle
	{
		public:
			ImpactParticle(Effect* _effect, ParticleMover* _mover,
				const Vec3 _pos, const Vec3 _velocity, const coord_t _size,
				const alpha_t _alpha, const color_t red, const color_t green,
#ifdef	NEW_TEXTURES
				const color_t blue, TextureEnum _texture, const uint16_t _LOD,
#else	/* NEW_TEXTURES */
				const color_t blue, Texture* _texture, const uint16_t _LOD,
#endif	/* NEW_TEXTURES */
				const ImpactEffect::ImpactType _type);
			~ImpactParticle()
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
				if ((type == ImpactEffect::BLOOD) || (type
					== ImpactEffect::POISON))
					return 0.0;
				else
					return 0.002;
			}
			;

#ifdef	NEW_TEXTURES
			TextureEnum texture;
#else	/* NEW_TEXTURES */
			Texture* texture;
#endif	/* NEW_TEXTURES */
			uint16_t LOD;
			ImpactEffect::ImpactType type;
	};

///////////////////////////////////////////////////////////////////////////////

} // End namespace ec

#endif	// defined EFFECT_IMPACT_H
