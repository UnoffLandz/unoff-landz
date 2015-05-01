/*!
 \brief Special effects for magic cast on one's self
 */

#ifndef EFFECT_SELFMAGIC_H
#define EFFECT_SELFMAGIC_H

// I N C L U D E S ////////////////////////////////////////////////////////////

#include "eye_candy.h"

namespace ec
{

	// C L A S S E S //////////////////////////////////////////////////////////////

	class SelfMagicEffect : public Effect
	{
		public:
			enum SelfMagicType
			{
				HEAL,
				MAGIC_PROTECTION,
				SHIELD,
				HEATSHIELD,
				COLDSHIELD,
				RADIATIONSHIELD,
				RESTORATION,
				BONES_TO_GOLD,
				TELEPORT_TO_THE_PORTALS_ROOM,
				MAGIC_IMMUNITY,
				ALERT // Not really a spell, but functions like one.
			};

			SelfMagicEffect(EyeCandy* _base, bool* _dead, Vec3* _pos,
				const SelfMagicType _type, const uint16_t _LOD);
			~SelfMagicEffect();

			virtual EffectEnum get_type()
			{
				return EC_SELFMAGIC;
			}
			;
			bool idle(const uint64_t usec);
			void draw(const uint64_t usec);
			static uint64_t get_max_end_time()
			{
				return 4000000;
			}
			;
			virtual uint64_t get_expire_time()
			{
				return 4000000 + born;
			}
			;

			ParticleSpawner* spawner;
			ParticleMover* mover;
			ParticleSpawner* spawner2;
			ParticleMover* mover2;
			Vec3 effect_center;
			Vec3 shift;
			Sint64 count;
			uint64_t count_scalar;
			SelfMagicType type;
#ifdef	NEW_TEXTURES
			CaplessCylinders* capless_cylinders;
			float alpha_scale;
#else	/* NEW_TEXTURES */
			std::vector<Shape*> capless_cylinders;
#endif	/* NEW_TEXTURES */
			coord_t size_scalar;
			float* target_alpha;
	};

	class SelfMagicParticle : public Particle
	{
		public:
			SelfMagicParticle(Effect* _effect, ParticleMover* _mover,
				const Vec3 _pos, const Vec3 _velocity, const coord_t _size,
				const alpha_t _alpha, const color_t red, const color_t green,
#ifdef	NEW_TEXTURES
				const color_t blue, TextureEnum _texture, const uint16_t _LOD,
#else	/* NEW_TEXTURES */
				const color_t blue, Texture* _texture, const uint16_t _LOD,
#endif	/* NEW_TEXTURES */
				const SelfMagicEffect::SelfMagicType _type);
			~SelfMagicParticle()
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
			SelfMagicEffect::SelfMagicType type;
	};

///////////////////////////////////////////////////////////////////////////////

} // End namespace ec

#endif	// defined EFFECT_SELFMAGIC_H
