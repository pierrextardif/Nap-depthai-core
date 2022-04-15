#include "napThread.h"

#ifdef TARGET_ANDROID
#include <jni.h>
#include "ofxAndroidUtils.h"
#endif

//-------------------------------------------------
napThread::napThread()
	:threadRunning(false)
	, threadDone(true)
	, mutexBlocks(true)
	, name("") {
}

//-------------------------------------------------
bool napThread::isThreadRunning() const {
	return threadRunning;
}

//-------------------------------------------------
std::thread::id napThread::getThreadId() const {
	return thread.get_id();
}

//-------------------------------------------------
std::string napThread::getThreadName() const {
	return name;
}

//-------------------------------------------------
void napThread::setThreadName(const std::string& name) {
	this->name = name;
}

//-------------------------------------------------
void napThread::startThread() {
	std::unique_lock<std::mutex> lck(mutex);
	if (threadRunning || !threadDone) {
		return;
	}

	threadDone = false;
	threadRunning = true;
	this->mutexBlocks = true;

	thread = std::thread(std::bind(&napThread::run, this));
}

//-------------------------------------------------
void napThread::startThread(bool mutexBlocks) {
	std::unique_lock<std::mutex> lck(mutex);
	if (threadRunning || !threadDone) {
		return;
	}

	threadDone = false;
	threadRunning = true;
	this->mutexBlocks = mutexBlocks;

	thread = std::thread(std::bind(&napThread::run, this));
}

//-------------------------------------------------
bool napThread::lock() {
	if (mutexBlocks) {
		mutex.lock();
	}
	else {
		if (!mutex.try_lock()) {
			return false; // mutex is locked, tryLock failed
		}
	}
	return true;
}

//-------------------------------------------------
bool napThread::tryLock() {
	return mutex.try_lock();
}

//-------------------------------------------------
void napThread::unlock() {
	mutex.unlock();
}

//-------------------------------------------------
void napThread::stopThread() {
	threadRunning = false;
}

//-------------------------------------------------
void napThread::waitForThread(bool callStopThread, long milliseconds) {
	if (!threadDone) {
		// tell thread to stop
		if (callStopThread) {
			stopThread();
		}

		// wait for the thread to finish
		if (isCurrentThread()) {
			return; // waitForThread should only be called outside thread
		}

		if (INFINITE_JOIN_TIMEOUT == milliseconds) {
			std::unique_lock<std::mutex> lck(mutex);
			if (!threadDone) {
				condition.wait(lck);
			}
		}
		else {
			// Wait for "joinWaitMillis" milliseconds for thread to finish
			std::unique_lock<std::mutex> lck(mutex);
			if (!threadDone && condition.wait_for(lck, std::chrono::milliseconds(milliseconds)) == std::cv_status::timeout) {
				// unable to completely wait for thread
			}
		}
	}
}

//-------------------------------------------------
void napThread::sleep(long milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//-------------------------------------------------
void napThread::yield() {
	std::this_thread::yield();
}

//-------------------------------------------------
bool napThread::isCurrentThread() const {
	return std::this_thread::get_id() == thread.get_id();
}

//-------------------------------------------------
std::thread& napThread::getNativeThread() {
	return thread;
}

//-------------------------------------------------
const std::thread& napThread::getNativeThread() const {
	return thread;
}

//-------------------------------------------------
void napThread::threadedFunction() {
}

//-------------------------------------------------
void napThread::run() {
#ifdef TARGET_ANDROID
	JNIEnv* env;
	jint attachResult = ofGetJavaVMPtr()->AttachCurrentThread(&env, nullptr);
	if (attachResult != 0) {
		ofLogWarning() << "couldn't attach new thread to java vm";
	}
#endif

	// user function
	// should loop endlessly.
	try {
		threadedFunction();
	}
	catch (const std::exception& exc) {
	}
	catch (...) {
	}
	try {
		thread.detach();
	}
	catch (...) {}
#ifdef TARGET_ANDROID
	attachResult = ofGetJavaVMPtr()->DetachCurrentThread();
#endif

	std::unique_lock<std::mutex> lck(mutex);
	threadRunning = false;
	threadDone = true;
	condition.notify_all();
}
