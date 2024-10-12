#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <AL\al.h>
#include <AL\alc.h>

#include <sndfile.h>
#include <inttypes.h>
#include <AL\alext.h>

#include <queue>


// Helper function to check for OpenAL errors
inline void checkALError(const std::string& message) {
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "OpenAL Error (" << message << "): " << alGetString(error) << std::endl;
	}
}

inline ALuint addSoundEffect(const char* filename)
{

	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audio file and check that it's usable. */
	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile)
	{
		fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
	{
		fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
		sf_close(sndfile);
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format)
	{
		fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	/* Decode the whole audio file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1)
	{
		free(membuf);
		sf_close(sndfile);
		fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
		return 0;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	return buffer;
}

namespace {
	// Static variables scoped inside the AudioManager namespace
	static std::thread audioThread;
	static std::atomic<bool> isRunning(false);  // Use atomic for thread-safe flag
	static std::mutex audioMutex;               // Protect shared resources (if needed)


	static std::queue<ALuint> taskQueue;
	static std::vector<ALuint> activeSources;  // Store currently playing sources
	static std::condition_variable condition;

	static ALCdevice* device = nullptr;
	static ALCcontext* context = nullptr;
}

namespace AudioManager {

	


	// Main function for the audio thread
	inline void mainAudioThread() {
		std::cout << "Audio thread started." << std::endl;

		// Keep processing audio while running
		while (isRunning) {
			{	
				// If not running, exit the loop
				if (!isRunning) break;

				{
					std::unique_lock<std::mutex> lock(audioMutex);
					condition.wait(lock, [&] { return !taskQueue.empty() || !isRunning; });
				}



				// Process all items in the queue
				while (!taskQueue.empty()) {
					ALuint buffer = 0;
					{
						std::lock_guard<std::mutex> lock(audioMutex);
						buffer = taskQueue.front();
						taskQueue.pop();
					}

					// Generate a source and assign the buffer to play
					ALuint source;
					alGenSources(1, &source);
					alSourcei(source, AL_BUFFER, buffer);
					alSourcePlay(source);

					// Add source to active sources
					activeSources.push_back(source);
				}

			}
		}

		std::cout << "Audio thread stopped." << std::endl;
	}





	// Function to stop the audio manager
	inline void stop() {
		// Signal the thread to stop
		{
			std::lock_guard<std::mutex> lock(audioMutex);
			isRunning = false;
			condition.notify_all();  // Wake up the audio thread if it's waiting
		}

		// Join the audio thread to ensure it finishes execution
		if (audioThread.joinable()) {
			audioThread.join();
		}

		// Clean up OpenAL sources and buffers
		{
			std::lock_guard<std::mutex> lock(audioMutex);

			// Stop and delete all active sources
			for (ALuint source : activeSources) {
				alSourceStop(source);
				alDeleteSources(1, &source);
			}
			activeSources.clear();

			// Cleanup OpenAL context and device
			if (context) {
				alcMakeContextCurrent(nullptr);  // Unset current context
				alcDestroyContext(context);
				context = nullptr;
			}

			if (device) {
				alcCloseDevice(device);
				device = nullptr;
			}
		}

		std::cout << "Audio Manager stopped and cleaned up." << std::endl;
	}

	// Function to initialize and start the audio thread
	inline void initAudioThread() {
		std::lock_guard<std::mutex> lock(audioMutex);
		if (!isRunning) {
			isRunning = true;


			device = alcOpenDevice(nullptr); // Open default device
			if (device) {
				context = alcCreateContext(device, nullptr);
				alcMakeContextCurrent(context);
			}


			audioThread = std::thread(mainAudioThread);
		}
	}


	// Add a sound to the play queue (thread-safe)
	inline void playSound(const std::string& filename) {

		if (!isRunning) { initAudioThread(); } //If thread has not been started, start it

		ALuint buffer = addSoundEffect(filename.c_str());
		std::lock_guard<std::mutex> lock(audioMutex);
		taskQueue.push(buffer);
		condition.notify_all();  // Notify the audio thread that there's work to do
	}


	// Check active sources and remove those that have finished playing
	inline void updateActiveSources() {
		std::lock_guard<std::mutex> lock(audioMutex);

		// Iterate over the active sources
		for (auto it = activeSources.begin(); it != activeSources.end(); /* no increment */) {
			ALint state;
			alGetSourcei(*it, AL_SOURCE_STATE, &state);

			if (state != AL_PLAYING) {
				// Source is done playing, delete it and remove from active list
				alDeleteSources(1, &(*it));
				it = activeSources.erase(it); // Erase returns the next iterator
			}
			else {
				++it;
			}
		}
	}

}
