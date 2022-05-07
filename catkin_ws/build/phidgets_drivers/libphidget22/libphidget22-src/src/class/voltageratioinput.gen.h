#ifndef _VOLTAGERATIOINPUT_H_
#define _VOLTAGERATIOINPUT_H_

/* Generated by WriteClassHeaderVisitor: Mon Aug 16 2021 13:12:04 GMT-0600 (Mountain Daylight Time) */

typedef struct _PhidgetVoltageRatioInput *PhidgetVoltageRatioInputHandle;

/* Methods */
API_PRETURN_HDR PhidgetVoltageRatioInput_create(PhidgetVoltageRatioInputHandle *ch);
API_PRETURN_HDR PhidgetVoltageRatioInput_delete(PhidgetVoltageRatioInputHandle *ch);

/* Properties */
API_PRETURN_HDR PhidgetVoltageRatioInput_setBridgeEnabled(PhidgetVoltageRatioInputHandle ch,
  int bridgeEnabled);
API_PRETURN_HDR PhidgetVoltageRatioInput_getBridgeEnabled(PhidgetVoltageRatioInputHandle ch,
  int *bridgeEnabled);
API_PRETURN_HDR PhidgetVoltageRatioInput_setBridgeGain(PhidgetVoltageRatioInputHandle ch,
  PhidgetVoltageRatioInput_BridgeGain bridgeGain);
API_PRETURN_HDR PhidgetVoltageRatioInput_getBridgeGain(PhidgetVoltageRatioInputHandle ch,
  PhidgetVoltageRatioInput_BridgeGain *bridgeGain);
API_PRETURN_HDR PhidgetVoltageRatioInput_setDataInterval(PhidgetVoltageRatioInputHandle ch,
  uint32_t dataInterval);
API_PRETURN_HDR PhidgetVoltageRatioInput_getDataInterval(PhidgetVoltageRatioInputHandle ch,
  uint32_t *dataInterval);
API_PRETURN_HDR PhidgetVoltageRatioInput_getMinDataInterval(PhidgetVoltageRatioInputHandle ch,
  uint32_t *minDataInterval);
API_PRETURN_HDR PhidgetVoltageRatioInput_getMaxDataInterval(PhidgetVoltageRatioInputHandle ch,
  uint32_t *maxDataInterval);
API_PRETURN_HDR PhidgetVoltageRatioInput_setSensorType(PhidgetVoltageRatioInputHandle ch,
  PhidgetVoltageRatioInput_SensorType sensorType);
API_PRETURN_HDR PhidgetVoltageRatioInput_getSensorType(PhidgetVoltageRatioInputHandle ch,
  PhidgetVoltageRatioInput_SensorType *sensorType);
API_PRETURN_HDR PhidgetVoltageRatioInput_getSensorUnit(PhidgetVoltageRatioInputHandle ch,
  Phidget_UnitInfo *sensorUnit);
API_PRETURN_HDR PhidgetVoltageRatioInput_getSensorValue(PhidgetVoltageRatioInputHandle ch,
  double *sensorValue);
API_PRETURN_HDR PhidgetVoltageRatioInput_setSensorValueChangeTrigger(PhidgetVoltageRatioInputHandle ch,
  double sensorValueChangeTrigger);
API_PRETURN_HDR PhidgetVoltageRatioInput_getSensorValueChangeTrigger(PhidgetVoltageRatioInputHandle ch,
  double *sensorValueChangeTrigger);
API_PRETURN_HDR PhidgetVoltageRatioInput_getVoltageRatio(PhidgetVoltageRatioInputHandle ch,
  double *voltageRatio);
API_PRETURN_HDR PhidgetVoltageRatioInput_getMinVoltageRatio(PhidgetVoltageRatioInputHandle ch,
  double *minVoltageRatio);
API_PRETURN_HDR PhidgetVoltageRatioInput_getMaxVoltageRatio(PhidgetVoltageRatioInputHandle ch,
  double *maxVoltageRatio);
API_PRETURN_HDR PhidgetVoltageRatioInput_setVoltageRatioChangeTrigger(PhidgetVoltageRatioInputHandle ch,
  double voltageRatioChangeTrigger);
API_PRETURN_HDR PhidgetVoltageRatioInput_getVoltageRatioChangeTrigger(PhidgetVoltageRatioInputHandle ch,
  double *voltageRatioChangeTrigger);
API_PRETURN_HDR PhidgetVoltageRatioInput_getMinVoltageRatioChangeTrigger(PhidgetVoltageRatioInputHandle ch, double *minVoltageRatioChangeTrigger);
API_PRETURN_HDR PhidgetVoltageRatioInput_getMaxVoltageRatioChangeTrigger(PhidgetVoltageRatioInputHandle ch, double *maxVoltageRatioChangeTrigger);

/* Events */
typedef void (CCONV *PhidgetVoltageRatioInput_OnSensorChangeCallback)(PhidgetVoltageRatioInputHandle ch,
  void *ctx, double sensorValue, Phidget_UnitInfo *sensorUnit);

API_PRETURN_HDR PhidgetVoltageRatioInput_setOnSensorChangeHandler(PhidgetVoltageRatioInputHandle ch,
  PhidgetVoltageRatioInput_OnSensorChangeCallback fptr, void *ctx);
typedef void (CCONV *PhidgetVoltageRatioInput_OnVoltageRatioChangeCallback)(PhidgetVoltageRatioInputHandle ch, void *ctx, double voltageRatio);

API_PRETURN_HDR PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(PhidgetVoltageRatioInputHandle ch,
  PhidgetVoltageRatioInput_OnVoltageRatioChangeCallback fptr, void *ctx);

#endif /* _VOLTAGERATIOINPUT_H_ */
