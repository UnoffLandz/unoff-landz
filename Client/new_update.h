/*!
 * \file
 * \ingroup misc
 * \brief new file update functions
 */
#ifndef UUID_092c4555_9096_481e_a41e_17900dc0341e
#define UUID_092c4555_9096_481e_a41e_17900dc0341e

#include <SDL.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * \ingroup	update
 * \brief	Update progress function pointer
 *
 * 		Update progress function pointer
 * \param	str The string with the current activity.
 * \param	max Max value to reach. Can be zero to show that it is
 *		currently unknown.
 * \param	current Current progress.
 * \param	user_data The user data given to update.
 * \retval	uint32_t A return value of other than one cancels the update.
 */
typedef uint32_t (*progress_fnc) (const char* str, const uint32_t max,
	const uint32_t current, void* user_data);

/*!
 * \ingroup	update
 * \brief	Update function
 *
 * 		The update function.
 * \param	server The server to update from.
 * \param	file The name of the file that holds the list of updates
 * \param	dir The directory on the server to use.
 * \param	zip The name of the zip file that should be updated.
 * \param	update_progress_function The progress function to use.
 * \param	user_data The user data for the progress function.
 * \retval	uint32_t A return value of other than one means an error.
 */
uint32_t update(const char* server, const char* file, const char* dir,
	const char* zip, progress_fnc update_progress_function,
	void* user_data);

#ifdef __cplusplus
}
#endif

#endif	/* UUID_092c4555_9096_481e_a41e_17900dc0341e */

