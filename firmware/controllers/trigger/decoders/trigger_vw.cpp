/*
 * @file trigger_vw.cpp
 *
 * @date Aug 25, 2018
 * @author Andrey Belomutskiy, (c) 2012-2018
 */

#include "trigger_vw.h"
#include "trigger_universal.h"

void setVwConfiguration(TriggerShape *s) {
	efiAssertVoid(CUSTOM_ERR_6660, s != NULL, "TriggerShape is NULL");

	s->initialize(FOUR_STROKE_CRANK_SENSOR, false);

	s->isSynchronizationNeeded = true;


	int totalTeethCount = 60;
	int skippedCount = 2;

	float engineCycle = FOUR_STROKE_ENGINE_CYCLE;
	float toothWidth = 0.5;

	addSkippedToothTriggerEvents(T_PRIMARY, s, 60, 2, toothWidth, 0, engineCycle,
			NO_LEFT_FILTER, 690);

	float angleDown = engineCycle / totalTeethCount * (totalTeethCount - skippedCount - 1 + (1 - toothWidth) );
	s->addEventClamped(0 + angleDown + 12, T_PRIMARY, TV_RISE, NO_LEFT_FILTER, NO_RIGHT_FILTER);
	s->addEventClamped(0 + engineCycle, T_PRIMARY, TV_FALL, NO_LEFT_FILTER, NO_RIGHT_FILTER);

	s->setTriggerSynchronizationGap2(1.6, 4);
}
