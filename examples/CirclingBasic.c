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
#define CIRCLES 100

/* size in Unreal units of the circle on the X-Y plane */
#define CIRCLE_RADIUS 200

/* number of steps to perform the circles */
#define STEP_COUNT 100000

/* Z axis location -- number of units above origin */
#define HEIGHT 100

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

	rc = UnrealLiveLink_Load(sharedObj, "CirclingBasic");
	if (rc != UNREAL_LIVE_LINK_OK)
	{
		printf("error: unable to load %s\n", sharedObj);
		return 1;
	}

	printf("Starting...\n");
	t = time(NULL);


	/* set up basic live link role */
	UnrealLiveLink_Name prop_names[PROP_COUNT] = { "property1", "property2" };
	struct UnrealLiveLink_Properties properties = { prop_names, PROP_COUNT };
	UnrealLiveLink_SetBasicStructure("circle", &properties);



	/* initialize the per frame basic structure */
	float values[PROP_COUNT] = { 0.0, 0.0 };
	struct UnrealLiveLink_PropertyValues prop_values = { values, PROP_COUNT };

	struct UnrealLiveLink_Transform xform;
	UnrealLiveLink_InitTransform(&xform);
	xform.translation[2] = HEIGHT;


	/* calculate the radian step */
	const double step = 2.0 * M_PI * CIRCLES / STEP_COUNT;

	/* loop STEP_COUNT times sending data every 16ms (not exact) */
	for (i = 0; i < STEP_COUNT; i++)
	{
		for (i = 0; i < PROP_COUNT; i++) {
			values[i] = (float)sin(angle)* CIRCLE_RADIUS;
		}

		UnrealLiveLink_UpdateBasicFrame("circle", worldTime, 0, &prop_values);

		//xform.translation[0] = (float)sin(angle) * CIRCLE_RADIUS;
		//xform.translation[1] = (float)cos(angle) * CIRCLE_RADIUS;
		//UnrealLiveLink_UpdateTransformFrame("circle", worldTime, 0, 0, &xform);

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

