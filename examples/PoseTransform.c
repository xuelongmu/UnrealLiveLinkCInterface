/**
 * Copyright (c) 2020 Patrick Palmer, The Jim Henson Company.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "UnrealLiveLinkCInterfaceAPI.h"
#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <time.h>
#include <math.h>

 /* number of circles to perform */
#define CIRCLES 30

/* number of steps to perform the circles */
#define STEP_COUNT 100000


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BONE_COUNT 23
#define PROP_COUNT 2


int main()
{
	int i;
	int rc;
	time_t t;
	double angle = 0.0;
	double worldTime = 0.0;

#ifdef WIN32
	const char* sharedObj = "UnrealLiveLinkCInterface.dll";
#else
	const char* sharedObj = "UnrealLiveLinkCInterface.so";
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 16 * 1000000;
#endif

	rc = UnrealLiveLink_Load(sharedObj, "PoseTransform");
	if (rc != UNREAL_LIVE_LINK_OK)
	{
		printf("error: unable to load %s\n", sharedObj);
		return 1;
	}

	printf("Starting...\n");
	t = time(NULL);

	/* set up transform live link role */

	// Properties do not seem to work. Not sure whether this is an error in the C interface,
	// or a limitation in Unreal's Live Link implementation.
	//struct UnrealLiveLink_Properties properties;
	//UnrealLiveLink_Name names[BONE_COUNT] = { "testA" }; // "testB", "testC" 
	//properties.names = names;
	//properties.nameCount = BONE_COUNT;

	struct UnrealLiveLink_AnimationStatic structure;

	// To make changing this easier, place the following line on a line number ending with 0.
	struct UnrealLiveLink_Bone bones[BONE_COUNT] = {
		{"root", -1},
		{"lhipjoint", 0},
		{"lfemur", 1},
		{"ltibia", 2},
		{"lfoot", 3},
		{"rhipjoint", 0},
		{"rfemur", 5},
		{"rtibia", 6},
		{"rfoot", 7},
		{"lowerback", 0},
		{"upperback", 9},
		{"thorax", 10},
		{"lowerneck", 11},
		{"upperneck", 12},
		{"head", 13},
		{"lclavicle", 11},
		{"lhumerus", 15},
		{"lradius", 16},
		{"lwrist", 17},
		{"rclavicle", 11},
		{"rhumerus", 19},
		{"rradius", 20},
		{"rwrist", 21},
	};
	structure.bones = bones;
	structure.boneCount = BONE_COUNT;

	UnrealLiveLink_SetAnimationStructure("TestPose", 0, &structure);

	/* initialize the per frame animation structure */
	struct UnrealLiveLink_Animation anim_frame;
	struct UnrealLiveLink_Transform bone_xforms[BONE_COUNT];
	anim_frame.transforms = bone_xforms;
	anim_frame.transformCount = BONE_COUNT;

	//float values[PROP_COUNT] = { 0.0, 0.0 };
	//struct UnrealLiveLink_PropertyValues prop_values = { values, PROP_COUNT };

	/* calculate the radian step */
	const double step = 2.0 * M_PI * CIRCLES / STEP_COUNT;

	/* loop STEP_COUNT times sending data every 16ms (not exact) */
	for (i = 0; i < STEP_COUNT; i++)
	{
		for (int i = 0; i < BONE_COUNT; i++) {
			UnrealLiveLink_InitTransform(&bone_xforms[i]);

			if (i == 0) {
				bone_xforms[i].translation[0] = (float)sin(angle)*1000;
				bone_xforms[i].translation[1] = 0.0;
				bone_xforms[i].translation[2] = 0.0;
			}

			bone_xforms[i].rotation[0] = (float)sin(angle);
			bone_xforms[i].rotation[1] = (float)cos(angle);
			bone_xforms[i].rotation[2] = 0.0;
			bone_xforms[i].rotation[3] = 0.0;
		}

		UnrealLiveLink_UpdateAnimationFrame("TestPose", worldTime, 0, 0, &anim_frame);

		angle += step;

		/* sleep 16ms */
#ifdef WIN32
		Sleep(16);
#else
		nanosleep(&ts, &ts);
#endif

		worldTime += 16.0;
	}

	printf("Done. Took %lld seconds.\n", time(NULL) - t);

	UnrealLiveLink_Unload();

	return 0;
	}

