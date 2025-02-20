/**
 * @file	efi_gpio.cpp
 * @brief	EFI-related GPIO code
 *
 * @date Sep 26, 2014
 * @author Andrey Belomutskiy, (c) 2012-2018
 */

#include "global.h"
#include "engine.h"
#include "efi_gpio.h"
#include "drivers/gpio/gpio_ext.h"

#if EFI_GPIO_HARDWARE
#include "pin_repository.h"
#include "io_pins.h"
#endif /* EFI_GPIO_HARDWARE */

#if EFI_ELECTRONIC_THROTTLE_BODY
#include "electronic_throttle.h"
#endif /* EFI_ELECTRONIC_THROTTLE_BODY */

EXTERN_ENGINE;

#if EFI_ENGINE_SNIFFER
#include "engine_sniffer.h"
extern WaveChart waveChart;
#endif /* EFI_ENGINE_SNIFFER */

// todo: clean this mess, this should become 'static'/private
EnginePins enginePins;
extern LoggingWithStorage sharedLogger;

pin_output_mode_e DEFAULT_OUTPUT = OM_DEFAULT;

static const char *sparkNames[] = { "coil1", "coil2", "coil3", "coil4", "coil5", "coil6", "coil7", "coil8",
		"coil9", "coil10", "coil11", "coil12"};

// these short names are part of engine sniffer protocol
static const char *sparkShortNames[] = { "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8",
		"c9", "cA", "cB", "cD"};

static const char *injectorNames[] = { "injector1", "injector2", "injector3", "injector4", "injector5", "injector6",
		"injector7", "injector8", "injector9", "injector10", "injector11", "injector12"};

static const char *injectorShortNames[] = { "i1", "i2", "i3", "i4", "i5", "i6", "i7", "i8",
		"j9", "iA", "iB", "iC"};

static const char *auxValveShortNames[] = { "a1", "a2"};

EnginePins::EnginePins() {
	dizzyOutput.name = DIZZY_NAME;
	tachOut.name = TACH_NAME;

	efiAssertVoid(CUSTOM_ERR_PIN_COUNT_TOO_LARGE, (sizeof(sparkNames) / sizeof(char*)) >= IGNITION_PIN_COUNT, "spark pin count");
	for (int i = 0; i < IGNITION_PIN_COUNT;i++) {
		enginePins.coils[i].name = sparkNames[i];
		enginePins.coils[i].shortName = sparkShortNames[i];
	}
	efiAssertVoid(CUSTOM_ERR_PIN_COUNT_TOO_LARGE, (sizeof(injectorNames) / sizeof(char*)) >= INJECTION_PIN_COUNT, "inj pin count");
	for (int i = 0; i < INJECTION_PIN_COUNT;i++) {
		enginePins.injectors[i].injectorIndex = i;
		enginePins.injectors[i].name = injectorNames[i];
		enginePins.injectors[i].shortName = injectorShortNames[i];
	}
	efiAssertVoid(CUSTOM_ERR_PIN_COUNT_TOO_LARGE, (sizeof(auxValveShortNames) / sizeof(char*)) >= AUX_DIGITAL_VALVE_COUNT, "aux pin count");
	for (int i = 0; i < AUX_DIGITAL_VALVE_COUNT;i++) {
		enginePins.auxValve[i].name = auxValveShortNames[i];
	}
}

/**
 * Sets the value of the pin. On this layer the value is assigned as is, without any conversion.
 */

#if EFI_PROD_CODE
#define setPinValue(outputPin, electricalValue, logicValue)                        \
  {                                                                                \
    if ((outputPin)->currentLogicValue != (logicValue)) {                          \
	  palWritePad((outputPin)->port, (outputPin)->pin, (electricalValue));         \
	  (outputPin)->currentLogicValue = (logicValue);                               \
    }                                                                              \
  }

#else /* EFI_PROD_CODE */

#define setPinValue(outputPin, electricalValue, logicValue)                        \
  {                                                                                \
    if ((outputPin)->currentLogicValue != (logicValue)) {                          \
	  (outputPin)->currentLogicValue = (logicValue);                               \
    }                                                                              \
  }
#endif /* EFI_PROD_CODE */

bool EnginePins::stopPins() {
	bool result = false;
	for (int i = 0; i < IGNITION_PIN_COUNT; i++) {
		result |= coils[i].stop();
	}
	for (int i = 0; i < INJECTION_PIN_COUNT; i++) {
		result |= injectors[i].stop();
	}
	for (int i = 0; i < AUX_DIGITAL_VALVE_COUNT; i++) {
		result |= auxValve[i].stop();
	}
	return result;
}

void EnginePins::unregisterPins() {
#if EFI_ELECTRONIC_THROTTLE_BODY
	unregisterEtbPins();
#endif /* EFI_ELECTRONIC_THROTTLE_BODY */
#if EFI_PROD_CODE
	fuelPumpRelay.unregisterOutput(activeConfiguration.bc.fuelPumpPin, engineConfiguration->bc.fuelPumpPin);
	fanRelay.unregisterOutput(activeConfiguration.bc.fanPin, engineConfiguration->bc.fanPin);
	hipCs.unregisterOutput(activeConfiguration.bc.hip9011CsPin, engineConfiguration->bc.hip9011CsPin);
	triggerDecoderErrorPin.unregisterOutput(activeConfiguration.bc.triggerErrorPin,
		engineConfiguration->bc.triggerErrorPin);

	sdCsPin.unregisterOutput(activeConfiguration.bc.sdCardCsPin, engineConfiguration->bc.sdCardCsPin);
	accelerometerCs.unregisterOutput(activeConfiguration.LIS302DLCsPin, engineConfiguration->LIS302DLCsPin);
//	etbOutput1.unregisterOutput(activeConfiguration.bc.etb1.directionPin1,
//			engineConfiguration->bc.etb1.directionPin1);
//	etbOutput2.unregisterOutput(activeConfiguration.bc.etb1.directionPin2,
//			engineConfiguration->bc.etb1.directionPin2);
	checkEnginePin.unregisterOutput(activeConfiguration.bc.malfunctionIndicatorPin,
			engineConfiguration->bc.malfunctionIndicatorPin);
	dizzyOutput.unregisterOutput(activeConfiguration.dizzySparkOutputPin,
			engineConfiguration->dizzySparkOutputPin);
	tachOut.unregisterOutput(activeConfiguration.bc.tachOutputPin,
			engineConfiguration->bc.tachOutputPin);
	idleSolenoidPin.unregisterOutput(activeConfiguration.bc.idle.solenoidPin,
			engineConfiguration->bc.idle.solenoidPin);

	for (int i = 0;i < FSIO_COMMAND_COUNT;i++) {
		fsioOutputs[i].unregisterOutput(activeConfiguration.bc.fsioOutputPins[i],
				engineConfiguration->bc.fsioOutputPins[i]);
	}

	alternatorPin.unregisterOutput(activeConfiguration.bc.alternatorControlPin,
			engineConfiguration->bc.alternatorControlPin);
	mainRelay.unregisterOutput(activeConfiguration.bc.mainRelayPin,
			engineConfiguration->bc.mainRelayPin);

#endif /* EFI_PROD_CODE */
}

void EnginePins::reset() {
	for (int i = 0; i < INJECTION_PIN_COUNT;i++) {
		injectors[i].reset();
	}
	for (int i = 0; i < IGNITION_PIN_COUNT;i++) {
		coils[i].reset();
	}
}

void EnginePins::stopIgnitionPins(void) {
#if EFI_PROD_CODE
	for (int i = 0; i < IGNITION_PIN_COUNT; i++) {
		NamedOutputPin *output = &enginePins.coils[i];
		output->unregisterOutput(activeConfiguration.bc.ignitionPins[i],
				engineConfiguration->bc.ignitionPins[i]);
	}
#endif /* EFI_PROD_CODE */
}

void EnginePins::stopInjectionPins(void) {
#if EFI_PROD_CODE
	for (int i = 0; i < INJECTION_PIN_COUNT; i++) {
		NamedOutputPin *output = &enginePins.injectors[i];
		output->unregisterOutput(activeConfiguration.bc.injectionPins[i],
				engineConfiguration->bc.injectionPins[i]);
	}
#endif /* EFI_PROD_CODE */
}

void EnginePins::startAuxValves(void) {
#if EFI_PROD_CODE
	for (int i = 0; i < AUX_DIGITAL_VALVE_COUNT; i++) {
		NamedOutputPin *output = &enginePins.auxValve[i];
		output->initPin(output->name, engineConfiguration->auxValves[i]);
	}
#endif /* EFI_PROD_CODE */
}

void EnginePins::startIgnitionPins(void) {
#if EFI_PROD_CODE
	for (int i = 0; i < engineConfiguration->specs.cylindersCount; i++) {
		NamedOutputPin *output = &enginePins.coils[i];
		// todo: we need to check if mode has changed
		if (CONFIGB(ignitionPins)[i] != activeConfiguration.bc.ignitionPins[i]) {
			output->initPin(output->name, CONFIGB(ignitionPins)[i],
				&CONFIGB(ignitionPinMode));
		}
	}
	// todo: we need to check if mode has changed
	if (engineConfiguration->dizzySparkOutputPin != activeConfiguration.dizzySparkOutputPin) {
		enginePins.dizzyOutput.initPin("dizzy tach", engineConfiguration->dizzySparkOutputPin,
				&engineConfiguration->dizzySparkOutputPinMode);

	}
#endif /* EFI_PROD_CODE */
}

void EnginePins::startInjectionPins(void) {
#if EFI_PROD_CODE
	// todo: should we move this code closer to the injection logic?
	for (int i = 0; i < engineConfiguration->specs.cylindersCount; i++) {
		NamedOutputPin *output = &enginePins.injectors[i];
		// todo: we need to check if mode has changed
		if (engineConfiguration->bc.injectionPins[i] != activeConfiguration.bc.injectionPins[i]) {

			output->initPin(output->name, CONFIGB(injectionPins)[i],
					&CONFIGB(injectionPinMode));
		}
	}
#endif /* EFI_PROD_CODE */
}

NamedOutputPin::NamedOutputPin() : OutputPin() {
	name = NULL;
}

const char *NamedOutputPin::getName() const {
	return name;
}

const char *NamedOutputPin::getShortName() const {
	return shortName == NULL ? name : shortName;
}

NamedOutputPin::NamedOutputPin(const char *name) : OutputPin() {
	this->name = name;
}

void NamedOutputPin::setHigh() {
#if EFI_DEFAILED_LOGGING
//	signal->hi_time = hTimeNow();
#endif /* EFI_DEFAILED_LOGGING */

	// turn the output level ACTIVE
	setValue(true);

#if EFI_ENGINE_SNIFFER

	addEngineSnifferEvent(getShortName(), WC_UP);
#endif /* EFI_ENGINE_SNIFFER */
}

void NamedOutputPin::setLow() {
	// turn off the output
	setValue(false);

#if EFI_DEFAILED_LOGGING
//	systime_t after = getTimeNowUs();
//	debugInt(&signal->logging, "a_time", after - signal->hi_time);
//	scheduleLogging(&signal->logging);
#endif /* EFI_DEFAILED_LOGGING */

#if EFI_ENGINE_SNIFFER
	addEngineSnifferEvent(getShortName(), WC_DOWN);
#endif /* EFI_ENGINE_SNIFFER */
}

InjectorOutputPin::InjectorOutputPin() : NamedOutputPin() {
	reset();
	injectorIndex = -1;
}

bool NamedOutputPin::stop() {
#if EFI_GPIO_HARDWARE
	if (isInitialized() && getLogicValue()) {
		setValue(false);
		scheduleMsg(&sharedLogger, "turning off %s", name);
		return true;
	}
#endif /* EFI_GPIO_HARDWARE */
	return false;
}

void InjectorOutputPin::reset() {
	overlappingScheduleOffTime = 0;
	cancelNextTurningInjectorOff = false;
	overlappingCounter = 0;
	// todo: this could be refactored by calling some super-reset method
	currentLogicValue = INITIAL_PIN_STATE;
}

IgnitionOutputPin::IgnitionOutputPin() {
	reset();
}

void IgnitionOutputPin::reset() {
	outOfOrder = false;
	signalFallSparkId = 0;
}

OutputPin::OutputPin() {
	modePtr = &DEFAULT_OUTPUT;
#if EFI_GPIO_HARDWARE
	port = NULL;
	pin = 0;
#endif /* EFI_GPIO_HARDWARE */
	currentLogicValue = INITIAL_PIN_STATE;
}

bool OutputPin::isInitialized() {
#if EFI_GPIO_HARDWARE
#if (BOARD_EXT_GPIOCHIPS > 0)
	if (ext)
		return true;
#endif /* (BOARD_EXT_GPIOCHIPS > 0) */
	return port != NULL;
#else /* EFI_GPIO_HARDWARE */
	return true;
#endif /* EFI_GPIO_HARDWARE */
}

void OutputPin::toggle() {
	setValue(!getLogicValue());

}
void OutputPin::setValue(int logicValue) {
#if EFI_PROD_CODE
	efiAssertVoid(CUSTOM_ERR_6621, modePtr!=NULL, "pin mode not initialized");
	pin_output_mode_e mode = *modePtr;
	efiAssertVoid(CUSTOM_ERR_6622, mode <= OM_OPENDRAIN_INVERTED, "invalid pin_output_mode_e");
	int eValue = getElectricalValue(logicValue, mode);

	#if (BOARD_EXT_GPIOCHIPS > 0)
		if (!this->ext) {
			/* onchip pin */
			if (port != GPIO_NULL) {
				setPinValue(this, eValue, logicValue);
			}
		} else {
			/* external pin */
			gpiochips_writePad(this->brainPin, logicValue);
			/* TODO: check return value */
			currentLogicValue = logicValue;
		}
	#else
		if (port != GPIO_NULL) {
			setPinValue(this, eValue, logicValue);
		}
	#endif

#else /* EFI_PROD_CODE */
	setPinValue(this, eValue, logicValue);
#endif /* EFI_PROD_CODE */
}

bool OutputPin::getLogicValue() const {
	return currentLogicValue;
}

void OutputPin::setDefaultPinState(const pin_output_mode_e *outputMode) {
	pin_output_mode_e mode = *outputMode;
	/* may be*/UNUSED(mode);
	assertOMode(mode);
	this->modePtr = outputMode;
	setValue(false); // initial state
}

void initOutputPins(void) {
#if EFI_GPIO_HARDWARE
	/**
	 * want to make sure it's all zeros so that we can compare in initOutputPinExt() method
	 */
// todo: it's too late to clear now? this breaks default status LEDs
// todo: fix this?
//	memset(&outputs, 0, sizeof(outputs));

#if HAL_USE_SPI
	enginePins.sdCsPin.initPin("spi CS5", CONFIGB(sdCardCsPin));
#endif /* HAL_USE_SPI */

	// todo: should we move this code closer to the fuel pump logic?
	enginePins.fuelPumpRelay.initPin("fuel pump relay", CONFIGB(fuelPumpPin), &CONFIGB(fuelPumpPinMode));

	enginePins.mainRelay.initPin("main relay", CONFIGB(mainRelayPin), &CONFIGB(mainRelayPinMode));

	enginePins.fanRelay.initPin("fan relay", CONFIGB(fanPin), &CONFIGB(fanPinMode));
	enginePins.o2heater.initPin("o2 heater", CONFIGB(o2heaterPin));
	enginePins.acRelay.initPin("A/C relay", CONFIGB(acRelayPin), &CONFIGB(acRelayPinMode));

	// digit 1
	/*
	 ledRegister(LED_HUGE_0, GPIOB, 2);
	 ledRegister(LED_HUGE_1, GPIOE, 7);
	 ledRegister(LED_HUGE_2, GPIOE, 8);
	 ledRegister(LED_HUGE_3, GPIOE, 9);
	 ledRegister(LED_HUGE_4, GPIOE, 10);
	 ledRegister(LED_HUGE_5, GPIOE, 11);
	 ledRegister(LED_HUGE_6, GPIOE, 12);

	 // digit 2
	 ledRegister(LED_HUGE_7, GPIOE, 13);
	 ledRegister(LED_HUGE_8, GPIOE, 14);
	 ledRegister(LED_HUGE_9, GPIOE, 15);
	 ledRegister(LED_HUGE_10, GPIOB, 10);
	 ledRegister(LED_HUGE_11, GPIOB, 11);
	 ledRegister(LED_HUGE_12, GPIOB, 12);
	 ledRegister(LED_HUGE_13, GPIOB, 13);

	 // digit 3
	 ledRegister(LED_HUGE_14, GPIOE, 0);
	 ledRegister(LED_HUGE_15, GPIOE, 2);
	 ledRegister(LED_HUGE_16, GPIOE, 4);
	 ledRegister(LED_HUGE_17, GPIOE, 6);
	 ledRegister(LED_HUGE_18, GPIOE, 5);
	 ledRegister(LED_HUGE_19, GPIOE, 3);
	 ledRegister(LED_HUGE_20, GPIOE, 1);
	 */
#endif /* EFI_GPIO_HARDWARE */
}

void OutputPin::initPin(const char *msg, brain_pin_e brainPin) {
	initPin(msg, brainPin, &DEFAULT_OUTPUT);
}

void OutputPin::initPin(const char *msg, brain_pin_e brainPin, const pin_output_mode_e *outputMode) {
#if EFI_GPIO_HARDWARE
	if (brainPin == GPIO_UNASSIGNED)
		return;

	assertOMode(*outputMode);
	iomode_t mode = (*outputMode == OM_DEFAULT || *outputMode == OM_INVERTED) ?
		PAL_MODE_OUTPUT_PUSHPULL : PAL_MODE_OUTPUT_OPENDRAIN;

	#if (BOARD_EXT_GPIOCHIPS > 0)
		this->ext = false;
	#endif
	if (brain_pin_is_onchip(brainPin)) {
		ioportid_t port = getHwPort(msg, brainPin);
		int pin = getHwPin(msg, brainPin);

		/**
		 * This method is used for digital GPIO pins only, for peripheral pins see mySetPadMode
		 */
		if (port == GPIO_NULL) {
			// that's for GRIO_NONE
			this->port = port;
			return;
		}

		/**
		 * @brief Initialize the hardware output pin while also assigning it a logical name
		 */
		if (this->port != NULL && (this->port != port || this->pin != pin)) {
			/**
			 * here we check if another physical pin is already assigned to this logical output
			 */
		// todo: need to clear '&outputs' in io_pins.c
			warning(CUSTOM_OBD_PIN_CONFLICT, "outputPin [%s] already assigned to %x%d", msg, this->port, this->pin);
			engine->withError = true;
			return;
		}
		this->port = port;
		this->pin = pin;
	}
	#if (BOARD_EXT_GPIOCHIPS > 0)
		else {
			this->ext = true;
			this->brainPin = brainPin;
		}
	#endif

	this->currentLogicValue = INITIAL_PIN_STATE;

	// The order of the next two calls may look strange, which is a good observation.
	// We call them in this order so that the pin is set to a known state BEFORE
	// it's enabled.  Enabling the pin then setting it could result in a (brief)
	// mystery state being driven on the pin (potentially dangerous).
	setDefaultPinState(outputMode);
	efiSetPadMode(msg, brainPin, mode);
#endif /* EFI_GPIO_HARDWARE */
}

#if EFI_GPIO_HARDWARE

void initPrimaryPins(void) {
	enginePins.errorLedPin.initPin("led: ERROR status", LED_ERROR_BRAIN_PIN);
}

/**
 * This method is part of fatal error handling.
 * Please note that worst case scenario the pins might get re-enabled by some other code :(
 * The whole method is pretty naive, but that's at least something.
 */
void turnAllPinsOff(void) {
	for (int i = 0; i < INJECTION_PIN_COUNT; i++) {
		enginePins.injectors[i].setValue(false);
	}
	for (int i = 0; i < IGNITION_PIN_COUNT; i++) {
		enginePins.coils[i].setValue(false);
	}
}

#else /* EFI_GPIO_HARDWARE */
const char *hwPortname(brain_pin_e brainPin) {
	(void)brainPin;
	return "N/A";
}
#endif /* EFI_GPIO_HARDWARE */
