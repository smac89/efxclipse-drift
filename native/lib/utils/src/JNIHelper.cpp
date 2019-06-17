/*
 * Copyright (c) 2019 BestSolution.at and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Christoph Caks <ccaks@bestsolution.at> - initial API and implementation
 */


#include <jni.h>

#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <utils/Logger.h>
using namespace std;

#include <string.h>

#include <utils/JNIHelper.h>

JavaVM* JNIHelper::jvm = nullptr;
thread_local JNIEnv* jniEnv;

void JNIHelper::Initialize(JavaVM* jvm) {
	LogDebug("jvm = " << jvm)
	if (JNIHelper::jvm != nullptr) {LogWarning("JNIHelper already initialized") return;}
	JNIHelper::jvm = jvm;
}

bool JNIHelper::IsThreadAttached() {
	if (jvm == nullptr) {LogError("JNIHelper not initialized") throw;}
	if (jniEnv == nullptr) {
		jint stat = jvm->GetEnv((void**)&jniEnv, JNI_VERSION_1_6);
		LogDebug("stat " << stat)
		if (stat == JNI_EDETACHED) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return true;
	}
}

void JNIHelper::AttachThread() {
	if (jvm == nullptr) {LogError("JNIHelper not initialized") throw;}
	if (jniEnv == nullptr) {
		jint stat = jvm->GetEnv((void**)&jniEnv, JNI_VERSION_1_6);
		if (stat == JNI_EDETACHED) {
			// attach
			ostringstream os;
			os << "C++ Thread id: " << this_thread::get_id();
			JavaVMAttachArgs args;
			args.version = JNI_VERSION_1_6;
			args.name = strdup(os.str().c_str());
			args.group = NULL;
			jint attachResult = jvm->AttachCurrentThreadAsDaemon((void**)&jniEnv, &args);
			if (JNI_OK != attachResult) {
				throw "Could not attach thread to jvm!";
			}
			else {
				LogInfo("Thread '" << os.str() << "' sucessfully attached to JVM.")
			}
		}
	}
}

void JNIHelper::ReleaseThread() {
	if (jvm == nullptr) {LogError("JNIHelper not initialized") throw;}
	if (JNI_OK == jvm->DetachCurrentThread()) {
		jniEnv = nullptr;
	}
}

JNIEnv* JNIHelper::GetJNIEnv(bool autoAttach) {
	if (jvm == nullptr) {LogError("JNIHelper not initialized") throw;}
	if (autoAttach && !IsThreadAttached()) {
		LogInfo("Thread " << this_thread::get_id() << " was automatically attached to JVM!")
		AttachThread();
	}
	return jniEnv;
}



