/**
 * @file	trigger_mazda.cpp
 *
 * @date Feb 18, 2014
 * @author Andrey Belomutskiy, (c) 2012-2018
 *
 * This file is part of rusEfi - see http://rusefi.com
 *
 * rusEfi is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * rusEfi is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "trigger_mazda.h"

void initializeMazdaMiataNaShape(TriggerShape *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, true);
	s->setTriggerSynchronizationGap2(1.4930 * 0.6f, 1.4930 * 1.3f);
	s->useRiseEdge = false;

	s->bothFrontsRequired = true;

	s->tdcPosition = 294;

	s->isSynchronizationNeeded = true;

	/**
	 * http://rusefi.com/forum/viewtopic.php?f=3&t=729&p=12983#p12983
	 */
	s->addEvent720(52.960405, T_SECONDARY, TV_RISE);
	s->addEvent720(122.635956, T_SECONDARY, TV_FALL);

	s->addEvent720(216.897031, T_PRIMARY, TV_RISE);
	s->addEvent720(232.640068, T_SECONDARY, TV_RISE);
	s->addEvent720(288.819688, T_PRIMARY, TV_FALL);
	s->addEvent720(302.646323, T_SECONDARY, TV_FALL);

	s->addEvent720(412.448056, T_SECONDARY, TV_RISE);
	s->addEvent720(482.816719, T_SECONDARY, TV_FALL);

	s->addEvent720(577.035495, T_PRIMARY, TV_RISE);
	s->addEvent720(592.878113, T_SECONDARY, TV_RISE);
	s->addEvent720(662.899708, T_SECONDARY, TV_FALL);
	s->addEvent720(720.0f, T_PRIMARY, TV_FALL);
	s->useOnlyPrimaryForSync = true;
}

/**
 * by alexander-n8hgeg5e
 * See https://rusefi.com/forum/viewtopic.php?f=5&t=1447
 */
void initialize_Mazda_Engine_z5_Shape(TriggerShape *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, false);
	/**
	 * My Signal is:      60,      60,      102,     60
	 *               120,     120,     120,      78,
	 *                                              ^  
	 *                                              |
	 *                                              sync point = 0 degree from now on 
	 * All rising edges are 60 befor some OT.
	 * If the edge goes high, it should look at the last past 2 events. (high-low-now)
	 * time1/time2 == 78/102 = 13/17 then triggerevent '0' would be nice.
	 * 
	 */
	s->useRiseEdge = false;
	s->tdcPosition = 0; // 1 and 3 are at top , so 0 or 360
	s->setTriggerSynchronizationGap(0.7);

	s->addEvent720(60.0f,   T_PRIMARY, TV_FALL);
	s->addEvent720(180.0f,  T_PRIMARY, TV_RISE);

	s->addEvent720(240.0f,  T_PRIMARY, TV_FALL);
	s->addEvent720(360.0f,  T_PRIMARY, TV_RISE);

	s->addEvent720(420.0f,  T_PRIMARY, TV_FALL);
	s->addEvent720(540.0f,  T_PRIMARY, TV_RISE);

	s->addEvent720(618.0f,  T_PRIMARY, TV_FALL);
	s->addEvent720(720.0f,  T_PRIMARY, TV_RISE);
}

// TT_MIATA_VVT
void initializeMazdaMiataNb2Crank(TriggerShape *s) {
	s->initialize(FOUR_STROKE_SYMMETRICAL_CRANK_SENSOR, false);

	float o = 160;

	s->tdcPosition = 60 + 655;

	s->setTriggerSynchronizationGap2(0.35f, 0.98f);
	s->addEvent720(o + 4 * 56.0f, T_PRIMARY, TV_FALL);
	s->addEvent720(o + 4 * 60.0f, T_PRIMARY, TV_RISE);
	s->addEvent720(o + 4 * 136.0f, T_PRIMARY, TV_FALL);
	s->addEvent720(o + 4 * 140.0f, T_PRIMARY, TV_RISE);
}

static void initializeMazdaMiataNb1ShapeWithOffset(TriggerShape *s, float offset) {
	s->initialize(FOUR_STROKE_CAM_SENSOR, true);
	s->setTriggerSynchronizationGap(0.11f);
	s->useRiseEdge = false;

	s->bothFrontsRequired = true;

	s->invertOnAdd = true;
	s->tdcPosition = 276;

	/**
	 * cam sensor is primary, crank sensor is secondary
	 */
	s->addEvent720(20.0f, T_PRIMARY, TV_RISE);

	s->addEvent720(offset + 66.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 70.0f, T_SECONDARY, TV_RISE);
	s->addEvent720(offset + 136.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 140.0f, T_SECONDARY, TV_RISE);
	s->addEvent720(offset + 246.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 250.0f, T_SECONDARY, TV_RISE);
	s->addEvent720(offset + 316.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 320.0f, T_SECONDARY, TV_RISE);

	s->addEvent720(340.0f, T_PRIMARY, TV_FALL);
	s->addEvent720(360.0f, T_PRIMARY, TV_RISE);

	s->addEvent720(380.0f, T_PRIMARY, TV_FALL);
	s->addEvent720(400.0f, T_PRIMARY, TV_RISE);

	s->addEvent720(offset + 426.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 430.0f, T_SECONDARY, TV_RISE);
	s->addEvent720(offset + 496.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 500.0f, T_SECONDARY, TV_RISE);
	s->addEvent720(offset + 606.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 610.0f, T_SECONDARY, TV_RISE);
	s->addEvent720(offset + 676.0f, T_SECONDARY, TV_FALL);
	s->addEvent720(offset + 680.0f, T_SECONDARY, TV_RISE);

	s->addEvent720(720.0f, T_PRIMARY, TV_FALL);
	s->useOnlyPrimaryForSync = true;
}

void initializeMazdaMiataNb1Shape(TriggerShape *s) {
	initializeMazdaMiataNb1ShapeWithOffset(s, 0);
}

void initializeMazdaMiataVVtTestShape(TriggerShape *s) {
	initializeMazdaMiataNb1ShapeWithOffset(s, -22);
}

void configureMazdaProtegeSOHC(TriggerShape *s) {

	// todo: move to into configuration definition s->needSecondTriggerInput = FALSE;

	s->initialize(FOUR_STROKE_CAM_SENSOR, false);
//	s->initialState[0] = 1;

//	float w = 720 / 4 * 0.215;
	float a = 5;

	s->bothFrontsRequired = true;

	float z = 0.093;
	a = 180;
	s->addEvent720(a - z * 720, T_PRIMARY, TV_RISE);
	s->addEvent720(a, T_PRIMARY, TV_FALL);

	a += 180;
	s->addEvent720(a - z * 720, T_PRIMARY, TV_RISE);
	s->addEvent720(a, T_PRIMARY, TV_FALL);
	a += 180;
	s->addEvent720(a - z * 720, T_PRIMARY, TV_RISE);
	s->addEvent720(a, T_PRIMARY, TV_FALL);
	a += 180;
	s->addEvent720(a - z * 720, T_PRIMARY, TV_RISE);
	s->addEvent720(a, T_PRIMARY, TV_FALL);

	s->isSynchronizationNeeded = false;
}

void configureMazdaProtegeLx(TriggerShape *s) {
	// todo: move to into configuration definition s->needSecondTriggerInput = FALSE;
	s->initialize(FOUR_STROKE_CAM_SENSOR, true);
	s->useOnlyPrimaryForSync = true;

	/**
	 * based on https://svn.code.sf.net/p/rusefi/code/trunk/misc/logs/1993_escort_gt/MAIN_rfi_report_2015-02-01%2017_39.csv
	 */
	s->addEvent720(95.329254, T_PRIMARY, TV_RISE);

	s->addEvent720(95.329254 + 14.876692, T_SECONDARY, TV_RISE);
	s->addEvent720(95.329254 + 82.693557, T_SECONDARY, TV_FALL);

	s->addEvent720(95.329254 + 137.119154, T_PRIMARY, TV_FALL);

	s->addEvent720(95.329254 + 192.378308, T_SECONDARY, TV_RISE);
	s->addEvent720(95.329254 + 261.556418, T_SECONDARY, TV_FALL);

	s->addEvent720(95.329254 + 373.060597, T_SECONDARY, TV_RISE);
	s->addEvent720(95.329254 + 443.503184, T_SECONDARY, TV_FALL);

	s->addEvent720(95.329254 + 555.349776, T_SECONDARY, TV_RISE);
	s->addEvent720(720, T_SECONDARY, TV_FALL);

	s->tdcPosition = 137.119154;
	s->isSynchronizationNeeded = false;
}
